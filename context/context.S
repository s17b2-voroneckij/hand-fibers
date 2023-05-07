    .text

    .global switch_context
switch_context:
// saving current context
    mov     %rbx, (%rdi)
    mov     %rsp, 8(%rdi)
    mov     %rbp, 16(%rdi)
    mov     %r12, 24(%rdi)
    mov     %r13, 32(%rdi)
    mov     %r14, 40(%rdi)
    mov     %r15, 48(%rdi)
    mov     (%rsp), %rax
    mov     %rax, 56(%rdi)
    rdfsbase    %rax
    mov     %rax, 64(%rdi)

    mov     %rsi, %rdi
// restoring other context
    mov     (%rdi), %rbx
    mov     8(%rdi), %rsp
    mov     16(%rdi), %rbp
    mov     24(%rdi), %r12
    mov     32(%rdi), %r13
    mov     40(%rdi), %r14
    mov     48(%rdi), %r15
    mov     56(%rdi), %rax
    mov     64(%rdi), %rsi
    wrfsbase    %rsi
    mov     %rax, (%rsp)
    mov     %rdx, %rdi
    ret