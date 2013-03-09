#include <Processor/Processor.h>

void cpu_power_wake_on_interrupt (void)
{
	dna_printf("<%d> Inside Idle Thread\n", cpu_mp_id());
    __asm__ volatile ("sti");
    __asm__ volatile ("hlt");
}

