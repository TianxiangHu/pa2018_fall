#include "cpu/instr.h"

make_instr_func(pusha)
{
	uint32_t temp_esp=cpu.esp;

	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eax);

	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.ecx);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.edx);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.ebx);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,temp_esp);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.ebp);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.esi);
	
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.edi);
	
	int len=1;
	return len;
}
	
