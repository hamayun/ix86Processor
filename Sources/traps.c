#include "cpu.h"

extern void gdb_converse(unsigned int trapno, struct ia32_regs *t);

void do_debug(long * esp, long error_code, long fs, long es, long ds, long ebp,
    long esi, long edi, long edx, long ecx, long ebx, long eax) {
  struct ia32_regs regs;
  int eip, cs, eflags;

  eip = esp[0];
  cs = esp[1];
  eflags = esp[2];

  regs.eip = eip;
  regs.ebp = ebp;
  regs.esp = (int) esp + 12;
  regs.eflags = eflags;
  regs.eax = eax;
  regs.ebx = ebx;
  regs.ecx = ecx;
  regs.edx = edx;
  regs.edi = edi;

  regs.cs = cs;
  regs.fs = 0x0000;
  regs.gs = 0x0000;
  regs.es = es;
  regs.ds = ds;
  regs.esi = esi;
  regs.ss = KERNEL_DS;

  //dna_printf("TRAP: DEBUG\n");
  gdb_converse(EXC_DB, &regs);

  // converse modifies eip, eflags
  esp[0] = regs.eip;
  esp[2] = regs.eflags;
}

void do_int3(long * esp, long error_code, long fs, long es, long ds, long ebp,
    long esi, long edi, long edx, long ecx, long ebx, long eax) {
  struct ia32_regs regs;
  int eip, cs, eflags;

  eip = esp[0];
  cs = esp[1];
  eflags = esp[2];

  regs.eip = eip;
  regs.ebp = ebp;
  regs.esp = (int) esp + 12;
  regs.eflags = eflags;
  regs.eax = eax;
  regs.ebx = ebx;
  regs.ecx = ecx;
  regs.edx = edx;
  regs.edi = edi;

  regs.cs = cs;
  regs.fs = 0x0000;
  regs.gs = 0x0000;
  regs.es = es;
  regs.ds = ds;
  regs.esi = esi;
  regs.ss = KERNEL_DS;

  //dna_printf("TRAP: BREAKPOINT\n");
  gdb_converse(EXC_BP, &regs);

  // converse modifies eip, eflags
  esp[0] = regs.eip;
  esp[2] = regs.eflags;
}

