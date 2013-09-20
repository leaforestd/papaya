#ifndef KU_UTILS_H
#define KU_UTILS_H
#include<valType.h>
#define min(x,y) ((x)<(y)?(x):(y))
//eat a digit in format-hex,store it in x,and make 'pt 'jump the digit
//bug:can not distinguish capital in hex-string,like 0XFFF,should be 0xfff
#define eat_hex(pt,x)\
	char*__pt=(pt);\
	x=0;\
	if(*__pt!='0'||*(__pt+1)!='x'){\
		x=-1;\
		goto donothing;\
	}\
	__pt+=2;\
	if(!((*__pt>='0'&&*__pt<='9')||(*__pt>='a'&&*__pt<='f'))){\
		x=-1;\
		goto donothing;\
	}\
	while((*__pt>='0'&&*__pt<='9')||(*__pt>='a'&&*__pt<='f')) __pt++;\
	__pt--;\
	int __len=__pt-pt+1-2;\
	for(int __i=0;__i<__len;__i++){\
		x+=hex_int(*__pt)*pow_int(16,__i);\
		__pt--;\
	}\
	(pt)+=(2+__len);\
donothing:;
//end define eat_hex
//eat a digit in format-dec,store it in x,and make 'pt 'jump the digit
#define eat_dec(pt,x)\
	if(*pt<'0'||*pt>'9') goto donothing;\
	x=0;\
	char*__pt=(pt);\
	while(*(__pt+1)>='0'&&*(__pt+1)<='9') __pt++;\
	int __len=__pt-(pt)+1;\
	for(int __i=0;__i<__len;__i++){\
		x+=(*__pt-48)*pow_int(10,__i);\
		__pt--;\
	}\
	(pt)+=__len;\
donothing:;
//end define eat_dec
#define eat_dec_with_len(pt,x,x_len) \
	char*__pt=(pt);\
	while(*(__pt+1)>='0'&&*(__pt+1)<='9') __pt++;\
	len=__pt-(pt)+1;\
	for(int __i=0;__i<x_len;__i++){\
		x+=(*__pt-48)*pow_int(10,__i);\
		__pt--;\
	}\
	(pt)+=x_len;
char mem_entity[4];
void memcp(char*dest,char*src,int byte);
void memsetw(unsigned short *dest,int word,unsigned short value);
void memset(char*dest,int value,int byte);
void chars_to_str(char*str,char*chars);
int charscmp(char*pt1,char*pt2,int end_flag);
int ceil_divide(int a,int b);
int pow_int(int base,int exp);
void human_memsize_into(int*gmkb,int size,int initial_scale_count);
int*human_memsize(int size,int initial_scale_count);
int strcmp(char*pt1,char*pt2);
int hex_int(char x);
#endif
