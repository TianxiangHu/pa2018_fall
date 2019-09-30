#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "cpu/instr.h"/*谨记只是为了方便取的巧，绝对不可以更改CPU中的值（你不是GDB！）*/
#include "monitor/elf.h"


#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	HEX,NUM,REG_E,IDENTIFIER,
	L_B, R_B,
	NEG, PTR, PLAINT, LDI,
	MULTI, DIVIDSION, MODULE,
	 ADD_SIGN,DECREA,
	 L_S, R_S,
	 J_G, J_GE, J_L, J_LE,
	 EQ, NEQ,
	 AND,
	 OR, 
	NOTYPE = 256
	/*按优先级自大到小排序,,虽然这样排了，但并没有什么用……*/
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},	// white space
	{"0[xX][0-9a-fA-F]{1,30}",HEX},
	{"[0-9]{1,30}",NUM},	//dec
	{"\\$e[abcd]x",REG_E},
	{"\\$e[sb]p",REG_E},
	{"\\$e[sd]i",REG_E},
	{"-",DECREA},
	{"<<",L_S},
	{">>",R_S},
	{">=",J_GE},
	{"<=",J_LE},
	{">",J_G},
	{"<",J_L},
	{"&&",AND},
	{"\\|\\|",OR},
	{"\\+", ADD_SIGN},
	{"\\*",MULTI},
	{"\\/",DIVIDSION},
	{"\\(",L_B},
	{"\\)",R_B},
	//{"\\-", '-'},		//'-'不是元字符
	{"!=",NEQ},
	{"==", EQ},
	{"!",PLAINT},
	{"~",LDI},
	{"%",MODULE},
	{"[a-zA-Z_][a-zA-Z1-9_]{1,30}",IDENTIFIER}
	/*以上为运算可能用到的操作符*/
	
	/*{"[0-9]+", NUM},
	{"$e[a,c,d,b]x", REG},
	{"$esp", REG},
	{"$esi", REG},
	{"[a-zA-Z]+[a-zA-Z0-9]*", SYMB},*/
	/*以上为特殊匹配*/
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int LENGTH;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */
				 /*
				 除了空格全部记下来算了，省得判断
				 */
			if(rules[i].token_type !=NOTYPE)
			{
				 if(substr_len<32)
				 {
				 	for(int temp_i=0;temp_i<substr_len;temp_i++)
				 	{
				 		tokens[nr_token].str[temp_i]=substr_start[temp_i];
				 		tokens[nr_token].LENGTH=temp_i;
				 	}
				 }
				 else
				 {
				 	//assert(substr_len<32);
				 	;
				 }
				 
				 tokens[nr_token].LENGTH +=1;
				 
				tokens[nr_token].type=rules[i].token_type;
				nr_token++;
			}
			//对于空格直接抛弃

/*				switch(rules[i].token_type) {
					default: tokens[nr_token].type = rules[i].token_type;
							 nr_token ++;
				}
*/
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

