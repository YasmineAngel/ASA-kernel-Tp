#include "ioport.h"
#include "gdt.h"
#include "idt.h"
#include "mmu.h"
#include "call_user.h"
#include "syscall.h"
#include "ctx.h"

extern char _user_start;
extern char _user_end;
extern void _entry_point();
void kclear_screen();				/* clear screen */
void kputc(char aChar);				/* print a single char on screen */
void kputs(char *aString);			/* print a string on the screen */
void puthex(unsigned aNumber);			/* print an Hex number on screen */
void sys_call_handler(int_regs_t *r);


void empty_irq(int_regs_t *r) {
}

/* multiboot entry-point with datastructure as arg. */
void main(unsigned int * mboot_info)
{
    /* clear the screen */
    kclear_screen();
    kputs("Early boot.\n"); 
    kputs("\t-> Setting up the GDT... ");
    gdt_init_default();
    kputs("OK\n");

    kputs("\t-> Setting up the IDT... ");
    setup_idt();
    kputs("OK\n");
    idt_setup_int_handler(14, page_fault_handler);
    idt_setup_int_handler(87, sys_call_handler);
    kputs("\t-> Setting up the MMU... ");
    setup_mmu();
    kputs("OK\n");

    idt_setup_irq_handler(0, empty_irq);
    idt_setup_irq_handler(1, empty_irq);

    __asm volatile("sti");

    /* minimal setup done ! */
    init_ctx();
    struct ctx_s *ctx = alloc_ctx();
    ctx->cr3 = (uint32_t)page_directory;
    ctx->next = ctx;
    current_ctx = ctx;
    kputs("Jumping to user land...\n");
    kputs("_user_start is at: ");
    puthex((uint32_t)&_user_start);
    kputs("\n");
    kputs("calling user\n");
    call_user((void (*)())&_user_start, (void*)0x00800000);
    kputs("Going idle\n");
    for(;;) ; /* nothing more to do... really nothing ! */
}

/* base address for the video output assume to be set as character oriented by the multiboot */
unsigned char *video_memory = (unsigned char *) 0xB8000;

/* clear screen */
void kclear_screen() {
  int i;
  for(i=0;i<80*25;i++) { 			/* for each one of the 80 char by 25 lines */
    video_memory[i*2+1]=0x0F;			/* color is set to black background and white char */
    video_memory[i*2]=(unsigned char)' '; 	/* character shown is the space char */
  }
}

/* print a string on the screen */
void kputs(char *aString) {
  char *current_char=aString;
  while(*current_char!=0) {
    kputc(*current_char++);
  }
}

/* print an number in hexa */
char *hex_digit="0123456789ABCDEF";
void puthex(unsigned aNumber) {
  int i;
  int started=0;
  for(i=28;i>=0;i-=4) {
    int k=(aNumber>>i)&0xF;
    if(k!=0 || started) {
      kputc(hex_digit[k]);
      started=1;
    }
  }
  if(!started) kputc('0');
}

/* print a char on the screen */
int cursor_x=0;					/* here is the cursor position on X [0..79] */
int cursor_y=0;					/* here is the cursor position on Y [0..24] */

void setCursor() {
  int cursor_offset = cursor_x+cursor_y*80;
  _outb(0x3d4,14);
  _outb(0x3d5,((cursor_offset>>8)&0xFF));
  _outb(0x3d4,15);
  _outb(0x3d5,(cursor_offset&0xFF));
}

void kputc(char c) {
  if(cursor_x>79) {
    cursor_x=0;
    cursor_y++;
  }
  if(cursor_y>24) {
    cursor_y=0;
    kclear_screen();
  }
  switch(c) {					/* deal with a special char */
    case '\r': cursor_x=0; break;		/* carriage return */
    case '\n': cursor_x=0; cursor_y++; break; 	/* new ligne */	
    case 0x8 : if(cursor_x>0) cursor_x--; break;/* backspace */
    case 0x9 : cursor_x=(cursor_x+8)&~7; break;	/* tabulation */
						/* or print a simple character */
    default  : 
      video_memory[(cursor_x+80*cursor_y)*2]=c;
      cursor_x++;
      break;
  }
  setCursor();
}

void sys_call_handler(int_regs_t *r) {
  switch (r->eax) {
        case SYS_PUTS:
            kputs((char *)r->edx);
            break;
        case SYS_PUTC:
            kputc((char)r->edx);
            break;
        case SYS_CLEAR:
            kclear_screen();
            break;
        case SYS_GETPID:
            *((int *)r->edx) = current_ctx->pid;
            break;
        case SYS_YIELD:
            kyield();
            break;
        case SYS_FORK:
            *(uint32_t *)(&r->eax) = kfork();
            break;
        case SYS_EXIT:
            kexit();
            break;
    }
}

