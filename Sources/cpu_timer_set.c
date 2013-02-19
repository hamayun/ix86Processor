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
#include <Platform/Platform.h>
#include <Processor/apic_regs.h>

#include "apic.h"

void cpu_timer_set (int32_t id, bigtime_t ns)
{
    uint32_t cycles;

    // reset the counter
    local_apic_mem[LAPIC_INITIAL_COUNTER >> 2] = 0;
    
    cycles = (uint32_t) ((cpu_bus_cycles_per_ms * (ns / 1000)) / (LOCAL_TIMER_DIVISOR * 1000));
    if (cycles < 100)
        cycles = 100;

    // set the counter (trigger it)
    local_apic_mem[LAPIC_INITIAL_COUNTER >> 2] = cycles;
}

