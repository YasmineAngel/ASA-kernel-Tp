#include "ctx.h"
#include "mmu.h"
#include "main.h"

struct ctx_s *current_ctx;
struct ctx_s ctxs[MAX_CTX];
struct ctx_s *first_free_ctx = NULL;
struct ctx_s *ctx_to_free = NULL;
struct Page_Directory_s *pd_to_free = NULL;

static int next_pid = 1;

void init_ctx() {
    for (int i = 0; i < MAX_CTX - 1; i++) {
        ctxs[i].next = &ctxs[i + 1];
    }
    ctxs[MAX_CTX - 1].next = NULL;
    first_free_ctx = &ctxs[0];
}

struct ctx_s *alloc_ctx() {
    if (first_free_ctx == NULL) {
        return NULL;
    }
    struct ctx_s *ctx = first_free_ctx;
    first_free_ctx = first_free_ctx->next;
    ctx->pid = next_pid++;
    return ctx;
}

void free_ctx(struct ctx_s *ctx) {
    ctx->next = first_free_ctx;
    first_free_ctx = ctx;
}

int switch_to(struct ctx_s *ctx) {
    asm volatile(
        "mov %%esp, %0\n"
        "mov %%ebp, %1\n"
        : "=r"(current_ctx->esp), "=r"(current_ctx->ebp)
    );

    if (ctx == NULL) {
        struct ctx_s *clone = alloc_ctx();
        clone->cr3 = (uint32_t)clone_page_directory(
            (struct Page_Directory_s *)current_ctx->cr3
        );
        clone->esp = current_ctx->esp;
        clone->ebp = current_ctx->ebp;
        clone->next = current_ctx->next;
        current_ctx->next = clone;

        return clone->pid; 
    }

    current_ctx = ctx;

    asm volatile(
        "mov %0, %%cr3\n"
        "mov %1, %%esp\n"
        "mov %2, %%ebp\n"
        : : "r"(ctx->cr3), "r"(ctx->esp), "r"(ctx->ebp)
    );
    
    if (ctx_to_free != NULL) {
        free_page_directory(pd_to_free);
        free_ctx(ctx_to_free);
        ctx_to_free = NULL;
        pd_to_free = NULL;
    }

    return 0; 
}

void kyield() {
    switch_to(current_ctx->next);
}

int kfork() {
   return switch_to(NULL);
}

void kexit() {
    struct ctx_s *prev = current_ctx;
    
    while (prev->next != current_ctx) {
        prev = prev->next;
    }

    if (prev == current_ctx) {
        extern void kputs(char *);
        kputs("\nAll processes finished. System halted.\n");
        for(;;) asm volatile("hlt");
    }

    prev->next = current_ctx->next;

    ctx_to_free = current_ctx;
    pd_to_free = (struct Page_Directory_s *)current_ctx->cr3;

    switch_to(current_ctx->next);
    
    for(;;);
}