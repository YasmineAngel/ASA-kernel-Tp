#include "main.h"
#include "syscall.h"
#include "ctx.h"
void puts(char *s) ;
void overflow(void); 
int getpid(void);
void yield(void);
int fork(void);
void exit(void);
void user_main(void); 

__attribute__((section(".user_text")))
void _entry_point() {
    user_main(); 
    exit();     
}

__attribute__((section(".user_text")))
void user_main() {
    puts("Hello world from user land!\n");
    
    int pid = fork();
    
    if (pid == 0) {
        puts("I am the child process! Returning naturally now...\n");
        return; 
    } else {
        puts("I am the parent process, child pid is: ");
        putc('0' + pid);
        putc('\n');
    }
    
    yield();
    puts("Parent : back from yield!\n");
    
    puts("Parent returning naturally now...\n");
    return; 
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


int fork(void);

__attribute__((section(".user_text")))
int fork() {
    int ret;
    asm volatile(
        "int $87"
        : "=a"(ret)
        : "a"(5)
    );
    return ret;
}

__attribute__((section(".user_text")))
void exit() {
    asm volatile(
        "int $87"
        :
        : "a"(SYS_EXIT)
    );
    for(;;);
}
