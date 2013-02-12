#include <Processor/Context.h>
#include <DnaTools/C.h>
#include <string.h>

void cpu_context_init(cpu_context_t * ctx, void * sp, int32_t ssize, void * entry, void * arg)
{
    uintptr_t *pnewstack, *pctx, *ebp_orig;
    uintptr_t ctx_start = 0;

    memset(ctx, 0, sizeof (cpu_context_t));

	if(ctx & 0x3){
		 dna_printf("Bad context alignment !!!");
		 while(1);
	}

    ctx_start = (((uint8_t *)ctx) + CPU_CONTEXT_SIZE);

#if 0
	/* Do it at the end */
    // The first entry gives us the current location of push/pop pointer on context.
    *((uintptr_t *) ctx) = ctx_save_start;
#endif

    // Get the 16-bit Aligned Address for our Stack Top.
    pnewstack = (uintptr_t *) (((uintptr_t) sp + ssize - sizeof(uintptr_t)) & ~0xF);
    ebp_orig = pnewstack;

    /* Get the context push/pop location; the first entry of context has it */
    pctx = (uintptr_t *)ctx_start;

    // "push (%%ebp) \n"           /* ebp                           */
    *--pctx = (uintptr_t)ebp_orig; /* start ebp is our original esp */

    // "push 0x8(%%ebp) \n"        /* return address                */
    *--pctx = (uintptr_t) entry;   /*  is (entry) address           */

    // "push %%ebp \n"             /* esp                           */
    *--pctx = (uintptr_t) 0;       /* will be patched later with args */

    /* "pushf \n" */
    *--pctx = (uintptr_t) 0x00000246; /* EFLAGS: IF ZF PF reserved  */

    /* "pusha \n"            GP registers default values */
    *--pctx = (uintptr_t) 0;      /* EDI */
    *--pctx = (uintptr_t) 0;      /* ESI */
    *--pctx = (uintptr_t) 0;      /* EBP */
    *--pctx = (uintptr_t) 0;      /* ESP; Ignored here */
    *--pctx = (uintptr_t) 0;      /* EBX */
    *--pctx = (uintptr_t) 0;      /* EDX */
    *--pctx = (uintptr_t) 0;      /* ECX */
    *--pctx = (uintptr_t) 0;      /* EAX */

    // Write (not push) the Argument on the stack's ebp_orig
    *pnewstack = (uintptr_t) arg;

    // The Return Address in the New Stack; Obviousely it should be invalid.
    *--pnewstack = 0xDEADBEAF;

    // Now move the stack pointer down to make space for @entry, @ctx & ebp_orig;
    // These will be restored by cpu_context_load from the context.
    pnewstack -= 3;

    // Patch the New Stack's address in the context as the context needs to know it for our ebp_temp (!ebp_orig)
    *((uintptr_t *) ctx_start - 3) = (uintptr_t) pnewstack;

    // Update our push/pop pointer in the first entry of our context
    *((uintptr_t *) ctx_start) = (uintptr_t) pctx;

    #ifdef DEBUG_PROCESSOR_CONTEXT
    print_context(ctx);
    print_context_mem(ctx);
    print_stack(sp, ssize, 10);
    #endif

    return;
}

#ifdef DEBUG_PROCESSOR_CONTEXT
void print_context(cpu_context_t *ctx)
{
    uintptr_t ctx_save_start = ((uintptr_t) ctx + CPU_CONTEXT_SIZE - sizeof(uintptr_t)) & ~0xF;     // Same as in context_init
    uintptr_t *pctx = (uintptr_t *) ctx_save_start;
    int i;

    dna_printf("--------------------------------------------------------------\n");
    dna_printf("Context Address (ctx) = 0x%x, ctx_save_start = 0x%x\n", ctx, ctx_save_start);
    dna_printf("--------------------------------------------------------------\n");
    pctx--;
    dna_printf("[0x%x (ebp_orig)]: 0x%x\n", (uintptr_t) pctx, (uintptr_t) * pctx); pctx--;
    dna_printf("[0x%x (entry   )]: 0x%x\n", (uintptr_t) pctx, (uintptr_t) * pctx); pctx--;
    dna_printf("[0x%x (ebp_temp)]: 0x%x\n", (uintptr_t) pctx, (uintptr_t) * pctx); pctx--;
    dna_printf("[0x%x (flags   )]: 0x%x\n", (uintptr_t) pctx, (uintptr_t) * pctx); pctx--;
    for (i = 0; i < 8; i++) {
        dna_printf("[0x%x (reg_%d   )]: 0x%x\n", (uintptr_t) pctx, i, (uintptr_t) * pctx); pctx--;
    }
    dna_printf("[0x%x (esp_pop )]: 0x%x\n", (uintptr_t) ((uintptr_t *) ctx), *((uintptr_t *) ctx));   // Its the first entry of context
    dna_printf("--------------------------------------------------------------\n");
}

void print_stack(void * stack_base, int32_t stack_size, int32_t n)
{
    uintptr_t *pstack = (uintptr_t *) (stack_base + stack_size);
    int i;
    dna_printf("--------------------------------------------------------------\n");
    dna_printf("Thread Stack (Base = 0x%x, Size = 0x%x, N = %d)\n", stack_base, stack_size, n);
    dna_printf("--------------------------------------------------------------\n");

    for (i = 0; i < n; i++) {
        pstack--;
        dna_printf("[0x%x]: 0x%x\n", (uintptr_t) pstack, (uintptr_t) *pstack);
    }
    dna_printf("--------------------------------------------------------------\n");
}

void print_context_mem(cpu_context_t *ctx)
{
    uintptr_t *pctx = (uintptr_t *) ctx;
    uintptr_t *pctx_end = (uintptr_t *)((uint8_t *) ctx + CPU_CONTEXT_SIZE );

    dna_printf("--------------------------------------------------------------\n");
    dna_printf("CONTEXT MEMORY CONTENTS\n");
    dna_printf("--------------------------------------------------------------\n");

    while (pctx < pctx_end) {
        dna_printf("[0x%x]: 0x%x\n", (uintptr_t) pctx, (uintptr_t) * pctx);
        pctx++;
    }
    dna_printf("--------------------------------------------------------------\n");
}
#endif

