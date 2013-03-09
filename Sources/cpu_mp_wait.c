#include <Processor/Processor.h>

// Tell SystemC CPU to go on Wait
#define MP_SYNCHRO_WAIT 0		// 0 means put myself on wait 

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
	cpu_io_write(UINT32,0x1000,MP_SYNCHRO_WAIT);
//	dna_printf(".");
  }
  
  dna_printf("CPU-%d in cpu_mp_wait Finished!\n", cpu_mp_id());
}

