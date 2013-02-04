#include <Processor/Processor.h>

#include <string.h>

#include "cpu.h"

#define GDT_ENTRIES 6

extern unsigned long long gdt[GDT_ENTRIES];
extern struct pseudo_descriptor gdtdesc;

static void fill_descriptor(unsigned long long *entry, void *addr,
        unsigned limit, unsigned access, unsigned sizebits) {
    unsigned long p0, p1;
    unsigned long base = (unsigned long) addr;

    if (limit > 0xfffff) {
        limit >>= 12;
        sizebits |= SZ_G;
    }
    p0 = (limit & 0xffff) + ((base & 0xffff) << 16);
    p1 = (base >> 16) & 0xff;
    p1 |= ((access | ACC_P) & 0xff) << 8;
    p1 |= limit & 0xf0000;
    p1 |= (sizebits & 0xf) << 20;
    p1 |= base & 0xff000000;
    *entry = (((unsigned long long) p1) << 32) | p0;
}

void gdt_init(void) {
    memset(gdt, 0, sizeof(gdt));

    fill_descriptor(&gdt[KERNEL_CS / 8], 0, 0xffffffff, ACC_PL_K | ACC_CODE_R,
            SZ_32);
    fill_descriptor(&gdt[KERNEL_DS / 8], 0, 0xffffffff, ACC_PL_K | ACC_DATA_W,
            SZ_32);

    fill_descriptor(&gdt[KERNEL_CS_16 / 8], 0, 0xffff, ACC_PL_K | ACC_CODE_CR, SZ_16);
    fill_descriptor(&gdt[KERNEL_DS_16 / 8], 0, 0xffff, ACC_PL_K | ACC_DATA_W, SZ_16);

    gdtdesc.limit = sizeof(gdt) - 1;
    gdtdesc.linear_base = (unsigned long) &gdt;

    __asm__ __volatile__("lgdt %0" :: "m" (gdtdesc.limit));
    __asm__ __volatile__("lldt %w0" :: "r" (0));

    // Reload CS register containing code selector:
    __asm__ __volatile__ ("ljmp %0,$0f\n\t0:" :: "i" (KERNEL_CS));
    // Reload data segment registers:
    __asm__ __volatile__ ("mov %0, %%ax" :: "r" ((unsigned short)KERNEL_DS));
    __asm__ __volatile__ ("mov %ax, %ds");
    __asm__ __volatile__ ("mov %ax, %es");
    __asm__ __volatile__ ("mov %ax, %fs");
    __asm__ __volatile__ ("mov %ax, %gs");
    __asm__ __volatile__ ("mov %ax, %ss");
}
