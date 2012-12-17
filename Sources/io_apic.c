/* Intel 82093 - I/O APIC (Advanced Programmable Interrupt Controller) */

#include <Processor/Processor.h>
#include <Core/Core.h>

#include <string.h>

#include "io_apic.h"
#include "i8259.h"

interrupt_handler_t isr_io_apic[24];

#define IOAPICID_OFFSET 0x00
#define IOAPICVER_OFFSET 0x01
#define IOAPICARB_OFFSET 0x02
#define IOREDTBL_OFFSET 0x10

static inline void io_apic_write(unsigned int apic, unsigned int reg, unsigned int value) {
    *((unsigned int *) apic) = reg;
    *((unsigned int *) (apic + 0x10)) = value;
}

static inline unsigned int io_apic_read(unsigned int apic, unsigned int reg) {
    *((unsigned int *) apic) = reg;
    return *((unsigned int *) (apic + 0x10));
}

union entry_union {
    struct { uint32_t w1; uint32_t w2; };
    struct IO_APIC_route_entry entry;
};

static struct IO_APIC_route_entry io_apic_read_entry(int apic, int pin) {
    union entry_union eu;
    //unsigned long flags;
    //raw_spin_lock_irqsave(&ioapic_lock, flags);
    eu.w1 = io_apic_read(apic, 0x10 + 2 * pin);
    eu.w2 = io_apic_read(apic, 0x11 + 2 * pin);
    //raw_spin_unlock_irqrestore(&ioapic_lock, flags);
    return eu.entry;
}

static void io_apic_write_entry(int apic, int pin, struct IO_APIC_route_entry e) {
    union entry_union eu = {{0, 0}};

    eu.entry = e;
    io_apic_write(apic, 0x11 + 2*pin, eu.w2);
    io_apic_write(apic, 0x10 + 2*pin, eu.w1);
}

static inline void imcr_pic_to_apic(void) {
    /* select IMCR register */
    outb(0x70, 0x22);
    /* NMI and 8259 INTR go through APIC */
    outb(0x01, 0x23);
}

static inline void imcr_apic_to_pic(void) {
    /* select IMCR register */
    outb(0x70, 0x22);
    /* NMI and 8259 INTR go directly to BSP */
    outb(0x00, 0x23);
}

static inline void apic_eoi(void) {
    *((unsigned int *)(0xFEE00000 + 0xB0)) = 0;
}

void io_apic_init(void) {
    int i;

    /* disable PIC */
    i8259_shutdown();

    imcr_pic_to_apic();

    for(i=0; i<24; i++) {
        isr_io_apic[i] = NULL;
    }

    unsigned int id = io_apic_read(0xFEC00000, IOAPICID_OFFSET);
    dna_printf("IOAPICID: %x\n", id);

    unsigned int ver = io_apic_read(0xFEC00000, IOAPICVER_OFFSET);
    dna_printf("IOAPICVER: %x\n", ver); // must be 0x--17----

    if((ver & 0x00170000) != 0x00170000) {
        dna_printf("I/O APIC error: bad version.\n");
        while(1);
    }

    unsigned int arb = io_apic_read(0xFEC00000, IOAPICARB_OFFSET);
    dna_printf("IOAPICARB: %x\n", arb);

    for(i=0; i<24; i++) {
        struct IO_APIC_route_entry entry;
        entry = io_apic_read_entry(0xFEC00000, i);
        dna_printf("apic[%d]: %d.%d.%d, ", i, entry.vector, entry.mask, entry.delivery_mode);

        memset(&entry, 0, sizeof(struct IO_APIC_route_entry));

        entry.mask = 1;
        entry.vector = IO_APIC_VECTOR_OFFSET + i;
        io_apic_write_entry(0xFEC00000, i, entry);
    }

    apic_eoi();
}

void io_apic_handler(int irq) {
    interrupt_handler_t handler;

    handler = isr_io_apic[irq];
    if(handler) {
        handler((void *)IO_APIC_VECTOR_OFFSET + irq);
    }

    apic_eoi();
}

void io_apic_enable(int irq) {
    struct IO_APIC_route_entry entry;
    entry = io_apic_read_entry(0xFEC00000, irq);
    entry.mask = 0;
    io_apic_write_entry(0xFEC00000, irq, entry);
}

void io_apic_disable(int irq) {
    struct IO_APIC_route_entry entry;
    entry = io_apic_read_entry(0xFEC00000, irq);
    entry.mask = 1;
    io_apic_write_entry(0xFEC00000, irq, entry);
}

