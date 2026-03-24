#include "main.h"
#include "syscall.h"
void puts(char *s) ;
void overflow(void); 


__attribute__((section(".user_text")))

void user_main() {
    puts("Hello world from user land!\n");
        putc('A');
    putc('B');
    putc('C');
    putc('\n');
    overflow();
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

__attribute__((section(".user_text")))
void overflow() {
    volatile int keep_going = 1;
    if (keep_going) overflow();
}