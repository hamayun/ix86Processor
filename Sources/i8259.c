/* Intel 8259 - PIC (Programmable Interrupt Controller) */

#include <Processor/Processor.h>

#include "i8259.h"

interrupt_handler_t isr_i8259[16];

void i8259_init(void) {
    int i;

    for(i=0; i<16; i++) {
        isr_i8259[i] = NULL;
    }

    /* Initialize the master. */
    outb(0x11, 0x20); // ICW1: edge-triggered.
    outb(i8259_VECTOR_OFFSET, 0x21); // ICW2: vector offset
    outb(0x4, 0x21); // ICW3: IRQ slave to master
    outb(0x5, 0x21); // ICW4: mode (master, no automatic ack)

    /* Initialize the slave. */
    outb(0x11, 0xa0); // ICW1: edge-triggered.
    outb(i8259_VECTOR_OFFSET + 8, 0xa1); // ICW2: vector offset
    outb(0x2, 0xa1); // ICW3: IRQ slave to master
    outb(0x1, 0xa1); // ICW4: mode (slave, no automatic ack)

    /* Ack any bogus interrupts. */
    outb(0x20, 0x20);
    outb(0x20, 0xa0);

    /* Disable all IRQs (except slave to master). */
    outb(0xfb, 0x21);
    outb(0xff, 0xa1);
}

void i8259_master_handler(int irq) {
    interrupt_handler_t handler;

    handler = isr_i8259[irq];
    if(handler) {
        handler((void *)i8259_VECTOR_OFFSET + irq);
    }

    outb(0x20, 0x20); // send an ack to the master
}

void i8259_slave_handler(int irq) {
    interrupt_handler_t handler;

    handler = isr_i8259[irq];
    if(handler) {
        handler((void *)i8259_VECTOR_OFFSET + irq);
    }

    outb(0x20, 0xa0); // send an ack to the slave
    outb(0x20, 0x20); // send an ack to the master
}

void i8259_enable(int irq) {
    if ((irq >= 0) && (irq <= 7)) {
        outb(inb(0x21) & (0xff - (1 << irq)), 0x21);
    } else if (irq <= 15) {
        outb(inb(0xa1) & (0xff - (1 << (irq - 8))), 0xa1);
    }
}

void i8259_disable(int irq) {
    if ((irq >= 0) && (irq <= 7)) {
        outb(inb(0x21) | (1 << irq), 0x21);
    } else if (irq <= 15) {
        outb(inb(0xa1) | (1 << (irq - 8)), 0xa1);
    }
}

void i8259_shutdown(void) {
    outb(0xff, 0x21);
    outb(0xff, 0xa1);
}

