#include <stdint.h>
#include "mmu.h"
#include "main.h"

extern char _begin_of_user_space;
extern char _end_of_user_space;
extern char _user_stack_guard;

#define MAX_PAGES 4096
int page_alloc[MAX_PAGES];
int first_free_page;



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

    for (int i = 0; i < last_page; i++) {
    page_alloc[i] = -1;
    }

    for (int i = last_page; i < MAX_PAGES - 1; i++) {
    page_alloc[i] = i + 1;
    }
    page_alloc[MAX_PAGES - 1] = -1;

    first_free_page = last_page;

    uint32_t pd_addr = (uint32_t)page_directory;
    asm volatile("mov %0, %%cr3" : : "r"(pd_addr));
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80010000; 
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void page_fault_handler(const int_regs_t *r) {
    
    uint32_t faulting_address = (uint32_t)get_cr2();
    uint32_t stack_top   = 0x00800000;
    uint32_t stack_limit = stack_top - 0x100000;
    
    if (faulting_address > stack_limit && faulting_address < stack_top) {
        kputs("\nAllocating new page !\n");
        void *new_page = alloc_page();

        int idx2 = addr_to_page((void *)faulting_address) - 1024;
        user_page_table[idx2].physical_page = addr_to_page(new_page);
        user_page_table[idx2].valid = 1;
        user_page_table[idx2].read_write = 1;
        user_page_table[idx2].mode = 1;

        uint32_t cr3;
        asm volatile("mov %%cr3, %0" : "=r"(cr3));
        asm volatile("mov %0, %%cr3" : : "r"(cr3));

        return;

    } else if (faulting_address >= stack_limit - PAGE_SIZE && faulting_address <= stack_limit) {
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

void *alloc_page() {
    if (first_free_page == -1) {
        return (void *)-1;  
    }
    int page = first_free_page;
    first_free_page = page_alloc[page];
    page_alloc[page] = -1;
    return page_to_addr(page);
}

void free_page(void *addr) {
    int page = addr_to_page(addr);
    page_alloc[page] = first_free_page;
    first_free_page = page;
}


void page_copy(char *pg_src, char *pg_dst) {
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    for (int i = 0; i < PAGE_SIZE; i++) {
        pg_dst[i] = pg_src[i];
    }

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}


struct Page_Directory_s *clone_page_directory(struct Page_Directory_s *src_pd){

    struct Page_Directory_s *new_pd = alloc_page();
    struct Page_Entry_s *new_upt = alloc_page();

    new_pd->entries[0] = src_pd->entries[0];

    for (int i = 1; i < 1024; i++) {
        new_pd->entries[i].valid_entry = 0;
    }

    new_pd->entries[1].physical_page = ((uint32_t)new_upt) >> 12;
    new_pd->entries[1].valid_entry = 1;
    new_pd->entries[1].read_write = 1;
    new_pd->entries[1].mode = 1;

    struct Page_Entry_s *src_upt = (struct Page_Entry_s *)(src_pd->entries[1].physical_page << 12);

    for (int i = 0; i < 1024; i++) {
        if (src_upt[i].valid) {
            char *new_page = alloc_page();
            char *src_page = (char *)(src_upt[i].physical_page << 12);
            page_copy(src_page, new_page);
            new_upt[i].physical_page = ((uint32_t)new_page) >> 12;
            new_upt[i].valid = 1;
            new_upt[i].read_write = 1;
            new_upt[i].mode = 1;
        } else {
            new_upt[i].valid = 0;
        }
    }

    return new_pd;
}