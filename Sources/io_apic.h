#ifndef _I82093AA_H_
#define _I82093AA_H_

extern unsigned long IO_APIC_VECTOR_OFFSET; // must be defined in linker script

struct IO_APIC_route_entry {
    uint32_t vector    :  8,
    delivery_mode :  3, /* 000: FIXED
           * 001: lowest prio
           * 111: ExtINT
           */
    dest_mode :  1, /* 0: physical, 1: logical */
    delivery_status :  1,
    polarity  :  1,
    irr   :  1,
    trigger   :  1, /* 0: edge, 1: level */
    mask    :  1, /* 0: enabled, 1: disabled */
    __reserved_2  : 15;

    uint32_t __reserved_3  : 24,
    dest    :  8;
} __attribute__ ((packed));

void io_apic_init(void);
void io_apic_enable(int irq);
void io_apic_disable(int irq);

extern interrupt_handler_t isr_io_apic[24];

#endif

