#include <Processor/Processor.h>
#include <string.h>

#include "i8259.h"

struct pseudo_descriptor {
    unsigned short limit;
    unsigned long linear_base;
} __attribute__ ((aligned(2), packed));

#define _set_gate(gate_addr, type, dpl, addr)           \
    __asm__ __volatile__ ( "movw %%dx, %%ax\n\t"        \
            "movw %0, %%dx\n\t"                         \
            "movl %%eax, %1\n\t"                        \
            "movl %%edx, %2"                            \
            ::                                          \
            "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
            "o" (*((char *) (gate_addr))),              \
            "o" (*(4+(char *) (gate_addr))),            \
            "d" ((char *) (addr)),"a" (KERNEL_CS << 16))

#define set_intr_gate(n, addr) _set_gate(&idt[n], 14, 0, addr)

#define set_trap_gate(n, addr) _set_gate(&idt[n], 15, 0, addr)

#define set_system_gate(n, addr) _set_gate(&idt[n], 15, 3, addr)

#define IDT_ENTRIES 256
extern unsigned long long idt[IDT_ENTRIES];
extern struct pseudo_descriptor idt_desc;

extern void divide_err(void);
extern void illegal_opcode(void);
extern void protection_fault(void);
extern void timer_int(void);
extern void ipi_int(void);
extern void lapic_error_int(void);

#define KERNEL_CS 0x10

#define LOCAL_TIMER_VECTOR 0XEF
#define LAPIC_ERROR_VECTOR 0xEE
#define LAPIC_IPI_VECTOR 0xED

extern unsigned long i8259_irq[16];

void cpu_idt_init(void) {
    int i;

    memset(idt, 0, sizeof(idt));

    set_intr_gate(0, &divide_err);
    set_intr_gate(6, &illegal_opcode);
    set_intr_gate(13, &protection_fault);

    set_intr_gate(LOCAL_TIMER_VECTOR, &timer_int);
    set_intr_gate(LAPIC_IPI_VECTOR, &ipi_int);
    set_intr_gate(LAPIC_ERROR_VECTOR, &lapic_error_int);

    for(i=0; i<16; i++) {
        set_intr_gate(i8259_VECTOR_OFFSET + i, i8259_irq[i]);
    }

    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.linear_base = (unsigned long) &idt;

    __asm__ __volatile__("lidt %0" :: "m" (idt_desc.limit));
}

