#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg) {
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base=cpu.segReg[sreg].base;
	uint32_t addr=base+offset;
	return addr;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg) {
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	 //这个部分是用来完成对于隐藏部分的base,limit进行填装，但因为base和limit的值是固定的，那么赋值就行了不是么……
	 cpu.segReg[sreg].base=0;
	 cpu.segReg[sreg].limit=0xFFFFFFFF;
}
