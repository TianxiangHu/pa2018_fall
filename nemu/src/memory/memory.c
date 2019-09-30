#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include "memory/cache.h"
#include <memory.h>
#include <stdio.h>

extern void init_cache();//shan
extern uint32_t cache_read(paddr_t paddr,size_t len);//shan
extern void cache_write(paddr_t paddr,size_t len,uint32_t data);//shan

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data) {
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;
	#ifdef CACHE_ENABLED
		ret = cache_read(paddr,len);
	#else
		int num=is_mmio(paddr);
		if(num!=(-1))
			ret = mmio_read(paddr,len,num);
		else
			ret = hw_mem_read(paddr, len);
	#endif
		return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data) {
	#ifdef CACHE_ENABLED
		cache_write(paddr,len,data);
	#else
		int num=is_mmio(paddr);
		if(num!=(-1))
			mmio_write(paddr,len,data,num);
		else
			hw_mem_write(paddr, len, data);
	#endif
}


uint32_t laddr_read(laddr_t laddr, size_t len) {
//	assert(len==1||len==2||len==4);
	if(cpu.cr0.pg==1)
	{
		uint32_t temp_s = laddr & 0xFFF;
		uint32_t temp_e = (temp_s+len) & 0xFFF;
		if(temp_s<temp_e||temp_e==0)//判断跨页
		{	
//			assert(0);
			uint32_t hwaddr = page_translate(laddr);
			return paddr_read(hwaddr, len);
		}	
		else
		{
			size_t len_2=(size_t)temp_e;
			size_t len_1=len-len_2;
			laddr_t laddr2=laddr+len_1;
			uint32_t ret=0,ret_1=0,ret_2=0;
			ret_1=laddr_read(laddr,len_1);
			ret_2=laddr_read(laddr2,len_2);
			ret_2=ret_2<<(len_1*8);
			ret=ret_1 | ret_2;
/*			printf("%d\n",len);
			printf("%d,%d\n",len_1,len_2);
			printf("%x,%x\n",laddr,laddr2);
			printf("\n");
*/			return ret;
		}
	}
	else
		return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data) {
//	assert(len==1||len==2||len==4);
	if(cpu.cr0.pg==1)
	{
		uint32_t temp_s = laddr & 0xFFF;
		uint32_t temp_e = (temp_s+len) & 0xFFF;
		if(temp_s<temp_e||temp_e==0)//判断跨页
		{	
//			assert(0);
			uint32_t hwaddr = page_translate(laddr);
			paddr_write(hwaddr, len, data);
		}
		else
		{
//			assert(0);
			size_t len_2=(size_t)temp_e;
			size_t len_1=len-len_2;
			laddr_t laddr2=laddr+len_1;
			uint32_t data_1 =data;
			uint32_t data_2 =data>>(len_1 * 8);
/*			printf("%d,%d\n",len,data);
			printf("%d,%d\n",len_1,len_2);
			printf("%d,%d\n",data_1,data_2);
			printf("%x,%x\n",laddr,laddr2);
			printf("\n");
*/			laddr_write(laddr,len_1,data_1);
			laddr_write(laddr2,len_2,data_2);
		}
	}
	else
		paddr_write(laddr, len, data);
}


uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	#ifndef IA32_PAGE
		return laddr_read(vaddr, len);
	#else
		uint32_t laddr=vaddr;
		if(cpu.cr0.pe==1)
		{
			laddr=segment_translate(vaddr,sreg);
		}
		return laddr_read(laddr,len);
	#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
	laddr_write(vaddr, len, data);
}

void init_mem() {
	// clear the memory on initiation
#ifdef CACHE_ENABLED
	init_cache();
#endif
	
	memset(hw_mem, 0, MEM_SIZE_B);

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t * get_mem_addr() {
	return hw_mem;
}
