#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>

#define __packed __attribute__((packed))

struct ELF64Header
{
    struct
    {
        union
        {
            char magic_bytes[4];
            uint32_t magic_number;
        };

        uint8_t eclass, data, version, osabi, abiversion;
        uint8_t pad[7];
    } ident __packed;

    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry_point;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __packed;

typedef struct {
    uint32_t	sh_name;
    uint32_t	sh_type;
    uint64_t	sh_flags;
    uint64_t	sh_addr;
    uint64_t	sh_offset;
    uint64_t	sh_size;
    uint32_t	sh_link;
    uint32_t	sh_info;
    uint64_t	sh_addralign;
    uint64_t	sh_entsize;
} Elf64_Shdr;

static void* EH_FRAME_START;
static void* NEXT_SECTION_START;
static Elf64_Shdr tbss;
static Elf64_Shdr tdata;
static const char* ELF_NAME;
static uint64_t thread_local_size;
static bool initialised = false;

static char* tdata_data;

ssize_t my_read(int fd, char* buf, ssize_t size, ssize_t offset = 0) {
    if (offset != 0) {
        auto ret = lseek(fd, offset, SEEK_SET);
        if (ret == (off_t)-1) {
            throw strerror(errno);
        }
    }
    ssize_t ready_bytes = 0;
    while (ready_bytes < size) {
        auto ret = read(fd, buf + ready_bytes, size - ready_bytes);
        if (ret <= 0) {
            return ready_bytes;
        }
        ready_bytes += ret;
    }
    return ready_bytes;
}

uint64_t alignup(uint64_t a, uint64_t alignment) {
    return ((a - 1) / alignment + 1) * alignment;
}

void free_tdata() {
    if (tdata_data) {
        free(tdata_data);
    }
}

void process_elf() {
    if (initialised) {
        return;
    }
    // reading metadata from ELF
    auto fd = open("/proc/self/exe", O_RDONLY);
    ELF64Header hdr;
    my_read(fd, (char*)&hdr, sizeof(ELF64Header));
    Elf64_Shdr string_table_section;
    my_read(fd, (char*)&string_table_section, sizeof(Elf64_Shdr), hdr.shoff + hdr.shstrndx * sizeof(Elf64_Shdr));
    char strings[string_table_section.sh_size];
    my_read(fd, strings, string_table_section.sh_size, string_table_section.sh_offset);
    bool previos_eh_frame = false;
    for (int i = 1; i < hdr.shnum; i++) {
        Elf64_Shdr section_header;
        my_read(fd, (char*)&section_header, sizeof(Elf64_Shdr), hdr.shoff + i * sizeof(Elf64_Shdr));
        char* name = &strings[section_header.sh_name];
        if (previos_eh_frame) {
            NEXT_SECTION_START = (void *)section_header.sh_addr;
            previos_eh_frame = false;
        }
        if (strcmp(name, ".eh_frame") == 0) {
            previos_eh_frame = true;
            EH_FRAME_START = (void *)section_header.sh_addr;
        }
        else if (strcmp(name, ".tdata") == 0) {
            tdata = section_header;
        }
        else if (strcmp(name, ".tbss") == 0) {
            tbss = section_header;
        }
    }

    // now let`s read .tdata

    thread_local_size = tbss.sh_size + tdata.sh_size;
    if (thread_local_size == 0) {
        return ;
    }
    uint64_t alignment = tdata.sh_addralign;
    if (tbss.sh_addralign > alignment) {
        alignment = tbss.sh_addralign;
    }
    thread_local_size = alignup(thread_local_size, alignment);

    if (tdata.sh_size) {
        tdata_data = (char *) malloc(tdata.sh_size);
        atexit(free_tdata);
        my_read(fd, tdata_data, tdata.sh_size, tdata.sh_offset);
    }

    initialised = true;
}

char* prepare_thread_local() {
    // returns the value to be written into %fs
    process_elf();
    if (thread_local_size == 0) {
        return nullptr;
    }
    uint64_t alignment = tdata.sh_addralign;
    if (tbss.sh_addralign > alignment) {
        alignment = tbss.sh_addralign;
    }
    auto addr = (char *)malloc(thread_local_size + sizeof(long) + alignment + 4096);
    auto result = addr + thread_local_size;
    *(unsigned long *)(addr + thread_local_size) = (unsigned long)(addr + thread_local_size);
    memcpy(addr, tdata_data, tdata.sh_size);
    memset(addr + tdata.sh_size, 0, tbss.sh_size);
    return result;
}

void clean_up_thread_local(char* fs_value) {
    if (thread_local_size == 0) {
        return;
    }
    free((void *)(fs_value - thread_local_size));
}