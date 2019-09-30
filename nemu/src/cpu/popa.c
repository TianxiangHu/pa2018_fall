#include "cpu/instr.h"

make_instr_func(popa)
{
	cpu.edi=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.esi=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.ebp=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	//uint32_t temp=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.ebx=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.edx=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.ecx=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;

	cpu.eax=vaddr_read(cpu.esp,SREG_DS,4);
	cpu.esp+=4;
	
	int len =1;
	return len;
}