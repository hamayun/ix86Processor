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

extern uint64_t     cpu_cycles_per_ms;
extern uint64_t     cpu_bus_cycles_per_ms;

#endif

