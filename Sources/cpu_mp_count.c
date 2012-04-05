#include <stdint.h>
#include <Processor/Processor.h>

extern int32_t CPU_COUNT;

int32_t cpu_mp_count (void)
{
    return CPU_COUNT;
}

