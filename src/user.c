#include "main.h"
#include "syscall.h"
void puts(char *s) ;


char user_stack[65536] __attribute__((section(".user_data")));
__attribute__((section(".user_text")))
void user_main() {
    puts("Hello world from user land!\n");
        putc('A');
    putc('B');
    putc('C');
    putc('\n');
    for(;;);

}

__attribute__((section(".user_text")))
void puts(char *s) {
    asm volatile(
        "int $87\n"
        :
        : "d"(s), "a"(0)
    );
}

__attribute__((section(".user_text")))
void putc(char c) {
    asm volatile(
        "int $87"
        : 
        : "d"(c), "a"(1)
    );
}

__attribute__((section(".user_text")))
void clear_screen(void) {
    asm volatile(
        "int $87" 
        : 
        : "a"(2)
    );
}