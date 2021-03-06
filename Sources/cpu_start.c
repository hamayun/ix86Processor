/*
 * Copyright (C) 2007 TIMA Laboratory
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Processor/Processor.h>
#include <Processor/apic_regs.h>
#include <string.h>

#include "i8259.h"
#include "apic.h"
#include "io_apic.h"

extern void system_kickstart (void);

void smp_init (void) {
    extern char inc_secondary_start, inc_secondary_end;

    // set error vector to lapic
    uint32_t lvt_err = local_apic_mem[LAPIC_LVT_ERR >> 2];
    lvt_err &= 0xFFFFFF00;
    lvt_err |= LAPIC_ERROR_VECTOR;
    local_apic_mem[LAPIC_LVT_ERR >> 2] = lvt_err;

	// copy the secondary cpu startup code
    memcpy ((void *) 0x30000, &inc_secondary_start, &inc_secondary_end - &inc_secondary_start);

    // broadcast INIT IPI
    local_apic_mem[LAPIC_ICR_LOW >> 2] = 0x000C4500;

    blocking_usleep_systemc (10000);

    // send SIPI
    int32_t   cpu, i;
    for (cpu = 1; cpu <= MAX_CPU_COUNT; cpu++)
    {
        local_apic_mem[LAPIC_ICR_HIGH >> 2] = cpu << 24;
        local_apic_mem[LAPIC_ICR_LOW >> 2] = 0x00004630;

        for (i = 0; i < 1000; i++)
        {
    		blocking_usleep_systemc (100);

            if (no_cpus_up == cpu + 1)
                break;
        }
        if (no_cpus_up < cpu + 1)
            break;
    }

    tty_print_info ("%d processors booted.\n", no_cpus_up);

    i8259_init();
#if 0
    io_apic_init();
#endif
}

void setup_pgtab(void) {

}

void cpu_start (void)
{
    int32_t cpu = cpu_mp_id ();

    if (!cpu) {
        smp_init ();
    }
    else {
        // CD: infinite loop for secondary processors, because it is not working yet
        /*while(1) {
            __asm__ __volatile__ ("hlt");
        }*/

        no_cpus_up++;
        cpus_up_mask |= 1 << cpu;

        tty_print_info ("Secondary CPU %d booted.\n", cpu);
    }

    __asm__ __volatile__ ("sti");

    system_kickstart ();
}

