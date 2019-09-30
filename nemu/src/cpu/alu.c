#include "cpu/cpu.h"

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	res=dest+src;
	
	set_CF_add(res,src,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_OF_add(res,src,dest,data_size);

	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	res=dest+src+cpu.eflags.CF;
	
	set_CF_adc(res,src,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_OF_add(res,src,dest,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	res=dest-src;
	
	set_SF(res,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_OF_sub(res,src,dest,data_size);
	set_CF_sub(res,dest,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	res=dest-(src+cpu.eflags.CF);
	
	set_SF(res,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_OF_sbb(res,src,dest,data_size);
	set_CF_sbb(res,dest,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}


uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint64_t res=0;
	res=(uint64_t)dest * (uint64_t)src;
	
	set_CF_mul(res,data_size);
	set_OF_mul(res,data_size);
	
	return res ;
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	int64_t res=0;
	res=(int64_t)dest * (int64_t)src;
	
	set_CF_imul(res,data_size);
	set_OF_imul(res,data_size);
	
	return res ;
}

uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	assert(src!=0);
	uint32_t res=0;
	res= dest / src;
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	assert(src!=0);
	int32_t res=0;
	res= dest / src;
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_mod(uint64_t src, uint64_t dest) {
	assert(src!=0);
	uint32_t res=0;
	res= dest % src;
	
	return res;
}

int32_t alu_imod(int64_t src, int64_t dest) {
	assert(src!=0);
	uint32_t res=0;
	res= dest % src;
	
	return res;
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	src= src & (0xFFFFFFFF>>(32-data_size));
	dest= dest & (0xFFFFFFFF>>(32-data_size));
	res= dest & src;

	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
	set_PF(res);
	set_SF(res,data_size);
	set_ZF(res,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	src= src & (0xFFFFFFFF>>(32-data_size));
	dest= dest & (0xFFFFFFFF>>(32-data_size));
	res=src^dest;
	
	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
	set_PF(res);
	set_SF(res,data_size);
	set_ZF(res,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res=0;
	src= src & (0xFFFFFFFF>>(32-data_size));
	dest= dest & (0xFFFFFFFF>>(32-data_size));
	res=src | dest;
	
	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
	set_PF(res);
	set_SF(res,data_size);
	set_ZF(res,data_size);
	
	return res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t temp_res=dest & (0xFFFFFFFF >> (32-data_size));
	uint32_t res=temp_res;
	temp_res=temp_res << src;
	
	set_CF_shl(res,src,data_size);
	set_SF(temp_res,data_size);
	set_ZF(temp_res,data_size);
	set_PF(temp_res);
	
	return temp_res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t temp_res=dest & (0xFFFFFFFF >> (32-data_size));
	uint32_t res=temp_res;
	temp_res=temp_res >> src;
	
	set_CF_shr(res,src,data_size);
	set_SF(temp_res,data_size);
	set_ZF(temp_res,data_size);
	set_PF(temp_res);	
	
	return temp_res & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t temp_res=dest & (0xFFFFFFFF >> (32-data_size));
	uint32_t res=temp_res;
	uint32_t temp_c=0x80000000;
	int i=0;
	temp_res=temp_res >> src;
	if(sign((dest>>(data_size-1))<<31))
		{
			temp_c=temp_c >> (32-data_size);
			for (i = 0; i < src; i += 1)
			{
				temp_c=temp_c | (temp_c >> 1);
			}
			temp_res=temp_res | temp_c;
		}
	else
		;	
	
	set_CF_sar(res,src,data_size);
	set_SF(temp_res,data_size);
	set_ZF(temp_res,data_size);
	set_PF(temp_res);
	
	return temp_res  & (0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size) {
	assert(data_size==8||data_size==16||data_size==32);
	uint32_t res= alu_shl(src,dest,data_size);
	return res;
}


void set_CF_add(uint32_t result,uint32_t src,size_t data_size)
{
	result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
	src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
	cpu.eflags.CF=result<src;
}

void set_CF_adc(uint32_t result,uint32_t src,size_t data_size)
{
	result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
	src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
	if(cpu.eflags.CF==1)
		cpu.eflags.CF=(result<=src);
	else
		cpu.eflags.CF=(result<src);
}

void set_CF_sub(uint32_t result,uint32_t dest,size_t data_size)
{
	result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
	dest=sign_ext(dest&(0xFFFFFFFF>>(32-data_size)),data_size);
	cpu.eflags.CF=(result>dest);
}

void set_CF_sbb(uint32_t result,uint32_t dest,size_t data_size)
{
	result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
	dest=sign_ext(dest&(0xFFFFFFFF>>(32-data_size)),data_size);
	if(cpu.eflags.CF==1)
		cpu.eflags.CF=(result>=dest);
	else
		cpu.eflags.CF=(result>dest);
}

void set_CF_mul(uint64_t res,size_t data_size)
{
	int64_t test=0;
	switch(data_size)
	{
		case 8:
			res=res & 0x000000000000FF00;
			test=0x0000000000000000;
			break;
		case 16:
			res=res & 0x00000000FFFF0000;
			test=0x0000000000000000;
			break;
		case 32:
			res=res & 0xFFFFFFFF00000000;
			test=0x0000000000000000;	
			break;
		default:break;
	}		
	if(res==test)
		cpu.eflags.CF=0;
	else
		cpu.eflags.CF=1;
}

void set_CF_imul(int64_t res,size_t data_size)
{
	int64_t test=0;
	switch(data_size)
	{
		case 8:
			res=res & 0x000000000000FE00;
			test=0x0000000000000000;
			break;
		case 16:
			res=res & 0x00000000FFFE0000;
			test=0x0000000000000000;
			break;
		case 32:
			res=res & 0xFFFFFFFE00000000;
			test=0x0000000000000000;	
			break;
		default:break;
	}		
	if(res==test)
		cpu.eflags.CF=0;
	else
		cpu.eflags.CF=1;
}

void set_CF_shl(uint32_t result,uint32_t src,uint32_t data_size)
{
	result = result >> (data_size-src);
	result = result << 31;
	if(sign(result))
		cpu.eflags.CF=1;
	else
		cpu.eflags.CF=0;
}

void set_CF_shr(uint32_t result,uint32_t src,uint32_t data_size)
{
	result = result >> (src-1);
	result = result << 31;
	if(sign(result))
		cpu.eflags.CF=1;
	else
		cpu.eflags.CF=0;	
}

void set_CF_sar(uint32_t result,uint32_t src,uint32_t data_size)
{
	result = result >> (src-1);
	result = result << 31;
	if(sign(result))
		cpu.eflags.CF=1;
	else
		cpu.eflags.CF=0;	
}

void set_OF_add(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
	switch(data_size)
		{
			case 8:
				result=sign_ext(result & 0xFF,8);
				src = sign_ext(src & 0xFF,8);
				dest = sign_ext(dest & 0xFF,8);
				break;
			case 16:
				result=sign_ext(result & 0xFFFF,16);
				src = sign_ext(src & 0xFFFF,16);
				dest = sign_ext(dest & 0xFFFF,16);
				break;
			default : break;
		}
		if(sign(src)==sign(dest))
			{
				if(sign(dest)!=sign(result))
					cpu.eflags.OF=1;
				else
					cpu.eflags.OF=0;
			}
		else
			cpu.eflags.OF=0;
}

void set_OF_sub(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
	switch(data_size)
		{
			case 8:
				result=sign_ext(result & 0xFF,8);
				src = sign_ext(src & 0xFF,8);
				dest = sign_ext(dest & 0xFF,8);
				break;
			case 16:
				result=sign_ext(result & 0xFFFF,16);
				src = sign_ext(src & 0xFFFF,16);
				dest = sign_ext(dest & 0xFFFF,16);
				break;
			default : break;
		}
		src=~src;
		if(sign(src)==sign(dest))
			{
				if(sign(dest)!=sign(result))
					cpu.eflags.OF=1;
				else
					cpu.eflags.OF=0;
			}
		else
			cpu.eflags.OF=0;
}

void set_OF_sbb(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
	switch(data_size)
		{
			case 8:
				result=sign_ext(result & 0xFF,8);
				src = sign_ext(src & 0xFF,8);
				dest = sign_ext(dest & 0xFF,8);
				break;
			case 16:
				result=sign_ext(result & 0xFFFF,16);
				src = sign_ext(src & 0xFFFF,16);
				dest = sign_ext(dest & 0xFFFF,16);
				break;
			default : break;
		}
		//src=src+cpu.eflags.CF;
		src=~src;
		if(sign(src)==sign(dest))
			{
				if(sign(dest)!=sign(result))
					cpu.eflags.OF=1;
				else
					cpu.eflags.OF=0;
			}
		else
			cpu.eflags.OF=0;
}

void set_OF_mul(uint64_t res,size_t data_size)
{
	uint64_t test=0;
	switch(data_size)
	{
		case 8:
			res=res & 0x000000000000FF00;
			test=0x0000000000000000;
			break;
		case 16:
			res=res & 0x00000000FFFF0000;
			test=0x0000000000000000;
			break;
		case 32:
			res=res & 0xFFFFFFFF00000000;
			test=0x0000000000000000;	
			break;
		default:break;
	}		
	if(res==test)
		cpu.eflags.OF=0;
	else
		cpu.eflags.OF=1;
}

void set_OF_imul(int64_t res,size_t data_size)
{
	int64_t test=0;
	switch(data_size)
	{
		case 8:
			res=res & 0x000000000000FE00;
			test=0x0000000000000000;
			break;
		case 16:
			res=res & 0x00000000FFFE0000;
			test=0x0000000000000000;
			break;
		case 32:
			res=res & 0xFFFFFFFE00000000;
			test=0x0000000000000000;	
			break;
		default:break;
	}		
	if(res==test)
		cpu.eflags.OF=0;
	else
		cpu.eflags.OF=1;
}

void set_PF(uint32_t result)
{
	unsigned count=0,i=0;
	for (i = 0; i < 8; i += 1)
	{
		count+=sign(result<<(31-i));
	}
	if(count%2==0)
		cpu.eflags.PF=1;
	else
		cpu.eflags.PF=0;
}

void set_ZF(uint32_t result,size_t data_size)
{
	result=result&(0xFFFFFFFF>>(32-data_size));
	cpu.eflags.ZF=(result==0);
}

void set_SF(uint32_t result,size_t data_size)
{
	result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
	cpu.eflags.SF=sign(result);
}
