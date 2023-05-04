#include "context.h"
#include <sys/mman.h>
#include <cerrno>
#include <cstring>

Context::~Context() {
    if (rsp) {
        munmap(reinterpret_cast<void *>(rip - STACK_SIZE), STACK_SIZE);
    }
}

Context Context::create_context() {
    Context result{};
    auto new_memory = mmap(nullptr, STACK_SIZE, PROT_READ | PROT_WRITE,
                                                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_memory == MAP_FAILED) {
        printf("%s\n", strerror(errno));
    }
    result.rsp = (unsigned long) new_memory + STACK_SIZE;
    return result;
}

void Context::setRip(unsigned long rip) {
    Context::rip = rip;
}
