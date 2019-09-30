#include "cpu/instr.h"

make_instr_func(call_near)
{
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eip+1+data_size/8);
	
	OPERAND rel;
    rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;
	rel.val=vaddr_read(rel.addr,rel.sreg,rel.data_size/8);

//	int offset = sign_ext(rel.val, data_size);
//	print_asm_1("jmp", "", 2, &rel);

//	cpu.eip += offset;
	cpu.eip+=rel.val;
//	printf("%x\n",cpu.eip);
    return 1+data_size/8;
}

make_instr_func(call_near_indirect)
{
	cpu.esp=cpu.esp-4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eip+1+data_size/8);
	
	int len=1;
	OPERAND rm;

	rm.data_size=32;
//	printf("%x,%x\n",cpu.eip,eip);
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eip+len);
/*这个地方因为是rm，有可能是寄存器也可能是操作数，所以指令长度由函数决定*/
//	printf("%x,%x\n",cpu.eip,rm.val);
	cpu.eip=rm.val;

	return 0;
}
