#include <Processor/Processor.h>
#include <Processor/apic_regs.h>

/*
 * TODO: Remove this platform specific Header
 *       Move to Platform/Platform.h
 */
#include <PCPlatformDriver/Driver.h>

void cpu_trap_disable (interrupt_id_t id)
{
   uint32_t    val;

    switch (id)
    {
    case 0: //IPI
        //§§IPI
        break;
    case 1: //TIMER
            val = local_timer_mem[LAPIC_TIMER_LVT >> 2];
            val &= ~0x10000;
            local_timer_mem[LAPIC_TIMER_LVT >> 2] = val;
        break;
    
    }
}

