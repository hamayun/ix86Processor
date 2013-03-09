#include <Processor/Processor.h>

void cpu_mp_wait (void)
{
  dna_printf("CPU-%d in cpu_mp_wait\n", cpu_mp_id());

  cpu_mp_synchro = 1;

  /*
   * TODO: add WBFLUSH
   */

//  while ((volatile int32_t)cpu_mp_synchro);

  while ((volatile int32_t)cpu_mp_synchro)
  {
	dna_printf(".");
  }
}

