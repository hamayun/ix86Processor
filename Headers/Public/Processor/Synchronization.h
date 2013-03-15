#ifndef PROCESSOR_SYNCHRONIZATION_H
#define PROCESSOR_SYNCHRONIZATION_H

#define SYSTEMC_TEST_N_SET_PORT 0x3000

/* static inline long int cpu_compare_and_swap (volatile long int * p_val, long int oldval, long int newval) { */
static inline int32_t cpu_compare_and_swap (volatile int32_t * p_val, int32_t oldval, int32_t newval)
{
    unsigned char prev;
    
	__asm__ volatile (
        "lock cmpxchgl %1, %2\n"
    	"setne %%al"
		     : "=a" (prev)
		     : "r" (newval), "m" (* p_val), "0" (oldval)
		     : "memory");

    // if return 0 => OK; Lock Successfully Taken
	return prev;
}

/* static inline long int cpu_test_and_set (volatile long int * spinlock) { */
static inline int cpu_test_and_set (volatile long int * spinlock)
{
    int my_cpu_id = cpu_mp_id() + 1;
	int locker_cpu_id = 0;	// Zero means unlocked

    locker_cpu_id = cpu_compare_and_swap (spinlock, 0, my_cpu_id);
	if(locker_cpu_id != 0)
	{
		if(locker_cpu_id != my_cpu_id)
		{
			cpu_io_write(UINT32, SYSTEMC_TEST_N_SET_PORT, locker_cpu_id);
			return 1;		// Try Again after the other CPU has run for some tim 
		}
		else
		{
			dna_printf("-");
			return 1;		// Try Again Immediatly 
		}
	}
	else
		return 0;		// OK, now the current CPU has this lock
}
     
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

