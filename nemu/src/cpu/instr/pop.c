#include "cpu/instr.h"

static void instr_execute_1op() {
	operand_read(&opr_src);
	assert(opr_dest.data_size==16||opr_dest.data_size==32);
	
	OPERAND r_1;
	r_1.type=OPR_MEM;
	r_1.data_size=data_size;
	//r_1.sreg=SREG_CS;
	r_1.addr=cpu.esp;
	
	operand_read(&r_1);
	opr_src.val=r_1.val;
	operand_write(&opr_src);
	
	if(opr_src.data_size==16)
		cpu.esp=cpu.esp+2;
	else
		cpu.esp=cpu.esp+4;
	
}


make_instr_impl_1op(pop, r, v)
