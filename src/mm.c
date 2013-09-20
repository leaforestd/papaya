//1~256M空间全部做了对等映射，但内核只用到2M，似乎浪费
#include<disp.h>
#include<utils.h>
#include<mm.h>
#include<valType.h>
#include<proc.h>
static u8 *pgbmp;
static void check_dirent(u32*dir,int entry_id);
static void set_pgbmp(int start_page,int pages);
static void init(void);
void mm(void){
	oprintf("mm init..\n");
	init();
//	oprintf("%x\n",alloc_page());
//	oprintf("%x\n",alloc_page());
	while(1){
//		sleep(MSGTYPE_TIMER,100);
//		int a=1;
//		int b=2;
//		int c=3;
//		pcb_table[4].mod=3;
//		pcb_table[4].msg_type=MSGTYPE_CHAR;
//		oprintf("mm runing..");
//		int d=4;
//		int e=5;
//		int f=6;
//		oprintf("mm second runing..\n");
	}
}

void pgerr(void){
	/**
	__asm__ __volatile__(
			"movl %0,%%cr3"
			:
			:"r" (pt_pcb->cr3)
			:
	);
	*/
	u32 err_addr;
	__asm__ __volatile__(
			"movl %%cr2,%0"
			:"=r" (err_addr)
			:
			:
	);
	u32 ppg=alloc_page();
	oprintf("pgerr solved,err_addr:%x,ppg alloc:%x,ill proc continue..\n",err_addr,ppg);
//	while(1);
//	oprintf("proc_get_ppg:%x\n",proc_get_ppg(pcb_table_info.curr_pid,err_addr>>12));
	proc_map_pg(pcb_table_info.curr_pid,err_addr>>12,ppg,PG_USU,PG_RWW);
	fire(pcb_table_info.curr_pid);
}
static void check_dirent(u32*dir,int entry_id){
//dir-entry都是用户级别,存在，可写，身份识别统统放在table-entry
	if((dir[entry_id]&PG_P)==0){
		int ppg_id=alloc_page();
		dir[entry_id]=PG_RWW|PG_USU|PG_P|ppg_id<<12;
//		oprintf("meet empty dir-entry,alloc a physical page for linking table,ppg_id=%x\n",ppg_id);
	}
}
int get_ppg(u32*dir,int vpg_id){
	u32*tbl=(u32*)(dir[PG_H10(vpg_id)]>>12<<12);
	oprintf("@get_ppg tbl_addr:%x\n",tbl);
//	assert(tbl!=0)//commen for temorary
	return tbl[PG_L10(vpg_id)]>>12;
}
int proc_get_ppg(int pid,int vpg_id){
	u32*dir=(u32*)ADDR_PROC_PGDIR(pid);
	return get_ppg(dir,vpg_id);
}

void proc_map_stackpg(int pid){
	for(int i=0;i<PROC_STACK_PGS;i++){
		int stack_pg=PROCSTACK_STARTPG(pid)+i;
		proc_map_pg(pid,stack_pg,stack_pg,PG_USU,PG_RWW);
	}
}
void proc_map_kpg(int pid){//ERR 暂时的kernel page都是用户可读写的
	for(int pg_id=0;pg_id<KERNEL_SPACE_SIZE/PGSIZE;pg_id++){
		proc_map_pg(pid,pg_id,pg_id,PG_USS,PG_RWW);
	}
}
void proc_map_pg(int pid,int vpg_id,int ppg_id,int us,int rw){
	u32*dir=(u32*)ADDR_PROC_PGDIR(pid);
//	oprintf("@proc_map_pg,dir:%x\n",dir);
	map_pg(dir,vpg_id,ppg_id,us,rw);
}

void global_equal_map(void){
	u32*entry=(u32*)RING0_PGDIR;		
	for(int i=0;i<1024;i++){
		*entry=(RING0_PGTBL+i*0x1000)>>12<<12|PG_USS|PG_RWW|PG_P;//i thought '&0x1000' the same as '>>12<<12' at first,and debug a long time
		entry++;
	}
	entry=(u32*)RING0_PGTBL;
	for(int pg_id=0;pg_id<PAGES;pg_id++){
		*entry=pg_id<<12|PG_USS|PG_RWW|PG_P;
		entry++;
	}
}
void map_pg(u32*dir,int vpg_id,int ppg_id,int us,int rw){
//	CHECK_DIRENT(dir,PG_H10(vpg_id));
	check_dirent(dir,PG_H10(vpg_id));
	u32*tbl=(u32*)(dir[PG_H10(vpg_id)]>>12<<12);
//	oprintf("@map_pg page-table at %x ",tbl);
	tbl[PG_L10(vpg_id)]=ppg_id<<12|us|rw|PG_P;
}
/**
 * |---kernel(1M)---|---kernel pgdir(4k)---|---kernel pgtbl(64*4k)---|---100 pgdir(100*4k)---|
 */
static void init(void){
	pgbmp=kmalloc(PAGES/8);	
	set_pgbmp(0,KERNEL_SPACE_SIZE/PGSIZE);
	/**
	int seg1_start_page=0;
	int seg1_pages=KERNEL_SIZE/PGSIZE;
	int seg2_start_page=seg1_start_page+seg1_pages;
	int seg2_pages=1+MEMSIZE/0x400000;
	int seg3_start_page=seg2_start_page+seg2_pages;
	int seg3_pages=MAX_TASK;
	oprintf("mm init..\n");
	oprintf("KERNEL_SPACE_SIZE:%x\n",KERNEL_SPACE_SIZE);
	oprintf("BASE_PROCSTACK:%x,LEN_PROCSTACK:%x,PROC_STACK_PGS:%u\n",BASE_PROCSTACK,LEN_PROCSTACK,PROC_STACK_PGS);
//	oprintf("test procstack_startpg(3):%x\n",PROCSTACK_STARTPG(3));
//	oprintf("test addr_proc_pgdir(6):%x\n",ADDR_PROC_PGDIR(6));
	oprintf("memory used:\n");
	oprintf("seg1  start_page:%x,pages:%x\n",seg1_start_page,seg1_pages);
	oprintf("seg2  start_page:%x,pages:%x\n",seg2_start_page,seg2_pages);
	oprintf("seg3  start page:%x,pages:%x\n",seg3_start_page,seg3_pages);
	set_pgbmp(seg1_start_page,seg1_pages);
	set_pgbmp(seg2_start_page,seg2_pages);//kernel pgdir+pgtbl
	set_pgbmp(seg3_start_page,seg3_pages);//pgdir of 100 process
	*/
	//forgot to set proc-stack bit dirty,papaya will crack when mem used>BASE_PROCSTACK
}
int alloc_page(){
	int page_id=bt0(pgbmp,PAGES);
	assert(page_id!=-1)
	set_pgbmp(page_id,1);
//	oprintf("alloc_page return %x ",page_id);
	return page_id;
}
static void set_pgbmp(int start_page,int pages){
	for(int offset=0;offset<pages;offset++){
		bs(pgbmp,start_page+offset);
	}
}
