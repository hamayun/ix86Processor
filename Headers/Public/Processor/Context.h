#ifndef PROCESSOR_CONTEXT_H
#define PROCESSOR_CONTEXT_H

#include <stdint.h>
#include <stdlib.h>

//#define DEBUG_PROCESSOR_CONTEXT

#define CPU_CONTEXT_NBREGS 32

typedef volatile uintptr_t cpu_context_t [CPU_CONTEXT_NBREGS];
#define CPU_CONTEXT_SIZE 128

void cpu_context_init (cpu_context_t * ctx, void * sp, int32_t ssize, void * entry, void * arg);
void cpu_context_save (cpu_context_t * ctx, uint32_t *entry);
void cpu_context_load (cpu_context_t * ctx);

#ifdef DEBUG_PROCESSOR_CONTEXT
void print_context(cpu_context_t *ctx);
void print_context_mem(cpu_context_t *ctx);
void print_stack(void * stack_base, int32_t stack_size, int32_t n);
#endif

/*
void cpu_context_save (cpu_context_t * ctx, uint32_t *entry);
*/
#define cpu_context_save(ctx,addr) \
    do{ \
		 __asm__ volatile (	                                      \
			"push %1 \n"									      \
           "push %0 \n"                                           \
           "push %%ebp \n"                                        \
           "mov %%esp, %%ebp \n"                                  \
           "\n"                                                   \
           "mov 0x4(%%ebp), %%esp \n"       /* switch to ctx  */  \
           "add $128, %%esp\n" /* move to the end */ \
           "\n"                             /* for push use    */ \
           "push (%%ebp) \n"          /* ebp            */        \
           "push 0x8(%%ebp) \n"       /* return address */        \
           "push %%ebp \n"            /* esp            */        \
           "pushf \n"                 /* flags (4B)     */        \
           "pusha \n"                 /* 8 * 4B         */        \
           "mov 0x4(%%ebp), %%eax \n" /* eax <- ctxt    */        \
           "mov %%esp, (%%eax) \n"    /* ctxt[0] <- start */      \
           "\n"                                                   \
		   "mov %%ebp, %%esp \n"      /* restore esp    */        \
		   "pop %%ebp \n"             /* restore ebp    */        \
           "add $0x8, %%esp"          /* pop useless    */	      \
           :: "a" ((unsigned long) (ctx)), "b" ((unsigned long) (addr)):);   \
    } while (0)

/*
void cpu_context_load (cpu_context_t * ctx);
*/
#define cpu_context_load(ctx) \
    do{ \
        __asm__ volatile (\
            "mov (%0), %%esp \n"        /* esp <- ctxt[0] (== start) */ \
            "popa \n"                   /* 8 * 4B                    */ \
            "popf \n"                   /* flags 4B                  */	\
            "pop %%ebp \n"              /* esp                       */	\
            "mov 0x4(%%esp), %%eax \n"  /* eax <- ebp                */ \
            "mov %%eax, (%%ebp) \n"     /* ebp in stack              */ \
            "mov (%%esp), %%eax \n"     /* eax <- return address     */ \
		    "mov %%eax, 0x04(%%ebp) \n" /* return address in stack   */ \
            "mov %%esp, %%eax\n"        /* restore eax               */ \
            "add $0x08, %%eax\n"                                        \
		    "sub $128, %%eax \n"                           \
            "\n"                                                        \
            "leave \n"                                                  \
            "ret $0x4 \n"                                               \
        ::"a" ((unsigned long) (ctx)):);\
    } while (0)

#endif

