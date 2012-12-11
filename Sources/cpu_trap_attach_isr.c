#include <stdint.h>
#include <Processor/Processor.h>

#include "i8259.h"

interrupt_handler_t isr_timer, isr_ipi;

extern interrupt_handler_t isr_i8259[16];

void cpu_trap_attach_isr (int32_t cpuid, interrupt_id_t id,
    uint32_t mode, interrupt_handler_t isr)
{
    switch (id)
    {
    case 0: //IPI
        isr_ipi = isr;
        break;
    case 1: //TIMER
        isr_timer = isr;
        break;
    default:
        if(id >= i8259_VECTOR_OFFSET && id < (i8259_VECTOR_OFFSET + 16)) {
            isr_i8259[id - i8259_VECTOR_OFFSET] = isr;
        }
        break;
    }
}

