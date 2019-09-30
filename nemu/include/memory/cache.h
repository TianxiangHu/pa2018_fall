#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

typedef struct
{/*1024*1024*128=2^27，组号3位，低6位，标记18位*/
	bool valid;
	uint8_t line_store[64];//储存空间为64B，低6位
	uint32_t flag_cache;//作为标记存在，只比较低18位
} CacheLine;

//extern CacheLine L1_dcache[8][128];//128组8行

void init_cache();//初始化，只是将valid全部置为零/false
uint32_t cache_read(const paddr_t paddr,const size_t len);/*根据paddr寻找
组号、标记、块内地址;注意跨行的情景*/
void cache_write(const paddr_t paddr,const size_t len,const uint32_t data);/*全写法，非写分配*/

#endif

