#include <Processor/Context.h>
#include <DnaTools/C.h>
#include <string.h>

#define SET_REGISTER(base, depl, val) ((*(uint32_t *)(((uint8_t *)base) + depl)) = val)

void cpu_context_init(cpu_context_t * ctx, void * sp, int32_t ssize, void * entry, void * arg)
{
    uintptr_t *pnewstack, *ebp_orig;

    memset(ctx, 0, sizeof (cpu_context_t));

    /*
     * Setup stack
     */
    /* Get the 16-bit Aligned Address for our Stack Top. */
    /* BKS: 16-bit ????? */
    pnewstack = (uintptr_t *) (((uintptr_t) sp + ssize - sizeof(uintptr_t)) & ~0xF);
    ebp_orig = pnewstack;

    /* Write (not push) the Argument on the stack's ebp_orig */
    *pnewstack = (uintptr_t) arg;

    /* The Return Address in the New Stack; Obviousely it should be invalid. */
    *--pnewstack = 0xDEADBEEF;

    /*
     * Now move the stack pointer down to make space for
     * EBP, EFLAGS et EIP in cpu_context_load.
    */
    pnewstack -= 3;

    /*
     * Set registers
     */
    SET_REGISTER(ctx, CPU_CTXT_CANARY0, CPU_CTXT_CANARY0_VAL);
    SET_REGISTER(ctx, CPU_CTXT_EAX    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_ECX    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_EDX    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_EBX    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_ESP    , pnewstack);
    SET_REGISTER(ctx, CPU_CTXT_EBP    , ebp_orig);
    SET_REGISTER(ctx, CPU_CTXT_ESI    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_EDI    , 0x00000000);
    SET_REGISTER(ctx, CPU_CTXT_EFLAGS , 0x00000246); /* EFLAGS: IF ZF PF reserved  */
    SET_REGISTER(ctx, CPU_CTXT_EIP    , entry);
    SET_REGISTER(ctx, CPU_CTXT_CANARY1, CPU_CTXT_CANARY1_VAL);



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

