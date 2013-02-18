#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "cpu.h"

typedef struct BreakpointStruct {
  unsigned char saved; // saved instruction (which was replaced by a breakpoint)
  unsigned char * address;
  void * data;
} Breakpoint;
#define MAX_BREAKPOINT 1024

void add_breakpoint(void * addr, void * data);
Breakpoint * get_breakpoint(void * addr);
void remove_breakpoint(void * addr);
void remove_all_breakpoints(void);

void mask_breakpoint(void * addr);
void unmask_breakpoint(void * addr);

void mask_all_breakpoints(void);
void unmask_all_breakpoints(void);

char dbg_recv(void);
void dbg_send(char c);
void dbg_flush(void);

#define do_step(regs) regs->eflags |= 0x100
#define do_continue(regs) regs->eflags &= ~0x100

static inline void pokeint(int * address, int value) {
  *address = value;
}

static inline int peekint(int * address) {
  return *address;
}

static inline void pokeref(void * address, void * ref) {
  pokeint(address, ref);
}

static inline void * peekref(int * address) {
  return peekint(address);
}

static inline unsigned char peekbyte(unsigned char * address) {
  return *address;
}

static inline unsigned char pokebyte(unsigned char * address, unsigned char value) {
  *address = value;
}

#define pokebreakpoint pokebyte
#define peekbreakpoint peekbyte
static inline restore_instruction_pointer_after_breakpoint(Registers * regs) {
  INSTRUCTION_POINTER(regs) -= 1;
}

#endif
