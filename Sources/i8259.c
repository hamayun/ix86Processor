/* Intel 8259 - PIC (Programmable Interrupt Controller) */

#include <Processor/Processor.h>
#include <Core/Core.h>

#include "i8259.h"

interrupt_handler_t isr_i8259[16];

/* PIC Master Command Port = 0x20, Master Data Port = 0x21 */
#define PIC_MST_CMD 0x20
#define PIC_MST_DATA 0x21

/* PIC Slave Command Port = 0xa0, Slave Data Port = 0xa1 */
#define PIC_SLV_CMD 0xa0
#define PIC_SLV_DATA 0xa1

#define PIC_EOI 0x20

void i8259_init(void) {
    int i;

    for(i=0; i<16; i++) {
        isr_i8259[i] = NULL;
    }

    /* Initialize the master. */
    outb(0x11, PIC_MST_CMD); // ICW1: edge-triggered.
    outb(i8259_VECTOR_OFFSET, PIC_MST_DATA); // ICW2: vector offset
    outb(0x4, PIC_MST_DATA); // ICW3: IRQ slave to master
    outb(0x5, PIC_MST_DATA); // ICW4: mode (master, no automatic ack)

    /* Initialize the slave. */
    outb(0x11, PIC_SLV_CMD); // ICW1: edge-triggered.
    outb(i8259_VECTOR_OFFSET + 8, PIC_SLV_DATA); // ICW2: vector offset
    outb(0x2, PIC_SLV_DATA); // ICW3: IRQ slave to master
    outb(0x1, PIC_SLV_DATA); // ICW4: mode (slave, no automatic ack)

    /* Ack any bogus interrupts. */
    outb(PIC_EOI, PIC_MST_CMD);
    outb(PIC_EOI, PIC_SLV_CMD);

    /* Disable all IRQs (except slave to master). */
    outb(0xfb, PIC_MST_DATA);
    outb(0xff, PIC_SLV_DATA);

}

void i8259_master_handler(int irq) {
    interrupt_handler_t handler;

    // send an ack to the master
    // we do it before handling interrupt because of DNA architecture
    outb(PIC_EOI, PIC_MST_CMD);
    
    handler = isr_i8259[irq];
    if(handler) {
        handler((void *)i8259_VECTOR_OFFSET + irq);
    }
}

void i8259_slave_handler(int irq) {
    interrupt_handler_t handler;

    // send an ack to the master and the slave
    // we do it before handling interrupt because of DNA architecture
    outb(PIC_EOI, PIC_SLV_CMD); // send an ack to the slave
    outb(PIC_EOI, PIC_MST_CMD); // send an ack to the master

    handler = isr_i8259[irq];
    if(handler) {
        handler((void *)i8259_VECTOR_OFFSET + irq);
    }
}

void i8259_enable(int irq) {
    if ((irq >= 0) && (irq <= 7)) {
        outb(inb(PIC_MST_DATA) & (0xff - (1 << irq)), PIC_MST_DATA);
    } else if (irq <= 15) {
        outb(inb(PIC_SLV_DATA) & (0xff - (1 << (irq - 8))), PIC_SLV_DATA);
    }
}

void i8259_disable(int irq) {
    dna_printf("i8259: disabling irq %d\n", irq);
    if ((irq >= 0) && (irq <= 7)) {
        outb(inb(PIC_MST_DATA) | (1 << irq), PIC_MST_DATA);
    } else if (irq <= 15) {
        outb(inb(PIC_SLV_DATA) | (1 << (irq - 8)), PIC_SLV_DATA);
    }
}

void i8259_shutdown(void) {
    outb(0xff, PIC_MST_DATA);
    outb(0xff, PIC_SLV_DATA);
}

