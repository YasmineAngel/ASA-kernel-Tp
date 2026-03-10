#include "main.h"
void puts(char *s) ;


char user_stack[65536] __attribute__((section(".user_data")));
__attribute__((section(".user_text")))
void user_main() {
    puts("Hello world from user land!\n");
    while(1) {
    }
}

__attribute__((section(".user_text")))
void puts(char *s) {
    asm volatile(
        "int $87\n"
        :
        : "d"(s)
    );
}