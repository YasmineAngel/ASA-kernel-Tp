#ifndef MMU_H
#define MMU_H
#include "idt.h"

struct Directory_Entry_s {
    uint32_t valid_entry      : 1 ;
    uint32_t read_write       : 1 ;
    uint32_t mode             : 1 ;
    uint32_t cache_management : 1 ;
    uint32_t cache_disabled   : 1 ;
    uint32_t used_entries     : 1 ;
    uint32_t ignored          : 1 ;
    uint32_t page_size        : 1 ;
    uint32_t not_used         : 4 ;
    uint32_t physical_page    : 20 ;
};

struct Page_Entry_s {
    uint32_t valid            : 1 ;
    uint32_t read_write       : 1 ;
    uint32_t mode             : 1 ;
    uint32_t cache_management : 1 ;
    uint32_t cache_disabled   : 1 ;
    uint32_t read_page        : 1 ;
    uint32_t dirty_bit        : 1 ;
    uint32_t ignored          : 1 ;
    uint32_t global           : 1 ;
    uint32_t not_used         : 3 ;
    uint32_t physical_page    : 20 ;
};

struct Page_Directory_s {
    struct Directory_Entry_s entries[1024];
};
void setup_mmu(void);
void page_fault_handler(int_regs_t *r);
void *page_to_addr(int page);
int   addr_to_page(void *addr);
void *get_cr2();
void *alloc_page();
void free_page(void *addr);
void page_copy(char *pg_src, char *pg_dst);
struct Page_Directory_s *clone_page_directory(struct Page_Directory_s *src_pd);
extern char _begin_of_user_space;
extern char _end_of_user_space;
extern char _user_stack_guard;
#endif