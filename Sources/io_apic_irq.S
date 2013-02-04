#define IO_APIC_IRQ(id) \
.text; \
.globl io_apic_irq_##id; \
io_apic_irq_##id: \
    pusha; \
    push $(id); \
    call io_apic_handler; \
    add $4, %esp; \
    popa; \
    iret; \
.data; \
    .long io_apic_irq_##id

.data
.global io_apic_irq
io_apic_irq:
IO_APIC_IRQ(0)
IO_APIC_IRQ(1)
IO_APIC_IRQ(2)
IO_APIC_IRQ(3)
IO_APIC_IRQ(4)
IO_APIC_IRQ(5)
IO_APIC_IRQ(6)
IO_APIC_IRQ(7)
IO_APIC_IRQ(8)
IO_APIC_IRQ(9)
IO_APIC_IRQ(10)
IO_APIC_IRQ(11)
IO_APIC_IRQ(12)
IO_APIC_IRQ(13)
IO_APIC_IRQ(14)
IO_APIC_IRQ(15)
IO_APIC_IRQ(16)
IO_APIC_IRQ(17)
IO_APIC_IRQ(18)
IO_APIC_IRQ(19)
IO_APIC_IRQ(20)
IO_APIC_IRQ(21)
IO_APIC_IRQ(22)
IO_APIC_IRQ(23)
