#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <Processor/Cache.h>
#include <Processor/Context.h>
#include <Processor/Endian.h>
#include <Processor/IO.h>
#include <Processor/Mp.h>
#include <Processor/Power.h>
#include <Processor/Synchronization.h>
#include <Processor/Timer.h>
#include <Processor/Trap.h>

void blocking_usleep (int us);
void blocking_nsleep (int ns);

extern uint64_t             cpu_cycles_per_ms;
extern uint64_t             cpu_bus_cycles_per_ms;
extern volatile int32_t     no_cpus_up;
extern volatile uint32_t    cpus_up_mask;

#define MAX_CPU_COUNT 32

typedef struct
{
    int32_t     command;
    void        *data;
    int32_t     status;
} cpu_ipi_par;

extern volatile cpu_ipi_par cpu_ipi_pars[MAX_CPU_COUNT];

#endif

