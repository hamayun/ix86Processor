#include <Processor/Context.h>

.section .text


	.global cpu_context_load

  /*
   * Arguments:
   *        0x4(esp): ctx_addr
   */

cpu_context_load:
	mov  0x8(%esp), %ebx  /* ebx <- ctx_addr */

	mov  $CPU_CTXT_EIP(%ebx), %eax
	push %eax                        /* EIP -> stack    */
			  
	mov  $CPU_CTXT_EBP(%ebx), %eax
	push %eax                        /* EBP -> stack    */

    mov  $CPU_CTXT_EFLAGS(%ebx), %eax
	push %eax                        /* EFLAGS -> stack */
			  
	mov	 $CPU_CTXT_ESP(%ebx), %ebp   /* ESP -> EBP      */

	mov  $CPU_CTXT_EAX(%ebx), %eax   /* EAX             */
	mov  $CPU_CTXT_ECX(%ebx), %ecx   /* ECX             */
    mov  $CPU_CTXT_EDX(%ebx), %edx   /* EDX             */

	mov  $CPU_CTXT_ESI(%ebx), %esi   /* ESI             */
	mov  $CPU_CTXT_EDI(%ebx), %edi   /* EDI             */
	mov  $CPU_CTXT_EBX(%ebx), %ebx   /* EBX             */

	popf                             /* stack -> EFLAGS */
	leave /* == mov %ebp, %esp; pop %ebp */
		                             /* EBP -> ESP      */
									 /* stack -> EBP    */
	ret                              /* stack -> EIP    */