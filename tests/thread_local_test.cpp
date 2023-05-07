#include <stdio.h>
#include <cstdint>
#include <string>
#include "../context/context.h"
#include <unistd.h>
#include <cstring>

Context main_context;
Context other;

thread_local int b = 2;
thread_local int a = 1;

#include <asm/unistd.h>      // compile without -m32 for 64 bit call numbers
// #define __NR_write 1
ssize_t my_write(int fd, const void *buf, size_t size)
{
    ssize_t ret;
    asm volatile
            (
            "syscall"
            : "=a" (ret)
        //                 EDI      RSI       RDX
            : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
            : "rcx", "r11", "memory"
            );
    return ret;
}


void f() {
    char out[] = "Hello from inside context\n";
    //write(2, out, 26);
    printf("Hello from inside context\n");
    printf("In context a is %d\n", a);
    a++;
    printf("now, In context a is %d\n", a);
    switch_context(&other, &main_context);
}


int main() {
    other = Context::create_context();
    other.setRip(reinterpret_cast<unsigned long>(&f));
    printf("Before switching context a is %d\n", a);
    switch_context(&main_context, &other);
    printf("After switching context a is %d\n", a);
    printf("back in main\n");

    return 0;
}