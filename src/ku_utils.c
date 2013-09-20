#include<ku_utils.h>

//任意一个字符序列到达end_flag，while就退出，此时再比较一下，可能其中一个先到达末尾。去掉一部分功能，可能出现短命令误判
#define CMP_GOON_WHEN(exp)\
while(exp){\
	if(*pt1!=*pt2) return 0;\
	pt1++;\
	pt2++;\
}\
return 1;
//end define CMP_GOON_WHEN
int hex_int(char x){
	if(x>='0'&&x<='9') return (x-48);
	if(x>='a'&&x<='f') return (x-87);
	return -1;
}
void memset(char*dest,int byte,int value){
	while(byte-->0){
		*dest=value;
		dest++;
	}
}

//if 'size' use byte as entity,you should set 'initial_scale_entity' as 0.and 1 when kb,2 when MB,3 when GB
int*human_memsize(int size,int initial_scale_count){
	static int gmkb[4];
	human_memsize_into(gmkb,size,initial_scale_count);
	return gmkb;
}
void human_memsize_into(int*gmkb,int size,int initial_scale_count){
	memset((char*)gmkb,0,16);
	for(int i=3-initial_scale_count;i>=0;i--){
		gmkb[i]=size%1024;
		size/=1024;
	}
}
int pow_int(int base,int exp){
	if(exp==0) return 1;
	int result=1;
	for(int i=0;i<exp;i++){
		result*=base;	
	}
	return result;
}
int ceil_divide(int a,int b){
	int quot=a/b;
	int remainder=a%b;
	return remainder==0?quot:quot+1;
}
void chars_to_str(char*str,char*chars){//endflag=\0,space
	while(*chars!=' '&&*chars!=0){
		*str=*chars;
		str++;
		chars++;
	}
	*str=0;//append a \0 to make str
}

void memcp(char*dest,char*src,int byte){
	while(byte>0){
		*dest=*src;
		dest++;
		src++;
		byte--;
	}
}

void memsetw(u16*dest,int word,u16 value){
	while(word>0){
		*(dest+word-1)=value;
		word-=1;
	}
}

int charscmp(char*pt1,char*pt2,int end_flag){
	switch(end_flag){
		case 0://normal string,'\0'
			CMP_GOON_WHEN((*pt1!=0)&&(*pt2!=0))
			break;
		case 1://'\0',' '
			CMP_GOON_WHEN((*pt1!=0)&&(*pt2!=0)&&(*pt1!=' ')&&(*pt2!=' '))
			break;
		default:
			while(1);
	}
}

int strcmp(char*pt1,char*pt2){
	while(*pt1==*pt2){
		pt1++;
		pt2++;
		if((*pt1&*pt2)==0) break;
	}
	return *pt1==*pt2;
}
