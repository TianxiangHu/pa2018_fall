#include "cpu/instr.h"

static void instr_execute_1op() {
	assert(opr_src.data_size==8||opr_src.data_size==16||opr_src.data_size==32);
	
	operand_read(&opr_src);
	if(opr_src.data_size==16)
		cpu.esp=cpu.esp-2;
	else if(opr_src.data_size==32)
		cpu.esp=cpu.esp-4;
	else
	{
		cpu.esp=cpu.esp-data_size/8;
		opr_src.val=sign_ext(opr_src.val,opr_src.data_size);
	}
		
	OPERAND r_1;
	r_1.type=OPR_MEM;
	r_1.data_size=data_size;
	//r_1.sreg=SREG_CS;
	r_1.addr=cpu.esp;
	
	r_1.val=opr_src.val;
	operand_write(&r_1);
}

make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, i, v)
make_instr_impl_1op(push, i, b)//涉及立即数拓展？/!
