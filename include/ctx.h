#ifndef CTX_H
#define CTX_H
#include <stdint.h>
#include <stddef.h>
#include "mmu.h"

#define MAX_CTX 10

struct ctx_s {
    int pid;           
    uint32_t cr3;        
    uint32_t esp;         
    uint32_t ebp;        
    struct ctx_s *next;
};

extern struct ctx_s *current_ctx;

void init_ctx();
struct ctx_s *alloc_ctx();
void free_ctx(struct ctx_s *ctx);
void switch_to(struct ctx_s *ctx);
void kyield();

#endif