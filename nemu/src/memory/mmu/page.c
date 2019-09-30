#include "cpu/cpu.h"
#include "memory/memory.h"

typedef union {
	struct {
		uint32_t pf_off		:12;
		uint32_t pt_idx		:10;
		uint32_t pdir_idx	:10;
	};
	uint32_t addr;
} PageAddr;

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr) {
#ifndef TLB_ENABLED
	/*printf("\nPlease implement page_translate()\n");
	assert(0);
	*/
	PageAddr *addr = (void *)&laddr;
	paddr_t pdir_base = cpu.cr3.val & ~PAGE_MASK;//取出高二十位，即第一级页表的基地址

	PDE pde;
	pde.val	= paddr_read(pdir_base + addr->pdir_idx * 4, 4);//得到第二级页表地址
	assert(pde.present);

	paddr_t pt_base = pde.val & ~PAGE_MASK;//得到第二级页表基地址
	PTE pte;
	pte.val = paddr_read(pt_base + addr->pt_idx * 4, 4);//得到页表中指定偏移位置的数据，即物理地址的高二十位
	assert(pte.present);

	return (pte.val & ~PAGE_MASK)| (laddr & PAGE_MASK);
	
#else	
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
