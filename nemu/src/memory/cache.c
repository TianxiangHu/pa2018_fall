#include "nemu.h"
#include "memory/cache.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include <stdio.h> 


#define ROW 128
#define LINE 8

CacheLine L1_dcache[ROW][LINE];//128组8行

void init_cache()
{
	int i=0,j=0;
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<LINE;j++)
		{
			L1_dcache[i][j].valid =false;
		}
	}
}

uint32_t cache_read(const paddr_t paddr,const size_t len)
{
	//assert(len == 1 || len == 2 || len == 4);
	/*因为返回最大为32位，所以需要保证len只能为以上值*/
	/*因为要处理跨行问题，所以不能限定len的值*/
	
	uint32_t row_temp=0,ret=0,flag_cache_temp=0,addr=0;
	/*分别为组号，返回值，标志信息，行内具体位置*/
	row_temp =(paddr>>6) & 0x7F;
	flag_cache_temp =(paddr>>13) & 0x7FFFFF;
	addr =paddr & 0x3F;
	
	uint32_t temp_s=0,temp_e=0;
	temp_s=paddr & 0x3F;
	temp_e=(temp_s+len) & 0x3F;
	
	//printf("paddr=%d,len=%d\n",paddr,len);
	if(temp_s<temp_e||temp_e==0)
	{/*此处处理未跨行的读取*/
		int i=0;
		bool flag=false;
		for(i=0;i<LINE;i++)
		{/*试图找到现成的cache块*/
			if(L1_dcache[row_temp][i].flag_cache==flag_cache_temp)
			{
				if(L1_dcache[row_temp][i].valid==true)
				{
					uint32_t ret_3=0;
					flag=true;
					memcpy(&ret_3,L1_dcache[row_temp][i].line_store + addr,len);
					//memcpy(&ret,hw_mem+paddr,len);//shan
					/*if(ret_3!=ret)//shan
						printf("ret_3=%d,ret=%d,paddr=%d,len=%d,temp_e=%d,temp_s=%d\n",ret_3,ret,paddr,len,temp_e,temp_s);//shan
					*/return ret_3;
				}
			}
		}
		if(flag==false)
		{/*找不到现成的cache块*/
			int i_temp_1=0,i_temp=0;
			for(i_temp_1=0;i_temp_1<LINE;i_temp_1++)
			{
				if(L1_dcache[row_temp][i_temp_1].valid==false)
				{/*找到一个空的行 占*/
					flag=true;
					i_temp =i_temp_1;
					break;
				}
			}
			if(flag==false)
			{
				i_temp =paddr % 8;//如果组内行均满了，则随机选一个行占
			}
			uint32_t addr_temp=paddr&0xFFFFFFC0;//找到该64B的起始位置
			L1_dcache[row_temp][i_temp].flag_cache=flag_cache_temp;
			memcpy(L1_dcache[row_temp][i_temp].line_store,hw_mem+addr_temp,64);
			L1_dcache[row_temp][i_temp].valid=true;
		//	printf("paddr=%d,len=%d,temp_e=%d,temp_s=%d\n",paddr,len,temp_e,temp_s);//shan			
			ret =cache_read(paddr,len);//再次调用该函数，读取返回值
			/*uint32_t ret_2=hw_mem_read(paddr,len);//shan
			if(ret_2!=ret)
				printf("ret_2=%d,ret=%d,paddr=%d,len=%d,temp_e=%d,temp_s=%d\n",ret_2,ret,paddr,len,temp_e,temp_s);//shan
			*/return ret;
		}
	}
	else
	{/*这里要开始处理跨行读问题；跨行问题暂且不管*/
	//ret =hw_mem_read(paddr,len);//shan
		uint32_t len_1=0,len_2=0;
		len_2 =temp_e;//跨到第二行的字节数
		len_1 =len-len_2;///在第一行需要读取的字节数
		paddr_t paddr_2 =paddr+len_1;//第二行的起始地址
		uint32_t ret_1=0,ret_2=0;
		ret_1 =cache_read(paddr,len_1);
		ret_2 =cache_read(paddr_2,len_2);
		//printf("len_1=%d,len_2=%d,ret_1=%d,ret_2=%d,paddr=%d,temp_e=%d,temp_s=%d,len=%d\n",len_1,len_2,ret_1,ret_2,paddr,temp_e,temp_s,len);//shan
		ret_2 =ret_2 << (len_1 * 8);
		ret =ret_1 | ret_2;/*ret_2为高位，ret_1为低位，两者或得到ret*/
		//ret_2 =hw_mem_read(paddr,len);//shan
		/*if(ret_2!=ret)//shan
			printf("ret_2=%d,ret=%d,paddr=%d,len=%d,temp_e=%d,temp_s=%d,len_1=%d,len_2=%d\n",ret_2,ret,paddr,len,temp_e,temp_s,len_1,len_2);//shan
		*/return ret;
	}
	//ret=hw_mem_read(paddr,len);//shan
	return ret;
}

void cache_write(const paddr_t paddr,const size_t len,const uint32_t data)
{
	uint32_t row_temp=0,flag_cache_temp=0,addr=0;
	/*分别为组号，标志信息，行内具体位置*/
	row_temp =(paddr>>6) & 0x7F;
	flag_cache_temp =(paddr>>13) & 0x7FFFFF;
	addr =paddr & 0x3F;
	
	uint32_t temp_s=0,temp_e=0;
	temp_s=paddr & 0x3F;
	temp_e=(temp_s+len) & 0x3F;
	//printf("paddr=%d,len=%d,data=%d,temp_s=%d,temp_e=%d\n",paddr,len,data,temp_s,temp_e);//shan
	if(temp_s<temp_e||temp_e==0)
	{/*写不跨行*/
	//printf("xie bu kuahang\n");//shan
		int i=0;
		bool flag=false;
		for(i=0;i<LINE;i++)
		{
			if(L1_dcache[row_temp][i].flag_cache==flag_cache_temp)
			{
				if(L1_dcache[row_temp][i].valid==true)
				{
				//	printf("why here?\n");//shan
					flag=true;
					memcpy(L1_dcache[row_temp][i].line_store+addr,&data,len);
					hw_mem_write(paddr,len,data);
				}
			}
		}
		if(flag==false)
		{
		//printf("Right here!\n");//shan
			hw_mem_write(paddr,len,data);
		}
	}
	else
	{/*写跨行，默认之前之后两行都是valid，就算不是也没关系……*/
//	printf("xie kua hang\n");
		size_t len_1=0,len_2=0;
		len_2 =(size_t)temp_e;//跨到第二行的字节数
		len_1 =len-len_2;///在第一行需要读取的字节数
		paddr_t paddr_2 =paddr+len_1;//第二行的起始地址
		uint32_t data_1 =data;
		uint32_t data_2 =data>>(len_1 * 8);
		//printf("data_1=%d,data_2=%d\n",data_1,data_2);//shan
		cache_write(paddr,len_1,data_1);
		cache_write(paddr_2,len_2,data_2);
		
		/*test,shan*/
		/*uint32_t ret=cache_read(paddr,len);
		uint32_t ret_2=hw_mem_read(paddr,len);
		if(ret!=data||ret_2!=ret||ret_2!=data)
			printf("data=%d,ret=%d,ret_2=%d,len=%d,paddr=%d,data_1=%d,data_2=%d\n",data,ret,ret_2,len,paddr,data_1,data_2);//shan
		*/
	}
//	hw_mem_write(paddr,len,data);
}




















