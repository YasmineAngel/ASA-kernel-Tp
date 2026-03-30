#include "main.h"
#include "syscall.h"
#include "ctx.h"
void puts(char *s) ;
void overflow(void); 
int getpid(void);
void yield(void);


__attribute__((section(".user_text")))

void user_main() {
    puts("Hello world from user land!\n");
        putc('A');
    putc('B');
    putc('C');
    putc('\n');
    int pid = getpid();
    puts("pid: ");
    putc('0' + pid);
    putc('\n');
    //overflow();
    yield();
    puts("back from yield!\n");
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

__attribute__((section(".user_text")))
int getpid() {
    int pid =0;
    asm volatile(
        "int $87"
        :
        : "d"(&pid), "a"(3)
    );
    return pid;
}

__attribute__((section(".user_text")))
void yield() {
    asm volatile(
        "int $87"
        :
        : "a"(4)
    );
}