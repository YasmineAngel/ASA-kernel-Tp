
struct Directory_Entry_s {
    int physical_page : 20;
    int not_used : 5 ;
    int page_size : 1 ;
    int used_entries : 1 ;
    int cache_disabled : 1 ;
    int cache_management : 1 ;
    int mode : 1 ;
    int read_write : 1 ;
    int valid_entry : 1 ;

};

struct Page_Entry_s {
    int physical_page : 20;
    int not_used : 4 ;
    int global : 1 ;
    int dirty_bit : 1 ;
    int read_page : 1 ;
    int cache_disabled : 1 ;
    int cache_management : 1;
    int mode : 1 ;
    int read_write : 1 ;
    int valid : 1 ;

};

#define PAGE_SIZE 4096 
struct Directory_Entry_s page_directory[1024] __attribute__((aligned (PAGE_SIZE)));
struct Page_Entry_s page_entry[1024] __attribute__((aligned (PAGE_SIZE)));

