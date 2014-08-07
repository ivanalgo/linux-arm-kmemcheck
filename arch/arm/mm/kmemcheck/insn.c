#include <linux/kernel.h>
#include <linux/bug.h>

#include "insn.h"

static unsigned long insn_field_value(unsigned long insn, int fstart, int fend)
{
	return (insn >> fstart) & ((1 << (fend - fstart + 1)) - 1);
}

void strb_check(unsigned long insn, struct pt_regs *regs, 
		unsigned long *addr, unsigned long *size)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long offset = insn_field_value(insn, 0, 11);
	unsigned long u = insn_field_value(insn, 23, 23);
	unsigned long b = insn_field_value(insn, 22, 22);

	if (u)
		*addr = regs->uregs[rn] + offset;
	else
		*addr = regs->uregs[rn] - offset;

	if (b)
		*size = 1;
	else
		*size = 4;
}

void strbr_check(unsigned long insn, struct pt_regs *regs,
		 unsigned long *addr, unsigned long *size)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long rm = insn_field_value(insn, 0, 3);
	unsigned long u = insn_field_value(insn, 23, 23);
	unsigned long b = insn_field_value(insn, 22, 22);

	*addr = u? regs->uregs[rn] + regs->uregs[rm]: 
		   regs->uregs[rn] - regs->uregs[rm];
	*size = b? 1 : 4;
}

void strb_exec(unsigned long insn, struct pt_regs *regs)
{
        unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long offset = insn_field_value(insn, 0, 11);
	unsigned long rd = insn_field_value(insn, 12, 15);
        unsigned long u = insn_field_value(insn, 23, 23);
        unsigned long b = insn_field_value(insn, 22, 22);
	unsigned address = u? regs->uregs[rn] + offset : regs->uregs[rn] - offset;

	if (b)
		*(unsigned char *)address = (unsigned char)regs->uregs[rd];
	else
		*(unsigned long *)address = regs->uregs[rd];


}

void strbr_exec(unsigned long insn, struct pt_regs *regs)
{
        unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long rm = insn_field_value(insn, 0, 3);
	unsigned long rd = insn_field_value(insn, 12, 15);
        unsigned long u = insn_field_value(insn, 23, 23);
        unsigned long b = insn_field_value(insn, 22, 22);
	unsigned address = u? regs->uregs[rn] + regs->uregs[rm] :
				regs->uregs[rn] - regs->uregs[rm];

	if (b)
		*(unsigned char *)address = (unsigned char)regs->uregs[rd];
	else
		*(unsigned long *)address = regs->uregs[rd];

}

void stmia_check(unsigned long insn, struct pt_regs *regs,
		 unsigned long *addr, unsigned long *size)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long reglist = insn_field_value(insn, 0, 15);
	unsigned long cnt = 0;

	*addr = regs->uregs[rn];

	while(reglist) {
		if (reglist & 1)
			cnt++;

		reglist >>= 1;
			
	}

	*size = sizeof(unsigned long) * cnt;

}

void stmia_simulate(unsigned long insn, struct pt_regs *regs)
{
        unsigned long rn = insn_field_value(insn, 16, 19);
        unsigned long reglist = insn_field_value(insn, 0, 15);
        unsigned long w = insn_field_value(insn, 21, 21);
	unsigned long s = insn_field_value(insn, 22, 22);
        unsigned long cnt = 0;
	unsigned long *addr;
	int i;

	addr = (unsigned long *)regs->uregs[rn];

	for (i = 0; i < 16; i++) {
		if (reglist & (1 << i)) {
			*addr = regs->uregs[i];
			++addr;	
			++cnt;
		}
	}
	
	if (w)
		regs->uregs[rn] += sizeof(unsigned long) * cnt;	

	if (s)
		BUG();
}

void strb_rpi_check(unsigned long insn, struct pt_regs *regs,
		    unsigned long *addr, unsigned long *size)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long b  = insn_field_value(insn, 22, 22);

	*addr = regs->uregs[rn];
	*size = b? 1 : 4;
}

void strb_rpi_exec(unsigned long insn, struct pt_regs *regs)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long rd = insn_field_value(insn, 12, 15);
	unsigned long rm = insn_field_value(insn,  0,  3);
	unsigned long b  = insn_field_value(insn, 22, 22);
	unsigned long u  = insn_field_value(insn, 23, 23);

	if (b)
		*(unsigned char *)regs->uregs[rn] = (unsigned char)regs->uregs[rd];
	else
		*(unsigned long *)regs->uregs[rn] = regs->uregs[rd];

	regs->uregs[rn] = u? regs->uregs[rn] + regs->uregs[rm] :
			     regs->uregs[rn] - regs->uregs[rm];
}

void ldrb_reg_check(unsigned long insn, struct pt_regs *regs,
		    unsigned long *addr, unsigned long *size)
{
	unsigned long rn = insn_field_value(insn, 16, 19);
	unsigned long rm = insn_field_value(insn,  0,  3);
	unsigned long u  = insn_field_value(insn, 23, 23);
	unsigned long b  = insn_field_value(insn, 22, 22);

	*addr = u? regs->uregs[rn] + regs->uregs[rm] :
		   regs->uregs[rn] - regs->uregs[rm];

	*size = b? 1 : 4;
}

void ldrb_reg_exec(unsigned long insn, struct pt_regs *regs)
{
        unsigned long rn = insn_field_value(insn, 16, 19);
        unsigned long rd = insn_field_value(insn, 12, 15);
        unsigned long rm = insn_field_value(insn,  0,  3);
        unsigned long u  = insn_field_value(insn, 23, 23);
        unsigned long b  = insn_field_value(insn, 22, 22);
	unsigned long addr = u? regs->uregs[rn] + regs->uregs[rm]:
				regs->uregs[rn] - regs->uregs[rm];

	if (b)
		regs->uregs[rd] = *(unsigned char *)addr;
	else
		regs->uregs[rd] = *(unsigned long *)addr;
	
}

struct kmemcheck_action arm_action_table[] = {
	/* str{b} rd, [rn, #offset] */
	{ .mask = 0x0f300000, .value = 0x05000000, KMEMCHECK_WRITE,
		.check = strb_check, .exec = strb_exec },	
	/* str{b}, rd, [rn, rm] */
	{ .mask = 0x0f300ff0, .value = 0x07000000, KMEMCHECK_WRITE,
		.check = strbr_check, .exec = strbr_exec },
	/* str{b}, rd, [rn], rm */
	{ .mask = 0x0f300000, .value = 0x06000000, KMEMCHECK_WRITE,
		.check = strb_rpi_check, .exec = strb_rpi_exec },
	/* stmia rn, {list} */
	{ .mask = 0x0f900000, .value = 0x08800000, KMEMCHECK_WRITE,
		.check = stmia_check, .exec = stmia_simulate },

	/* ldr{b}, rd, [rn, rm] */
	{ .mask = 0x0f300000, .value = 0x07100000, KMEMCHECK_READ,
		.check = ldrb_reg_check, .exec = ldrb_reg_exec },

};

#define ACTION_SIZE	sizeof(arm_action_table)/sizeof(arm_action_table[0])

const struct kmemcheck_action *search_action_entry(unsigned  long insn)
{
	int i;

	for (i = 0; i < ACTION_SIZE; ++i)
		if ((insn & arm_action_table[i].mask) == arm_action_table[i].value)
			return &arm_action_table[i];

	return NULL;
}
