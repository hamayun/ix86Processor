#ifndef PROCESSOR_SYNCHRONIZATION_H
#define PROCESSOR_SYNCHRONIZATION_H

#define SYSTEMC_TEST_N_SET_PORT 0x3000

/* static inline long int cpu_compare_and_swap (volatile long int * spinlock, long int oldval, long int newval) { */
static inline int32_t cpu_compare_and_swap (volatile int32_t * spinlock, int32_t oldval, int32_t newval)
{
    unsigned char prev;
    
	__asm__ volatile (
        "lock cmpxchgl %1, %2\n"
		     : "=a" (prev)  /* prev will have a non-zero value if cmpxchgl didn't update the lock */
		     : "r" (newval), "m" (* spinlock), "0" (oldval)
		     : "memory");

	return prev;	// Return the previous value of lock; Zero means lock was taken successfully.
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
			// if the lock is taken by some one else try to tell him
			cpu_io_write(UINT32, SYSTEMC_TEST_N_SET_PORT, locker_cpu_id);
			return 1;		// Try Again after the other CPU has run for some time
		}
		else
		{
			return 1;		// This should never happen; otherwise we have a deadlock
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

