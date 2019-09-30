#include "cpu/instr.h"

make_instr_func(ret_near)
{
	cpu.eip=laddr_read(cpu.esp,4);
	cpu.esp+=4;
	return 0;
}

make_instr_func(ret_near_imm16)
{
	OPERAND rel;
	rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
	rel.data_size = 16;
	rel.addr = eip + 1;
	operand_read(&rel);	
	
	cpu.eip=laddr_read(cpu.esp,4);
	cpu.esp+=4;
	cpu.esp+=rel.val;
	
	return 0;
}
