#include <cstdint>

asm(
        ".global rdfsbase\n"
        "rdfsbase:\n"
        "   rdfsbase    %rax\n"
        "   ret\n"
        );

asm(
        ".global rdgsbase\n"
        "rdgsbase:\n"
        "   rdgsbase    %rax\n"
        "   ret\n"
        );

asm(
        ".global wrfsbase\n"
        "wrfsbase:\n"
        "   wrfsbase    %rdi\n"
        "   ret\n"
        );

asm(
        ".global wrgsbase\n"
        "wrgsbase:\n"
        "   wrgsbase    %rdi\n"
        "   ret\n"
        );

extern "C" {
extern uint64_t rdfsbase();
extern uint64_t rdgsbase();
extern void wrfsbase(uint64_t val);
extern void wrgsbase(uint64_t val);
}