#ifndef ASM_ARM_KMEMCHECK_H
#define ASM_ARM_KMEMCHECK_H

#include <linux/types.h>
#include <asm/ptrace.h>

#ifdef CONFIG_KMEMCHECK
bool kmemcheck_active(struct pt_regs *regs);

void kmemcheck_show(struct pt_regs *regs);
void kmemcheck_hide(struct pt_regs *regs, int fail);

bool kmemcheck_fault(struct pt_regs *regs,
	unsigned long address, unsigned long error_code);
bool kmemcheck_trap(struct pt_regs *regs);

#define KMEMCHECK_BREAK_INSN 	".word 0x07f002f8\n"

#define KMEMCHECK_FIXUP(insns, back)		\
"\n 	@ kmemcheck_fixup\n"			\
"	.section .kmemcheck_fixup, \"ax\"\n"	\
insns						\
"	b	" #back	"\n"			\
KMEMCHECK_BREAK_INSN				\
"	.previous\n"

#define KMEMCHECK_TABLE(orgin, fixup)		\
"\n 	@ kmemcheck_table\n"			\
"	.section .kmemcheck_table, \"a\"\n"	\
"	.align	2\n"				\
"	.long	" #orgin "\n"			\
"	.long	" #fixup "\n"			\
"	.previous\n"

#else
static inline bool kmemcheck_active(struct pt_regs *regs)
{
	return false;
}

static inline void kmemcheck_show(struct pt_regs *regs)
{
}

static inline void kmemcheck_hide(struct pt_regs *regs)
{
}

static inline bool kmemcheck_fault(struct pt_regs *regs,
	unsigned long address, unsigned long error_code)
{
	return false;
}

static inline bool kmemcheck_trap(struct pt_regs *regs)
{
	return false;
}

#endif /* CONFIG_KMEMCHECK */

#endif
