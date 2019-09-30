#include "cpu/instr.h"
#include"memory/memory.h"

make_instr_func(jmp_near) {
        OPERAND rel;
        rel.type = OPR_IMM;
		rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

	int offset = sign_ext(rel.val, data_size);
	print_asm_1("jmp", "", 2, &rel);

	cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short)
{
	OPERAND rel;
    rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;

    operand_read(&rel);

	int offset = sign_ext(rel.val, rel.data_size);
	print_asm_1("jmp", "", 2, &rel);

	cpu.eip += offset;

    return 1 + rel.data_size / 8;
}

make_instr_func(jmp_near_indirect)
{
	int len=1;
	OPERAND rm;

	rm.data_size=32;

	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	cpu.eip=rm.val;
	
	return 0;
}

make_instr_func(jmp_far_imm)
{
	OPERAND src;
	uint32_t sc = paddr_read(eip+5,2);
	uint32_t sreg = (sc >> 3) & 0x7;
	src.data_size=32;
	
	src.val = paddr_read(eip+1,4);
	
	cpu.eip=src.val;
	load_sreg(sreg);
	
	return 0;
}






