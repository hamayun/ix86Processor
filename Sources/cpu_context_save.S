#include <Processor/Context.h>

.section .text

   .global cpu_context_save

  
  /*
   * Arguments:
   *        0x4(esp): ctx_addr
   *        0x8(esp): entry
   */

 /*
  * load will need three words on saved stack (EBP, EFLAGS and EIP)
  *    but EIP is the return address push by caller.
  */
cpu_context_save:
              /* pushed ret will receive EIP at load */
   push %eax  /* will be replaced by EFLAGS at load  */
   push %ebx  /* will be replaced by EBP at load     */

   mov 0xC(%esp), %ebx /* ebx <- ctx_addr   */

   mov  %eax, CPU_CTXT_EAX(%ebx)  /* EAX      */
   mov  %ecx, CPU_CTXT_ECX(%ebx)  /* ECX      */
   mov  %edx, CPU_CTXT_EDX(%ebx)  /* EDX      */
   mov  %ebx, %eax        /* eax <- ctx_addr   */

   mov  0x0(%esp), %ebx
   mov  %ebx, CPU_CTXT_EBX(%eax)  /* EBX      */

   /* lea  8(%esp), %ebx */
             /* no need to remove the pushes  */ 
   mov  %esp, CPU_CTXT_ESP(%eax)  /* ESP      */
   mov  %ebp, CPU_CTXT_EBP(%eax)  /* EBP      */
    
   mov  %esi, CPU_CTXT_ESI(%eax)  /* ESI      */
   mov  %edi, CPU_CTXT_EDI(%eax)  /* EDI      */

   pushf
   pop %ebx
   mov %ebx, CPU_CTXT_EFLAGS(%eax) /* EFLAGS  */

   mov 0x10(%esp), %ebx   /* ebx <- entry      */ 
   mov %ebx, CPU_CTXT_EIP(%eax)    /* entry   */

   pop %ebx
   pop %eax
   ret
