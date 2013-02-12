#include <Processor/Context.h>

.section .text

	.global cpu_context_load

  /*
   * Arguments:
   *        0x4(esp): ctx_addr
   */

   /*
    * load need three words one saved stack (EBP, EFLAGS and EIP).
    */
cpu_context_load:
   mov  0x4(%esp), %ebx  /* ebx <- ctx_addr */

   mov  CPU_CTXT_ESP(%ebx), %ecx    /* get newstack       */
           
   mov  CPU_CTXT_EIP(%ebx), %eax
   mov  %eax, 0x8(%ecx)             /* EIP -> newstack    */
         
   mov  CPU_CTXT_EFLAGS(%ebx), %eax
   mov  %eax, 0x4(%ecx)             /* EFLAGS -> newstack */

   mov  CPU_CTXT_EBP(%ebx), %eax
   mov  %eax, 0x0(%ecx)             /* EBP -> oldstack    */

   mov CPU_CTXT_ESP(%ebx), %ebp     /* ESP -> EBP         */

   mov  CPU_CTXT_EAX(%ebx), %eax    /* EAX                */
   mov  CPU_CTXT_ECX(%ebx), %ecx    /* ECX                */
   mov  CPU_CTXT_EDX(%ebx), %edx    /* EDX                */
   mov  CPU_CTXT_ESI(%ebx), %esi    /* ESI                */
   mov  CPU_CTXT_EDI(%ebx), %edi    /* EDI                */
   mov  CPU_CTXT_EBX(%ebx), %ebx    /* EBX                */

   /* switch to newstack using oldstack */
   leave /* == mov %ebp, %esp; pop %ebp */
                                    /* EBP -> ESP         */
                                    /* stack -> EBP       */
   popf                             /* stack -> EFLAGS    */
   ret                              /* stack -> EIP       */
