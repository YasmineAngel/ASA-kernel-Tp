#include "ctx.h"
#include "mmu.h"


struct ctx_s *current_ctx ;
struct ctx_s ctxs[MAX_CTX];
struct ctx_s *first_free_ctx = NULL;

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

void switch_to(struct ctx_s *ctx) {
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

        return;
    }


    current_ctx = ctx;

    asm volatile(
        "mov %0, %%cr3\n"
        "mov %1, %%esp\n"
        "mov %2, %%ebp\n"
        :
        : "r"(ctx->cr3), "r"(ctx->esp), "r"(ctx->ebp)
    );
}

void kyield() {
    switch_to(current_ctx->next);
}

void kfork() {
    switch_to(NULL);
}