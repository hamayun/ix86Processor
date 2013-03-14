    .globl cpu_mp_id
cpu_mp_id:
    movl    0xFEE00020, %eax       /* Read the Local APIC ID Register */
    shrl    $0x18, %eax            /* Drop the 24 LSBs by Shift Right */
    ret                            /* Return EAX with the 8 MSBs i.e. VCPU-ID */
