#include <stdint.h>
#include "mmu.h"
#include "main.h"



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
        user_page_table[j].physical_page = 1024 + j; 
        user_page_table[j].valid = 1;          
        user_page_table[j].read_write = 1;
        user_page_table[j].mode = 1;
    }

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
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    puts("\nPage fault\n");
    puts("Error accessing address: ");
    puthex(faulting_address);
    puts("\nInstruction pointer (EIP): ");
    puthex(r->eip);
    puts("\n");
    for (;;) {
        asm volatile("hlt");
    }
}



