#include "cpu/instr.h"

make_instr_func(leave)
{
	cpu.esp=cpu.ebp;
	cpu.ebp=vaddr_read(cpu.esp,0,4);
	cpu.esp+=4;
	return 1;
}

