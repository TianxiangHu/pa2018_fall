#include "cpu/instr.h"
#include"memory/memory.h"

static void instr_execute_1op()
{
	cpu.gdtr.limit=vaddr_read(opr_src.addr,opr_src.sreg,2);
	cpu.gdtr.base=vaddr_read(opr_src.addr+2,opr_src.sreg,4);
}

make_instr_impl_1op(lgdt,rm,l);
