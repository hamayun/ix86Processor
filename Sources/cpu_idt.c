#include <Processor/Processor.h>
#include <string.h>

#include "cpu.h"

#include "i8259.h"
#include "io_apic.h"

#define KERNEL_CS 0x10

#define IDT_ENTRIES 256
extern unsigned long long idt[IDT_ENTRIES];
extern struct pseudo_descriptor idtdesc;

enum {
  GATE_INTERRUPT = 0xE,
  GATE_TRAP = 0xF,
  GATE_CALL = 0xC,
  GATE_TASK = 0x5,
};

struct desc_struct {
  union {
    struct {
      unsigned int a;
      unsigned int b;
    };
    struct {
      uint16_t limit0;
      uint16_t base0;
      unsigned base1: 8, type: 4, s: 1, dpl: 2, p: 1;
      unsigned limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
    };
  };
} __attribute__((packed));

typedef struct desc_struct gate_desc;

static inline void write_idt_entry(gate_desc *idt, int entry, const gate_desc *gate) {
      memcpy(&idt[entry], gate, sizeof(*gate));
}

static inline void pack_gate(gate_desc *gate, unsigned char type,
           unsigned long base, unsigned dpl, unsigned flags,
           unsigned short seg) {
  gate->a = (seg << 16) | (base & 0xffff);
  gate->b = (base & 0xffff0000) | (((0x80 | type | (dpl << 5)) & 0xff) << 8);
}

static inline void _set_gate(int gate, unsigned type, void *addr,
            unsigned dpl, unsigned ist, unsigned seg) {
    gate_desc s;

    pack_gate(&s, type, (unsigned long)addr, dpl, ist, seg);

    write_idt_entry((void *) idt, gate, &s);
}

static inline void set_intr_gate(unsigned int n, void *addr) {
  _set_gate(n, GATE_INTERRUPT, addr, 0, 0, KERNEL_CS);
}

static inline void set_system_intr_gate(unsigned int n, void *addr) {
  _set_gate(n, GATE_INTERRUPT, addr, 0x3, 0, KERNEL_CS);
}

static inline void set_trap_gate(unsigned int n, void *addr) {
  _set_gate(n, GATE_TRAP, addr, 0, 0, KERNEL_CS);
}

extern void divide_err(void);
extern void illegal_opcode(void);
extern void protection_fault(void);
extern void timer_int(void);
extern void ipi_int(void);
extern void lapic_error_int(void);
extern void ignore_int(void);

extern void debug(void);
extern void int3(void);

#define LOCAL_TIMER_VECTOR 0XEF
#define LAPIC_ERROR_VECTOR 0xEE
#define LAPIC_IPI_VECTOR 0xED

extern void *i8259_irq[16];
extern void *io_apic_irq[24];

void cpu_idt_init(void) {
    int i;

    for(i=0; i<IDT_ENTRIES; i++) {
        set_intr_gate(i, &ignore_int);    
    }

    /* exceptions */
    set_trap_gate(0, &divide_error);
    set_trap_gate(1, &debug);
    set_trap_gate(2, &nmi);
    set_system_intr_gate(3, &int3);
    set_system_intr_gate(4, &overflow);
    set_system_intr_gate(5, &bounds);
    set_trap_gate(6, &invalid_op);
    set_trap_gate(7, &device_not_available);
    set_trap_gate(8, &double_fault);
    set_trap_gate(9, &coprocessor_segment_overrun);
    set_trap_gate(10, &invalid_TSS);
    set_trap_gate(11, &segment_not_present);
    set_trap_gate(12, &stack_segment);
    set_trap_gate(13, &general_protection);
    //set_trap_gate(14, &page_fault);
    set_trap_gate(15, &reserved);
    set_trap_gate(16, &coprocessor_error); 

    set_intr_gate(LOCAL_TIMER_VECTOR, &timer_int);
    set_intr_gate(LAPIC_IPI_VECTOR, &ipi_int);
    set_intr_gate(LAPIC_ERROR_VECTOR, &lapic_error_int);

    for(i=0; i<16; i++) {
        set_intr_gate(i8259_VECTOR_OFFSET + i, i8259_irq[i]);
    }

    for(i=0; i<24; i++) {
        set_intr_gate(IO_APIC_VECTOR_OFFSET + i, io_apic_irq[i]);
    }

    idtdesc.limit = sizeof(idt) - 1;
    idtdesc.linear_base = (unsigned long) &idt;

    __asm__ __volatile__("lidt %0" :: "m" (idtdesc.limit));
}

