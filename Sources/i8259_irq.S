#define I8259_MASTER_IRQ(id) \
.text; \
i8259_irq_##id: \
    pusha; \
    push $(id); \
    call i8259_master_handler; \
    add $4, %esp; \
    popa; \
    iret; \
.data; \
    .long i8259_irq_##id

#define I8259_SLAVE_IRQ(id) \
.text; \
i8259_irq_##id: \
    pusha; \
    push $(id); \
    call i8259_slave_handler; \
    add $4, %esp; \
    popa; \
    iret; \
.data; \
    .long i8259_irq_##id

.data
.global i8259_irq
i8259_irq:
I8259_MASTER_IRQ(0)
I8259_MASTER_IRQ(1)
I8259_MASTER_IRQ(2)
I8259_MASTER_IRQ(3)
I8259_MASTER_IRQ(4)
I8259_MASTER_IRQ(5)
I8259_MASTER_IRQ(6)
I8259_MASTER_IRQ(7)
I8259_SLAVE_IRQ(8)
I8259_SLAVE_IRQ(9)
I8259_SLAVE_IRQ(10)
I8259_SLAVE_IRQ(11)
I8259_SLAVE_IRQ(12)
I8259_SLAVE_IRQ(13)
I8259_SLAVE_IRQ(14)
I8259_SLAVE_IRQ(15)

