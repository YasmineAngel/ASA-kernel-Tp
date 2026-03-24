#ifndef CTX_H
#define CTX_H
#include <stdint.h>
#include <stddef.h>
#include "mmu.h"

struct ctx_s {
    int pid;           
    uint32_t cr3;        
    uint32_t esp;         
    uint32_t ebp;        
    struct ctx_s *next;
};

extern struct ctx_s *current_ctx;

#endif