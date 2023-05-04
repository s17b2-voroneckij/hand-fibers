#include <stdio.h>
#include "context.h"

Context main_context;
Context other;


void f() {
    printf("Hello from inside context\n");
    switch_context(&other, &main_context);
}

int main() {
    other = Context::create_context();
    other.setRip(reinterpret_cast<unsigned long>(&f));
    switch_context(&main_context, &other);
    printf("back in main\n");

    return 0;
}