/*将字符串转换为数值,限十进制和十六进制、寄存器、变量名*/
static int str2uint32(char *s,int k,bool *success)
{
	int val=0;
	if(tokens[k].type==IDENTIFIER)//变量名的转换
	{	
		bool success_s2u =true;
		val =look_up_symtab(s,&success_s2u);
		if(success_s2u==true)
			;
		else
		{
			success_s2u =false;
			printf("Unavailable identifier :%.*s\n",tokens[k].LENGTH,s);
			return 0;
		}
	}
	else if(tokens[k].type==HEX)//十六进制的转换
	{
		int i =2;
		int temp_val =0;
		for ( ; i<tokens[k].LENGTH&&((s[i]<='9'&&s[i]>='0')||(s[i]<='f'&&s[i]>='a')||(s[i]<='F'&&s[i]>='A')); i += 1)
		{
			if(s[i]<='9'&&s[i]>='0')
				temp_val =s[i]-'0'+0;
			else if(s[i]<='f'&&s[i]>='a')
				temp_val =s[i]-'a'+10;
			else
				temp_val =s[i]-'A'+10;
				
			val =val*16+temp_val;
		}
	}
	else if(tokens[k].type==NUM)//十进制的转换
	{
		int i =0;
		int temp_val =0;
		for ( ; i<tokens[k].LENGTH&&(s[i]<='9'&&s[i]>='0'); i += 1)
		{
			temp_val =s[i]-'0'+0;
				
			val =val*10+temp_val;
		}
	}
	else//寄存器的转换
	{
		char store[4];
		int i=0;
		for (; i < 4; i += 1)
		{
			store[i]=s[i];
		}
		if(store[3]=='x')
		{
			switch(store[2])
			{
				case 'a':return cpu.eax;
				case 'b':return cpu.ebx;
				case 'c':return cpu.ecx;
				case 'd':return cpu.edx;
				default: {val =0;
				printf("NOT MATCHING!\n");}
			}
		}
		else if(store[3]=='p')
		{
			switch(store[2])
			{
				case 's':return cpu.esp;
				case 'b':return cpu.ebp;
				default: {val =0;
				printf("NOT MATCHING!\n");}
			}
		}
		else
		{
			switch(store[2])
			{
				case 's':return cpu.esi;
				case 'd':return cpu.edi;
				default: {val =0;
				printf("NOT MATCHING!\n");}
			}
		}	
	}
	
	return val;
}

int val_find(int x)
{
	switch(tokens[x].type)
	{
		case ADD_SIGN :return 4;
		case DECREA :return 4;
		case MULTI :return 3;
		case DIVIDSION :return 3;
		case MODULE :return 3;
		case AND :return 11;
		case OR  :return 12;
		case J_GE:return 6;
		case J_LE:return 6;
		case J_G :return 6;
		case J_L :return 6;
		case EQ  :return 7;
		case NEQ :return 7;
		case L_S :return 5;
		case R_S :return 5;
		case NEG :return 2;
		case LDI :return 2;
		case PLAINT :return 2;
		default :return 0;
		printf("val_find\n");
	}
	return 0;
}

/*在p、q之间寻找正确的dominant operator,返回其在tokens中位置*/
int find_domin_opr(int p,int q)
{	
	//printf("p=%d,q=%d\n",p,q);
	int result =0;
	int i=0; 
	int type=0;
	for (i=p; i <= q; i =i+1)
	{
	//	printf("i=%d\n",i);
		if(tokens[i].type==L_B)
		{
			int l_count =1,r_count =0;
			i =i+1;
			for(;l_count>r_count&&i<=q;i =i+1)
			{
				if(tokens[i].type==L_B)
					l_count+=1;
				else if(tokens[i].type==R_B)
					r_count+=1;
				else
					;
			}
			i =i-1;
		}
		else
		{
			int val=val_find(i);
			if(val>=type)
			{/*按BNF规则，同优先级先计算右边的符号*/
				type =val;
				result =i;
			}
		}
	}
	return result;
}

bool check_parentheses(int p,int q,bool *success)
{
	bool flag =false;
	int end =q+1;
	int l_count =0;
	int r_count =0;
	int i=p;
	if(tokens[p].type==L_B&&tokens[q].type==R_B)
		flag =true;
	for (; i<end; i += 1)
	{
		if(tokens[i].type==L_B)
			l_count+=1;
		else if(tokens[i].type==R_B)
		{
			r_count+=1;
			if(r_count>l_count)//因为从'('开始计数，所以')'数量大于'('数量时可知表达式非法
			{
				flag =false;
			//	printf("2\n");
					
				*success =false;
			}
		}
		else
			;
	}
	if(r_count==l_count)
		;
	else
	{
		flag =false;
		//printf("3\n");
		*success =false;
	}
	return flag;
}

