#ifndef PROCESSOR_SYNCHRONIZATION_H
#define PROCESSOR_SYNCHRONIZATION_H

/* static inline long int cpu_compare_and_swap (volatile long int * p_val, long int oldval, long int newval) { */
static inline int32_t cpu_compare_and_swap (volatile int32_t * p_val, int32_t oldval, int32_t newval)
{
    //return 0 => ok
    unsigned char prev;
    
	__asm__ volatile (
        "lock cmpxchgl %1, %2\n"
    	"setne %%al"
		     : "=a" (prev)
		     : "r" (newval), "m" (* p_val), "0" (oldval)
		     : "memory");

	// MMH: Activating this I/O generates a lot of random MMIOs in KVM
	if(prev){
        cpu_io_write(UINT32,0x1000,1);
	}

	return prev;
}

/* static inline long int cpu_test_and_set (volatile long int * spinlock) { */
static inline int cpu_test_and_set (volatile long int * spinlock)
{
    return cpu_compare_and_swap (spinlock, 0, 1);
}

/*
 * MMH: Some examples previous test and set implementations in Native Simulation.
 * May give some ideas for a different implementation of the KVM based Native simulation using SC_THREADs only.
 */
     
/*
static inline int cpu_test_and_set (volatile long int * spinlock)
{
    // It's not ok with smp. Use cmpxchg instead.
    int oldValue;
    oldValue = *spinlock;
    *spinlock = 1;
    return oldValue;
}
*/

/*
static inline int cpu_test_and_set (volatile long int * spinlock)
{
    long int ret;

    do {
        if((ret = p_io->load_linked((uint32_t*)spinlock, m_cpu_id)) != 0)
            break;
    } while((ret = p_io->store_cond((uint32_t*)spinlock, 1, m_cpu_id)) != 0);
    
    return ret;
}
*/

#endif

