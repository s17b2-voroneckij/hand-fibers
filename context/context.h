#pragma once

#include <cstdio>

class Context {
public:
    static const ssize_t STACK_SIZE = 4096 * 2;

    Context() = default;
    static Context create_context();
    ~Context();

    void setRip(unsigned long rip);

private:
    unsigned long rbx;
    unsigned long rsp;
    unsigned long rbp;
    unsigned long r12;
    unsigned long r13;
    unsigned long r14;
    unsigned long r15;
    unsigned long rip;
};

extern "C" {
/*
 * save_context saves the context into the structure provided
 * rip is filled in with the caller`s caller return address
 */
//extern void save_context(Context* context);
/*
 * load_context loads context into registers and returns into rip
 */
//extern void load_context(Context* context);
/*
 * saves current context into old_context_dest and loads new_context
 */
extern void switch_context(Context* old_context_dest, Context* new_context, unsigned long first_arg = 0);
}
