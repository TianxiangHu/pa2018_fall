#include "cpu/instr.h"
#include"cpu/intr.h"

make_instr_func(int_i)
{
    OPERAND opr;
    opr.type = OPR_IMM;
	opr.sreg = SREG_CS; 
	opr.addr = cpu.eip + 1;
    opr.val=vaddr_read(opr.addr,opr.sreg,1);
    uint8_t intr_no=opr.val;
    raise_sw_intr(intr_no);

    int len=0;
    return len;
}