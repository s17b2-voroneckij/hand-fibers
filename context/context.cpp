#include "context.h"
#include <sys/mman.h>
#include <cerrno>
#include <cstring>

extern void clean_up_thread_local(char* fs_value);

Context::~Context() {
    if (rsp) {
        munmap(reinterpret_cast<void *>(rip - STACK_SIZE), STACK_SIZE);
        if (init_fs) {
            clean_up_thread_local(reinterpret_cast<char *>(fs));
        }
    }
}

extern char* prepare_thread_local();

Context Context::create_context() {
    Context result{};
    auto new_memory = mmap(nullptr, STACK_SIZE * 2, PROT_READ | PROT_WRITE,
                                                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_memory == MAP_FAILED) {
        printf("%s\n", strerror(errno));
    }
    result.rsp = (unsigned long) new_memory + STACK_SIZE;
    result.fs = reinterpret_cast<unsigned long>(prepare_thread_local());
    result.init_fs = true;
    return result;
}

void Context::setRip(unsigned long rip) {
    Context::rip = rip;
}

unsigned long Context::getFs() const {
    return fs;
}

void Context::setFs(unsigned long fs) {
    Context::fs = fs;
}

Context::Context(Context &&other) {
    rbx = other.rbx;
    rsp = other.rsp;
    rbp = other.rbp;
    r12 = other.r12;
    r13 = other.r13;
    r14 = other.r14;
    r15 = other.r15;
    rip = other.rip;
    fs = other.fs;
    init_fs = other.init_fs;
    other.init_fs = false;
    other.rsp = 0;
}

Context &Context::operator=(Context &&other)  noexcept {
    rbx = other.rbx;
    rsp = other.rsp;
    rbp = other.rbp;
    r12 = other.r12;
    r13 = other.r13;
    r14 = other.r14;
    r15 = other.r15;
    rip = other.rip;
    fs = other.fs;
    init_fs = other.init_fs;
    other.init_fs = false;
    other.rsp = 0;
    return *this;
}
