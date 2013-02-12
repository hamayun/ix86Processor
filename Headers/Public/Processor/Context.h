#ifndef PROCESSOR_CONTEXT_H
#define PROCESSOR_CONTEXT_H

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdlib.h>

#endif /* __ASSEMBLER__ */

//#define DEBUG_PROCESSOR_CONTEXT

#define CPU_CTXT_CANARY0_VAL  0xAAAA5555
#define CPU_CTXT_CANARY1_VAL  0xCCCC3333

#define CPU_CONTEXT_NBREGS 12
#define CPU_CONTEXT_SIZE 0x30

#define CPU_CTXT_CANARY0   0x00
#define CPU_CTXT_EAX       0x04
#define CPU_CTXT_ECX       0x08
#define CPU_CTXT_EDX       0x0C
#define CPU_CTXT_EBX       0x10
#define CPU_CTXT_ESP       0x14
#define CPU_CTXT_EBP       0x18
#define CPU_CTXT_ESI       0x1C
#define CPU_CTXT_EDI       0x20
#define CPU_CTXT_EFLAGS    0x24
#define CPU_CTXT_EIP       0x28
#define CPU_CTXT_CANARY1   0x2C

#ifndef __ASSEMBLER__

typedef volatile uintptr_t cpu_context_t [CPU_CONTEXT_NBREGS];

void cpu_context_init (cpu_context_t * ctx, void * sp, int32_t ssize, void * entry, void * arg);
void cpu_context_save (cpu_context_t * ctx, uint32_t *entry);
void cpu_context_load (cpu_context_t * ctx);

#ifdef DEBUG_PROCESSOR_CONTEXT
void print_context(cpu_context_t *ctx);
void print_context_mem(cpu_context_t *ctx);
void print_stack(void * stack_base, int32_t stack_size, int32_t n);
#endif

#if 0
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
#endif /* 0 */


#endif /* __ASSEMBLER__ */

#endif

