#ifndef _CPU_H_
#define _CPU_H_

struct pseudo_descriptor {
    unsigned short limit;
    unsigned long linear_base;
} __attribute__ ((aligned(2), packed));

// GDT Segments :
#define BASE_TSS      0x08
#define KERNEL_CS     0x10
#define KERNEL_DS     0x18
#define KERNEL_CS_16  0x20
#define KERNEL_DS_16  0x28

//==================================================================
#define SZ_32           0x4                     /* 32-bit segment */
#define SZ_16           0x0                     /* 16-bit segment */
#define SZ_G            0x8                     /* 4K limit field */

#define ACC_A           0x01                    /* accessed */
#define ACC_TYPE        0x1e                    /* type field: */

#define ACC_TYPE_SYSTEM 0x00                    /* system descriptors: */

#define ACC_LDT         0x02                        /* LDT */
#define ACC_CALL_GATE_16 0x04                       /* 16-bit call gate */
#define ACC_TASK_GATE   0x05                        /* task gate */
#define ACC_TSS         0x09                        /* task segment */
#define ACC_CALL_GATE   0x0c                        /* call gate */
#define ACC_INTR_GATE   0x0e                        /* interrupt gate */
#define ACC_TRAP_GATE   0x0f                        /* trap gate */

#define ACC_TSS_BUSY    0x02                        /* task busy */

#define ACC_TYPE_USER   0x10                    /* user descriptors */

#define ACC_DATA        0x10                        /* data */
#define ACC_DATA_W      0x12                        /* data, writable */
#define ACC_DATA_E      0x14                        /* data, expand-down */
#define ACC_DATA_EW     0x16                        /* data, expand-down,
                                                             writable */
#define ACC_CODE        0x18                        /* code */
#define ACC_CODE_R      0x1a                        /* code, readable */
#define ACC_CODE_C      0x1c                        /* code, conforming */
#define ACC_CODE_CR     0x1e                        /* code, conforming,
                                                       readable */
#define ACC_PL          0x60                    /* access rights: */
#define ACC_PL_K        0x00                    /* kernel access only */
#define ACC_PL_U        0x60                    /* user access */
#define ACC_P           0x80                    /* segment present */

/*
 * Components of a selector
 */
#define SEL_LDT         0x04                    /* local selector */
#define SEL_PL          0x03                    /* privilege level: */
#define SEL_PL_K        0x00                        /* kernel selector */
#define SEL_PL_U        0x03                        /* user selector */

#endif

