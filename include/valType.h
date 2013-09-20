//不要随便改动，很多包含文件都没有声明依赖性
#ifndef VALTYPE_H
#define VALTYPE_H
typedef int boolean;
#define true 1
#define false 0
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;
//following var-type for ext2-header
typedef unsigned __le32;
typedef unsigned __u32;
typedef unsigned short __le16;
typedef unsigned short __u16;
typedef unsigned char __u8;
typedef unsigned Elf32_Word;
typedef unsigned Elf32_Off;
typedef unsigned Elf32_Addr;
typedef unsigned short Elf32_Half;
#define NULL 0
typedef struct descriptorr{
	u16 limit_01;
	u16 base_01;
	u8 base_2;
	u8 attr1;
	u8 limit_attr;
	u8 base_3;
		
}DESCRIPTOR;
#endif
