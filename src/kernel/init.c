#include "asm/asm.h"
#include "asm/pic.h"
#include "sys/const.h"
#include "sys/symbol.h"
#include "sys/proc.h"
#include "sys/proto.h"
#include "type.h"
#include "string.h"

PRIVATE void init_interrupt(void);
PRIVATE void init_idt_desc(u8 vector, u8 desc_type, int_handler handler, u8 privilege);
PRIVATE void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);
PRIVATE u32 seg2phys(u16 seg);

PUBLIC void init(void)
{
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n-----\"cstart\" begins-----\n");

    /* GDT */
    u16 *p_gdt_limit = (u16 *) (&gdt_ptr[0]);
    u32 *p_gdt_base  = (u32 *) (&gdt_ptr[2]);
    memcpy(&gdt,(void *) *p_gdt_base, *p_gdt_limit + 1);
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32) &gdt;

    init_pic(); /* 8259A */
    init_pit(); /* 8253 */
    init_interrupt();

   /*
    * UGLY!!!!!
    */ 
    /* Initialize TSS descriptor in GDT */
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[INDEX_TSS], 
        va2pa(seg2phys(SELECTOR_KERNEL_DS), &tss),
        sizeof(tss) - 1, DA_386TSS);
    tss.iobase = sizeof(tss);   /* No I/O bit map */

    /* Initialize LDT descriptor in GDT */
    PROCESS *p_proc = proc_table;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    for (int i = 0; i < NR_TASKS + NR_PROCS; i++) {
        init_descriptor(&gdt[selector_ldt >> 3], 
            va2pa(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].ldts),
            LDT_SIZE * sizeof(DESCRIPTOR) - 1, DA_LDT);
        p_proc++;
        selector_ldt += 1 << 3;
    }

    disp_str("-----\"cstart\" finished-----\n");
}

/* Exception handler defined in kernel.asm */
void divide_error(void);
void single_step_exception(void);
void nmi(void);
void breakpoint_exception(void);
void overflow(void);
void bounds_check(void);
void inval_opcode(void);
void copr_not_available(void);
void double_fault(void);
void copr_seg_overrun(void);
void inval_tss(void);
void segment_not_present(void);
void stack_exception(void);
void general_protection(void);
void page_fault(void);
void copr_error(void);

/* Hardware interrupt handler defined in kernel.asm */
void hwint00(void);
void hwint01(void);
void hwint02(void);
void hwint03(void);
void hwint04(void);
void hwint05(void);
void hwint06(void);
void hwint07(void);
void hwint08(void);
void hwint09(void);
void hwint10(void);
void hwint11(void);
void hwint12(void);
void hwint13(void);
void hwint14(void);
void hwint15(void);

PRIVATE void init_interrupt(void)
{
    /* IDT */
    u16 *p_idt_limit = (u16 *) (&idt_ptr[0]);
    u32 *p_idt_base  = (u32 *) (&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32) &idt;

    /* Exception */
    init_idt_desc(INT_VECTOR_DIVIDE,       DA_386IGate, divide_error,          PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DEBUG,        DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_NMI,          DA_386IGate, nmi,                   PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_BREAKPOINT,   DA_386IGate, breakpoint_exception,  PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_OVERFLOW,     DA_386IGate, overflow,              PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_BOUNDS,       DA_386IGate, bounds_check,          PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_OP,     DA_386IGate, inval_opcode,          PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_NOT,   DA_386IGate, copr_not_available,    PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault,          PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_SEG,   DA_386IGate, copr_seg_overrun,      PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_TSS,    DA_386IGate, inval_tss,             PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_SEG_NOT,      DA_386IGate, segment_not_present,   PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_STACK_FAULT,  DA_386IGate, stack_exception,       PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PROTECTION,   DA_386IGate, general_protection,    PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PAGE_FAULT,   DA_386IGate, page_fault,            PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_ERR,   DA_386IGate, copr_error,            PRIVILEGE_KRNL);
    /* Hardware interrupt */
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
    /* System call */
    init_idt_desc(INT_VECTOR_SYS_CALL, DA_386IGate, sys_call, PRIVILEGE_USER);
}

PRIVATE void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute)
{
    p_desc->limit_low = limit & 0xFFFF;
    p_desc->base_low = base & 0xFFFF;
    p_desc->base_mid = (base >> 16) & 0xFF;
    p_desc->attr1 = attribute & 0xFF;
    p_desc->limit_high_attr2 = ((limit >> 16) & 0xF) | (attribute >> 8) | 0xF;
    p_desc->base_high = (base >> 24) & 0xFF;
}

PRIVATE void init_idt_desc(u8 vector, u8 desc_type, int_handler handler, u8 privilege)
{
    GATE *p_gate = &idt[vector];
    u32 base = (u32) handler;
    p_gate->offset_low = base & 0xFFFF;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = desc_type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xFFFF;
}

/* Get the physics address acording to segment selector */
PRIVATE u32 seg2phys(u16 seg)
{
    DESCRIPTOR *p_desc = &gdt[seg >> 3];
    return (p_desc->base_high << 24) | (p_desc->base_mid << 16) | (p_desc->base_low);
}