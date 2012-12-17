#include <Processor/Processor.h>
#include <Processor/apic_regs.h>

/*
 * TODO: Remove this platform specific Header
 *       Move to Platform/Platform.h
 */
#include <PCPlatformDriver/Driver.h>

#include "i8259.h"
#include "io_apic.h"

void cpu_trap_disable (interrupt_id_t id)
{
    uint32_t    val;

    switch (id)
    {
    case 0: //IPI
            val = local_apic_mem[LAPIC_SPURIOUS >> 2];
            val &= ~0x100;
            local_apic_mem[LAPIC_SPURIOUS >> 2] = val;
        break;
    case 1: //TIMER
            val = local_apic_mem[LAPIC_TIMER_LVT >> 2];
            val |= 0x10000;
            local_apic_mem[LAPIC_TIMER_LVT >> 2] = val;
        break;
    default:
        if(id >= i8259_VECTOR_OFFSET && id < (i8259_VECTOR_OFFSET + 16)) {
            i8259_disable(id - i8259_VECTOR_OFFSET);
        }
        else if(id >= IO_APIC_VECTOR_OFFSET && id < (IO_APIC_VECTOR_OFFSET + 24)) {
            io_apic_disable(id - IO_APIC_VECTOR_OFFSET);
        } 
        break;    
    }
}

