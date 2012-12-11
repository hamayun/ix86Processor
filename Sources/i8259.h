#ifndef _I8259_H_
#define _I8259_H_

extern long i8259_VECTOR_OFFSET; // must be defined in linker script

void i8259_init(void);
void i8259_enable(int irq);
void i8259_disable(int irq);

#endif

