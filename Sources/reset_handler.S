IDT_ENTRIES                     = 256

LOCAL_APIC_BASE                 = 0xFEE00000
LAPIC_CPU_ID                    = 0x020
LAPIC_EOI                       = 0x0B0
LAPIC_SPURIOUS                  = 0x0F0
LAPIC_ICR_LOW                   = 0x300
LAPIC_LVT_ERR                   = 0x370
LOCAL_TIMER_LVT                 = 0x320
LOCAL_TIMER_INITIAL_COUNTER     = 0x380
LOCAL_TIMER_CURRENT_COUNTER     = 0x390
LOCAL_COUNTER_DIVIDER           = 0x3E0
LOCAL_TIMER_VECTOR              = 0XEF
LAPIC_ERROR_VECTOR              = 0xEE
LAPIC_IPI_VECTOR                = 0xED

    .code32
    .section ".reset", "ax"

    .globl reset_handler;
reset_handler:
    cli
    call    gdt_init
    cmp     $1, first_cpu
    jne     4f
    movl    $0, first_cpu
    movl    CPU_SYS_STACK_ADDR, %esp
    call    cpu_idt_init
    call    local_apic_setup
    call    tsc_calibrate
    #sti
    jmp     5f

4: # cpu > 1
    lidt    idtdesc      # cpu != 0
    call    local_apic_setup
    
    # Compute the basic stack addresses
    call    cpu_mp_id
    mov     %eax, %ebp
    mov     $0x400, %ecx
    mul     %ecx
    movl    CPU_SYS_STACK_ADDR, %ecx
    subl    %eax, %ecx
    movl    %ecx, %esp
    
    #sti

5:
    # Call cpu_start
    movl    $cpu_start, %eax
    jmp     * %eax
    nop

    .global lapic_error_int
lapic_error_int:
    pusha
    call    cpu_mp_id
    push    %eax
    push    $msg_lapic_error
    call    tty_print_info
    add     $8, %esp
    popa

    iret /* lapic_error_int */

local_apic_setup:
	movl    $1, %eax            # see if this cpu has rdmsr/wrmsr instructions
	cpuid
	bt      $9, %edx            # see if this chip has an local apic
	jnc     no_lapic            # if not, can't use apic
	bt      $5, %edx            # check for rdmsr instruction capability
	jnc     apic_default
	movl    $0x1B, %ecx         # ok, read the apic physical base address
	rdmsr                       # ... into eax
	orl     $0x800, %eax		# set the global enable bit
	andl    $0xFFF, %eax
	orl     $LOCAL_APIC_BASE, %eax
	wrmsr
apic_default:
    # vector EF, one shot
    movl    $LOCAL_TIMER_VECTOR, (LOCAL_APIC_BASE + LOCAL_TIMER_LVT)
    # divide by 1
    movl    $0x0B,  (LOCAL_APIC_BASE + LOCAL_COUNTER_DIVIDER)
    movl    $0x00, (LOCAL_APIC_BASE + LOCAL_TIMER_INITIAL_COUNTER)
    movl    (LOCAL_APIC_BASE + LAPIC_SPURIOUS), %eax    #enable IPI
    orl     $0x100, %eax
    movl    %eax, (LOCAL_APIC_BASE + LAPIC_SPURIOUS)

    ret                         # local_apic_setup
no_lapic:
    pushl   $msg_no_lapic
    call    tty_print_info
    jmp     _hell_pit


    .global ignore_int
ignore_int:
    pusha
    
    #EOI
    movl    $0, (LAPIC_EOI + LOCAL_APIC_BASE)

    call    cpu_mp_id
    push    %eax
    push    $msg_ignored_int
    call    tty_print_info
    add     $8, %esp
    
    popa
    iret


    .global timer_int
timer_int:
    pusha

    #EOI
    movl    $0, (LAPIC_EOI + LOCAL_APIC_BASE)

    pushl   $1
    movl    isr_timer, %eax
    call    * %eax
    add     $4, %esp

    popa
    iret


    .global ipi_int
    .align 4
ipi_int:
    pusha

    #EOI
    movl    $0, (LAPIC_EOI + LOCAL_APIC_BASE)

    pushl   $1
    movl    isr_ipi, %eax
    call    * %eax
    add     $4, %esp

    popa
    iret


_hell_pit:
    call    cpu_mp_id
    push    %eax
    pushl   $msg_hell_pit
    call    tty_print_info
    add     $8, %esp
    hlt
    jmp     _hell_pit

.data
    .align  4
first_cpu: .long 1

# messages
msg_ignored_int: .string "An ignored interrupt occured on cpu %ld.\n"
msg_hell_pit: .string "Hell pit reached by cpu %ld.\n"
msg_no_lapic: .string "No local local APIC found on this computer.\n"
msg_timer_int: .string "Timer interrupt occured on cpu %ld.\n"
msg_lapic_error: .string "Local APIC error interrupt occured on cpu %ld.\n"

