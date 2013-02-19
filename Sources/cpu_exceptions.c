#include <Core/Core.h>

#include "cpu.h"

extern void gdb_converse(unsigned int trapno, struct ia32_regs *t);



void do_page_fault(u32 *esp, u32 error_code, u32 addr) {
  dna_printf("TODO : do_page_fault");
  for (;;)
    ;
}

void do_double_fault(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_double_fault");
  for (;;)
    ;
}

void do_general_protection(u32 *esp, u32 error_code) {
  u32 eip, cs, eflags;

  eip = esp[0];
  cs = esp[1];
  eflags = esp[2];

  dna_printf("0x%lx\n", eip);
  dna_printf("0x%lx\n", cs);
  dna_printf("0x%lx\n", eflags);

  dna_printf("#GPF: esp=0x%lx, error_code=0x%lx\n", esp, error_code);
  for (;;)
    ;
}

void do_divide_error(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_divide_error");
  for (;;)
    ;
}

void do_int3(u32 *esp, u32 error_code, u32 fs, u32 es, u32 ds, u32 ebp,
    u32 esi, u32 edi, u32 edx, u32 ecx, u32 ebx, u32 eax) {
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

  gdb_converse(EXC_BP, &regs);

  // converse modifies eip, eflags
  esp[0] = regs.eip;
  esp[2] = regs.eflags;
}

void do_nmi(u32 esp, u32 error_code) {
  dna_printf("TODO : do_nmi");
  for (;;)
    ;
}

void do_debug(u32 *esp, u32 error_code, u32 fs, u32 es, u32 ds, u32 ebp,
    u32 esi, u32 edi, u32 edx, u32 ecx, u32 ebx, u32 eax) {
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

  gdb_converse(EXC_DB, &regs);

  // converse modifies eip, eflags
  esp[0] = regs.eip;
  esp[2] = regs.eflags;
}

void do_overflow(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_overflow");
  for (;;)
    ;
}

void do_bounds(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_bounds");
  for (;;)
    ;
}

void do_invalid_op(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_invalid_op");
  for (;;)
    ;
}

void do_coprocessor_segment_overrun(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_coprocessor_segment_overrun");
  for (;;)
    ;
}

void do_invalid_TSS(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_invalid_TSS");
  for (;;)
    ;
}

void do_segment_not_present(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_segment_not_present");
  for (;;)
    ;
}

void do_stack_segment(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_stack_segment");
  for (;;)
    ;
}

void do_coprocessor_error(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_coprocessor_error");
  for (;;)
    ;
}

void do_reserved(u32 *esp, u32 error_code) {
  dna_printf("TODO : do_reserved");
  for (;;)
    ;
}
