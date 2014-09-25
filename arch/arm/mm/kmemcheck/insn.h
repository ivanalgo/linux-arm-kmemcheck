#ifndef _KMEMCHECK_INSN_
#define _KMEMCHECK_INSN_

#include <asm/ptrace.h>

typedef void (*action_check)(unsigned long insn, struct pt_regs *regs, 
			unsigned long *addr, unsigned long *size);
typedef int (*action_exec)(unsigned long insn, struct pt_regs *regs);

struct kmemcheck_action {
	unsigned long mask;
	unsigned long value;

	action_check check;
	action_exec  exec;
};

const struct kmemcheck_action *search_action_entry(unsigned  long insn);

struct kmemcheck_entry {
	unsigned long ldrex_start;
	unsigned long fixup_start;
};

#define KMEMCHECK_ARM_BREAKPOINT_INSTRUCTION	0x07f002f8

#endif /* _KMEMCHECK_INSN_ */