/*p,q分别指向表达式开头和结尾，相等则表达式只有一个元素*/
static int eval(int p,int q,bool *success)
{
	//printf("p=%d,q=%d\n",p,q);
	int val =0;
	if(p>q)
	{
	//	printf("4\n");
		*success=false;
		return 0;
		/*不应该发生的错误*/
	}
	else if(p==q)
	{/*此处检验一次表达式合法性——一般情况下操作数不与操作数相连*/
		if(tokens[p].type==NUM||tokens[p].type==HEX||tokens[p].type==REG_E||tokens[p].type==IDENTIFIER)
			{
				val =str2uint32(tokens[p].str,p,success);
			/*	printf("p==q,val,here\n");
				if(*success==false)
					{printf("HEHE\n");}*/
			}
		else
		{
			*success =false;
	//		printf("5\n");
			}
	/*	printf("p==q,here\n");
		if(*success==false)
			{printf("HEHEHE\n");}*/
		return val;

	}
	else if(check_parentheses(p,q,success)==true)//顺带判断左右括号是否匹配
	{
		return eval(p+1,q-1,success);
		/*如果是(a+B)形式，则去掉括号*/
	}
	else
	{
		if(*success==false)
			return 0;
	
		//printf("q-p=%d\n",q-p);
		if((q-p>1)&&tokens[p].type!=NEG&&tokens[p].type!=PTR){/*双目操作符*/
		int op=0,val1=0,val2=0;
		op =find_domin_opr(p,q);
		//printf("op=%d\n",op);
		val1 =eval(p,op-1,success);
		val2 =eval(op+1,q,success);
		switch(tokens[op].type)
		{
			case ADD_SIGN :return val1+val2;
			case DECREA :return val1-val2;
			case MULTI :return val1*val2;
			case DIVIDSION :return val1/val2;
			case MODULE :return val1%val2;
			case AND :return val1&&val2;
			case OR  :return val1||val2;
			case J_GE:return val1>=val2;
			case J_LE:return val1<=val2;
			case J_G :return val1>val2;
			case J_L :return val1<val2;
			case EQ  :return val1==val2;
			case NEQ :return val1!=val2;
			case L_S :return val1<<val2;
			case R_S :return val1>>val2;
			default:*success =false;
			//printf("6\n");
			/*根据type进行不同的操作*/
		}
		return 0;
		}
		else
		{/*单目操作符*/
			//printf("单目操作符\n");
			int val=0;
			if(tokens[p].type==PTR)
			{
				uint32_t addr_temp =eval(p+1,q,success);
				val =vaddr_read(addr_temp,0,4);//还是不知道第二个参数做什么的……
				return val;
			}
			else
			{
				int op=p;
				val=eval(p+1,q,success);
				switch(tokens[op].type)
				{
					case NEG :return -val;
					case LDI :return ~val;
					case PLAINT :return !val;
					default:*success =false;
					//printf("7\n");
				}
			}
		}
	}
	return 0;
}

int expr(char *e, bool *success) {
	*success=true;
	if(!make_token(e)) {
		//printf("8\n");
		*success = false;
		return 0;
	}

/*	printf("\nPlease implement expr at expr.c\n");
	assert(0);
*/

	int val =0;
	/*分开*与取值符号、-与取负符号*/
	int i=0;
	for(;i<nr_token;i++)
	{
		if(tokens[i].type==MULTI)
		{
			if(i==0||(tokens[i-1].type!=HEX&&tokens[i-1].type!=NUM&&tokens[i-1].type!=R_B&&tokens[i-1].type!=L_B&&tokens[i-1].type!=REG_E))
				tokens[i].type =PTR;
		}
		else if(tokens[i].type==DECREA)
		{
			if(i==0||(tokens[i-1].type!=HEX&&tokens[i-1].type!=NUM&&tokens[i-1].type!=R_B&&tokens[i-1].type!=L_B&&tokens[i-1].type!=REG_E))
				tokens[i].type =NEG;
		}
	}
	/*if(*success==false)
		printf("HEHEHEHE\n");
	printf("nr_token:%d\n",nr_token);*/
	val =eval(0,nr_token-1,success);//减一是为了使后者表示最后一个成员的下标
	/*if(*success==false)
		{printf("WHY\n");
		printf("val:%d\n",val);
		}*/

	return val;
}

