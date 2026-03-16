#ifndef MMU_H
#define MMU_H
#include "idt.h"
void setup_mmu(void);
void page_fault_handler(int_regs_t *r);
void *page_to_addr(int page);
int   addr_to_page(void *addr);
void *get_cr2();
extern char _begin_of_user_space;
extern char _end_of_user_space;
extern char _user_stack_guard;
#endif