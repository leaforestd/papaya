#include<proc.h>
#include<disp.h>
#include<utils.h>
#include<mm.h>
#include<ku_utils.h>
#include<fs.h>
extern int idle,p1,p2,sec_data;
extern void tty(void);
extern void tty1(void);
extern void p3(void);
extern void p4(void);
extern void hs(void);
extern void t(void);
extern void fs_ext(void);
extern void mm(void);
char cpu_string[16];
void kernel_c(){
	//init basic data&struct
	heap_init();
	proc_init();
	init_fs();
	mem_entity[0]='G';
	mem_entity[1]='M';
	mem_entity[2]='K';
	mem_entity[3]='B';
	k_screen_reset();
	init_debug();
	detect_cpu();
	oprintf("detecting cpu...cpu identify:%s\n",cpu_string);
//	oprintf("mm init..\nring0_pgdir:%x,ring0_pgtbl:%x,base_proc_pgdir:%x,addr_kernel_info:%x,pages:%x\n",RING0_PGDIR,RING0_PGTBL,BASE_PROC_PGDIR,ADDR_KERNEL_INFO,PAGES);
	global_equal_map();
	__asm__("mov $0x101000,%eax\t\n"
			"mov %eax,%cr3\t\n"
			"mov %cr0,%eax\t\n"
			"or $0x80000000,%eax\t\n"
			"mov %eax,%cr0\t\n"
			);
	oprintf("global page-mapping for kernel built..open MMU\n");
	create_kernel_process((int)&idle,9,0xffff,"idle",0);	//pid must =0
	create_kernel_process((int)hs,2,0xffff,"hs",1);//hs的时间片要非常多，保证在下一轮时间片重置之前不会被挂起 ERR:pid must =1
	create_kernel_process((int)fs_ext,4,10,"fs_ext",1);//pid must =2

	create_kernel_process((int)mm,3,10,"mm",1);//ERR mm has great prio,because it shall run and prepare condition for other process
	create_kernel_process((int)tty,5,10,"tty",1);
	create_kernel_process((int)&p1,8,10,"p1",1);
//	ofork(t1,9,15,"t1",1);
//	ofork(t2,9,5,"t2",1);
//	ofork((int)&p2,7,5,"p2",3);
	oprintf("basic process ofork done..now open IRQ,proc-dispatch begin\n");
	__asm__("sti");
	while(1);//内核陷入死循环，等待第一次时钟中断
}












