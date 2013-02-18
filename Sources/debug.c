#include <Core/Core.h>
#include <Platform/Platform.h>
#include <stdlib.h>

#include "i8259.h"

#include "debug.h"
#include "uart.h"

#define IRQ_COM1 4

static int verbose = 0;
static int verbose_rcv = 0;
static Breakpoint breakpoints[MAX_BREAKPOINT] = { { 0, 0 }, }; // zero filled

#define RECV_BUFFER_SIZE 1024
static volatile unsigned char recv_buffer[RECV_BUFFER_SIZE];
static volatile int recv_start = 0;
static volatile int recv_end = 0;
static int recv_init = 0;

void dbg_send(char c) {
  uart_write(DEBUGGER_COM_PORT, c);
}

void dbg_flush(void) {
}

void foo(char c) {
  int next = (recv_end + 1) % RECV_BUFFER_SIZE;
  if (next == recv_start) {
    dna_printf("panic: buffer overflow in debugger ...\n");
    while(1);
  }

  recv_buffer[recv_end] = c;
  recv_end = next;
}

void recv_char_irq(long* esp, int irq) {
  dna_printf("recv_char_irq...\n");
  if((inb(DEBUGGER_COM_PORT + 5) & 1) != 0) {
    unsigned char c = inb(DEBUGGER_COM_PORT) & 0xff;
    foo(c);
  }
}

static void init_serial_callback(void) {
  if (recv_init == 0) {
    //isr_i8259[IRQ_COM1] = recv_char_irq;
    //i8259_enable(IRQ_COM1);
    recv_init = 1;
  }
}

char dbg_recv(void) {
  init_serial_callback();

  while (recv_start == recv_end) {
    //if(native_isInterruptsEnabled()) {
    //  halt();
    //}
    //else {
      char c = uart_read(DEBUGGER_COM_PORT);
      foo(c);
    //}
  }

  char c = recv_buffer[recv_start];
  recv_start = (recv_start + 1) % RECV_BUFFER_SIZE;
  //printf("0x%02x\n",c);
  return c;
}

void add_breakpoint(void * addr, void * data) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++) {
    if (breakpoints[i].address == NULL) {
      breakpoints[i].address = (unsigned char *) addr;
      breakpoints[i].saved = *breakpoints[i].address; // saves instruction value
      *breakpoints[i].address = INSN_BREAKPOINT;
      if (verbose)
        printf("breakpoint added @ %p\n", breakpoints[i].address);
      break;
    }
  }
}

void remove_breakpoint(void * addr) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++) {
    if (breakpoints[i].address == (unsigned char *) addr) {
      *breakpoints[i].address = breakpoints[i].saved; // restores instruction value
      if (verbose)
        printf("breakpoint removed @ %p\n", breakpoints[i].address);
      breakpoints[i].address = NULL;
      return;
    }
  }
  if (verbose)
    printf("hum... didn't find the breakpoint to remove @ %p\n", breakpoints[i].address);
  // if(i == MAX_BREAKPOINT)
  //panic("breakpoint to remove not found @ 0x%08x\n", addr);
}

void remove_all_breakpoints(void) {
  if(verbose)
    printf("removing all breakpoints ...\n");

  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++)
    if (breakpoints[i].address != NULL) {
      *breakpoints[i].address = breakpoints[i].saved; // restores instruction value
      if (verbose)
        printf("breakpoint removed @ %p\n", breakpoints[i].address);
      breakpoints[i].address = NULL;
      return;
    }
}

void mask_breakpoint(void * addr) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++)
    if (breakpoints[i].address == (unsigned char *) addr) {
      *breakpoints[i].address = breakpoints[i].saved;
      if (verbose)
        printf("breakpoint masked @ %p\n", breakpoints[i].address);
      return;
    }
  printf("FYI: breakpoint to mask not found @ %p", addr);
}

void unmask_breakpoint(void * addr) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++)
    if (breakpoints[i].address == (unsigned char *) addr) {
      *breakpoints[i].address = INSN_BREAKPOINT;
      if (verbose)
        printf("breakpoint unmasked @ %p\n", breakpoints[i].address);
      return;
    }
  printf("FYI: breakpoint to unmask not found @ %p", addr);
}

void mask_all_breakpoints(void) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++)
    if (breakpoints[i].address != NULL)
      *breakpoints[i].address = breakpoints[i].saved;
}

void unmask_all_breakpoints(void) {
  int i;
  for (i = 0; i < MAX_BREAKPOINT; i++)
    if (breakpoints[i].address != NULL)
      *breakpoints[i].address = INSN_BREAKPOINT;
}
