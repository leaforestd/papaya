#ifndef KU_MM_H
#define KU_MM_H
//memory infomation between 6M~16M~20M
#define BASE_PROCSTACK (16*0x100000)
#define LEN_PROCSTACK (0x1000*16)

typedef struct{
	int curr_pid;
}KINFO;
#define kinfo (*(KINFO*)0x100000)
#endif
