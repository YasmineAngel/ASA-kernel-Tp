#ifndef MMU_H
#define MMU_H
#include "idt.h"
void setup_mmu(void);
void page_fault_handler(int_regs_t *r);
void *page_to_addr(int page);
int   addr_to_page(void *addr);

#endif