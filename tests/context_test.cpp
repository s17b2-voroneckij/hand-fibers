#include <stdio.h>
#include <cstdint>
#include "../context/context.h"

Context main_context;
Context other;


void f() {
    printf("Hello from inside context\n");
    switch_context(&other, &main_context);
}

extern "C" {
extern uint64_t rdfsbase();
extern uint64_t rdgsbase();
extern void wrfsbase(uint64_t val);
extern void wrgsbase(uint64_t val);
}

int main() {
    other = Context::create_context();
    other.setRip(reinterpret_cast<unsigned long>(&f));
    switch_context(&main_context, &other);
    printf("back in main\n");

    return 0;
}