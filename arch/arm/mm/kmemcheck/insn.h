#ifndef _KMEMCHECK_INSN_
#define _KMEMCHECK_INSN_

#include <asm/ptrace.h>

enum kmemcheck_method {
        KMEMCHECK_READ,
        KMEMCHECK_WRITE,
};

typedef void (*action_check)(unsigned long insn, struct pt_regs *regs, 
			unsigned long *addr, unsigned long *size);
typedef void (*action_exec)(unsigned long insn, struct pt_regs *regs);

struct kmemcheck_action {
	unsigned long mask;
	unsigned long value;
	enum kmemcheck_method access_type;

	action_check check;
	action_exec  exec;
};

const struct kmemcheck_action *search_action_entry(unsigned  long insn);

#endif /* _KMEMCHECK_INSN_ */
