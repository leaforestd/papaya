#ifndef MM_H
#define MM_H
#include<valType.h>
#include<ku_mm.h>
#define PROC_ENTRY (0x8048000)  //这个概念根本就是错的
#define PG_P 1
#define PG_NP 0
#define PG_USU 4
#define PG_USS 0
#define PG_RWW 2
#define PG_RWR 0

//#define MEMSIZE (0x100000000)								//mem size=4G
#define MEMSIZE 0x100000*256
#define PGSIZE (0x1000)
#define PAGES (MEMSIZE/PGSIZE)								//page count=1M

//memory 6M~16M~20M
#define HEAP_BASE (6*0x100000)
#define HEAP_SIZE (10*0x100000)
/**ku	#define BASE_PROCSTACK (16*0x100000)*/
/**ku	#define LEN_PROCSTACK (0x1000*16)*/							//do you know why i use 0x1000 instead of PGSIZE
#define MAX_TASK 64
#define KERNEL_SPACE_SIZE (BASE_PROCSTACK+LEN_PROCSTACK*MAX_TASK) //kernel_space ends when proc_stack_space ends
//memory infomation between 1M~6M
#define ADDR_KERNEL_INFO 0x100000							//start at 1M
#define RING0_PGDIR (ADDR_KERNEL_INFO+PGSIZE)				//+4K
#define RING0_PGTBL (RING0_PGDIR+PGSIZE)					//+4k
#define SIZE_RING0_PGTBLS (PAGES*4)				
#define BASE_PROC_PGDIR (RING0_PGTBL+SIZE_RING0_PGTBLS)		//+4M
#define SIZE_PROC_PGDIRS (MAX_TASK*PGSIZE)					


#define RING0_CR3 RING0_PGDIR
#define PROC_STACK_PGS (LEN_PROCSTACK/PGSIZE)

#define ADDR_PGDIR(pid)  (BASE_PROC_PGDIR+(pid)*PGSIZE)
#define PG_H10(pg_id) (pg_id>>10)
#define PG_L10(pg_id) (pg_id&(0x400-1))
#define ADDR_PROC_PGDIR(pid) (BASE_PROC_PGDIR+(pid)*PGSIZE)
#define PROCSTACK_STARTPG(pid) ((BASE_PROCSTACK+LEN_PROCSTACK*pid)/PGSIZE)
void map_pg(u32*dir,int vpg_id,int ppg_id,int us,int rw);
int alloc_page();
void proc_map_kpg(int pid);
void proc_map_pg(int pid,int vpg_id,int ppg_id,int us,int rw);
int get_ppg(u32*dir,int vpg_id);
int proc_get_ppg(int pid,int vpg_id);
void proc_map_stackpg(int pid);
void global_equal_map(void);
//ill macro
//dir-entry都是用户级别,存在，可写，身份识别统统放在table-entry
//#define CHECK_DIRENT(dir,entry_id)  dir[entry_id]=((dir[entry_id]&PG_P)==0?(PG_RWW|PG_P|PG_USU|alloc_page()<<12):dir[entry_id])
#endif
