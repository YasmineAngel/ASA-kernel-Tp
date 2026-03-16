#include <stdint.h>
#include "mmu.h"
#include "main.h"

extern char _begin_of_user_space;
extern char _end_of_user_space;
extern char _user_stack_guard;

int page_alloc[4096];
int first_free_page;


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

#define PAGE_SIZE 4096 
struct Directory_Entry_s page_directory[1024] __attribute__((aligned (PAGE_SIZE)));
struct Page_Entry_s page_entry[1024] __attribute__((aligned (PAGE_SIZE)));
struct Page_Entry_s user_page_table[1024] __attribute__((aligned (PAGE_SIZE)));

void setup_mmu() {

    for (int i = 0; i < 1024; i++) {
        page_directory[i].valid_entry = 0;
    }
    for (int j = 0; j < 1024; j++) {
        page_entry[j].physical_page = j;  
        page_entry[j].valid = 1;          
        page_entry[j].read_write = 1;     
        page_entry[j].mode = 0;           
    }

    for (int j = 0; j < 1024; j++) {
        user_page_table[j].valid = 0;
    }


    int first_page = addr_to_page((void *)&_begin_of_user_space);
    int last_page  = addr_to_page((void *)&_end_of_user_space);

    for (int j = first_page; j < last_page; j++) {
        int idx = j - 1024;
        user_page_table[idx].physical_page = j; 
        user_page_table[idx].valid = 1;          
        user_page_table[idx].read_write = 1;
        user_page_table[idx].mode = 1;
    }

    uint32_t guard_addr = ((uint32_t)&_user_stack_guard + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    int guard_idx = addr_to_page((void *)guard_addr) - 1024;
    user_page_table[guard_idx].valid = 0;

    page_directory[0].physical_page = ((uint32_t)page_entry) >> 12;
    page_directory[0].valid_entry = 1;
    page_directory[0].read_write = 1;
    page_directory[0].mode = 0;

    page_directory[1].physical_page = ((uint32_t)user_page_table) >> 12;
    page_directory[1].valid_entry = 1;
    page_directory[1].read_write = 1;
    page_directory[1].mode = 1;

    uint32_t pd_addr = (uint32_t)page_directory;
    asm volatile("mov %0, %%cr3" : : "r"(pd_addr));
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80010000; 
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void page_fault_handler(const int_regs_t *r) {

    uint32_t faulting_address = (uint32_t)get_cr2();
    uint32_t stack_bottom = (uint32_t)&_user_stack_guard;
    uint32_t guard_addr = (stack_bottom + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    if (faulting_address >= guard_addr && faulting_address < guard_addr + PAGE_SIZE) {
        kputs("\nStack overflow\n");
    } else {
    kputs("\nPage fault\n");
    }
    kputs("Error accessing address: ");
    puthex(faulting_address);
    kputs("\nInstruction pointer (EIP): ");
    puthex(r->eip);
    kputs("\n");
    for (;;) {
        asm volatile("hlt");
    }

}

void *page_to_addr(int page) {
    return (void *)(page * PAGE_SIZE);
}

int addr_to_page(void *addr) {
    return (uint32_t)addr / PAGE_SIZE;
}

void *get_cr2() {
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    return (void *)cr2;
}

