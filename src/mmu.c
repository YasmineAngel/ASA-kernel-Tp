
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

}