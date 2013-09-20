//进程休眠称为sleep，sleep有多种MSGTYPE
#include<mm.h>
#include "proc.h"
#include<utils.h>
#include<kbd_drv.h>
#include<disp.h>
#include<tty.h>
#include<fs.h>
#include<elf.h>
#include<struinfo.h>
extern int p2,i20h;
extern u32 ticks;
	//dispStr("pid_empty=",0x400);
//	dispInt(pid_empty);
static u32 old_ticks=0;
static int ticks_new=0; 
int size_stackframe=sizeof(STACK_FRAME);
//process only run at ring1,ring3
static int selector_plain_d[4]={(int)&selector_plain_d0,(int)&selector_plain_d1,0,(int)&selector_plain_d3};
static int selector_plain_c[4]={(int)&selector_plain_c0,(int)&selector_plain_c1,0,(int)&selector_plain_c3};
int eflags=0x1200;//IOPL=1,STI
static int count_dispatch=0;

static char*loadbuf;
static int proc_init_vspace(int pid,char*exec_file);
static void proc_load_seg(int pid,Elf32_Phdr*ph);
static void proc_init_pcb(int pid_empty,u32 addr,int prio,int time_slice,char*p_name,int ring);

void proc_dispatch(void){
//	oprintf("proc_dispath>>>>>>>>>>>>>>>>>>>\n\n\n\n\n");
	count_dispatch++;
	if(old_ticks<ticks){
		//说明进程调度是被时钟中断触发
		old_ticks=ticks;
		ticks_new=1;
	}
	else{
		ticks_new=0;
	}

	int curr_pid=pcb_table_info.curr_pid;
	pcb_table[curr_pid].time_slice--;

	//1,发现当前进程时间片耗尽,而且当前进程是活动进程
	//2,若休眠进程时间片耗尽，则不做expire。让它继续sleep，它也不可能再被唤醒。只能等待时间片重置.
	if(pcb_table[curr_pid].mod==TASKMOD_ACTIVE&&pcb_table[curr_pid].time_slice==0){
		//如果当前进程为活动状态,标识为过期
		if(pcb_table[curr_pid].mod==TASKMOD_ACTIVE){
			ACTIVE_EXPIRE(curr_pid);//宏碰到if，for要小心，语句里千万要加{}
		}
	}
	
	//2,如果只有idle进程在活动，重置时间片并激活expire进程。
	//该代码块活动时机：1,最后一个进程time_slice耗尽被expire   2,最后一个进程进入sleep状态
	if(pcb_table_info.num_active==1){//当所有普通进程时间片耗尽时，重置时间片; 这样效率很差,逻辑也不好，可以优化
		pcb_table_info.num_active+=pcb_table_info.num_expire;
		pcb_table_info.num_expire=0;//调整pcb_table_info信息	
		for(int pid=0;pid<100;pid++){
			pcb_table[pid].time_slice=pcb_table[pid].time_slice_full;//所有进程的时间片重置
			pcb_table[pid].mod==TASKMOD_EXPIRE?(pcb_table[pid].mod=TASKMOD_ACTIVE):0;//激活所有expire进程
		}
	}

	//3,支持code函数--》定时功能
	//注意，timer信息比较特殊，不能用sen_msg函数,papaya的send_msg能用就用，因为他没在最初的设计里。
	//ticks更新才进入该模块，
	if(ticks_new){
		for(int pid=0;pid<100;pid++){
			if(pcb_table[pid].mod==TASKMOD_SLEEP&&pcb_table[pid].msg_type==MSGTYPE_TIMER){
	//			dispInt(pcb_table[pid].msg_bind);
				if(pcb_table[pid].msg_bind==0){
					SLEEP_ACTIVE(pid);//唤醒到时函数
				}
				else{
					pcb_table[pid].msg_bind--;//未到时函数自减
	//				dispStr("msg_bind--",0x400);
				}
			}
		}
	}
	int pid_ok=pickNext();	
//	assert(pid_ok!=0);//允许idle进程运行
//	oprintf("count_dispatch:%u,pid to fire:%u\n",count_dispatch,pid_ok);
	pcb_table_info.curr_pid=pid_ok;
	fire(pid_ok);//far away now,abandon kernel-stack
}

/**
 * cr3 was pre-set before calling fire_asm,be careful that  MMU-page-map has been changed to usr-space
 */
void fire(int pid){
	kinfo.curr_pid=pid;
	PCB*pt_pcb=pcb_table+pid;
	if(pt_pcb->ring==3||pt_pcb->fix_cr3){
		__asm__ __volatile__(
				"movl %0,%%cr3"
				:
				:"r" (pt_pcb->cr3)
				:
		);
	}
	fire_asm((int)pt_pcb);
}


int pickNext(void){//return  min_prio active process's pid
	int minprio=9;
	int minprio_pid=0;
	for(int pid=0;pid<100;pid++){
		if(pcb_table[pid].mod==TASKMOD_ACTIVE&&pcb_table[pid].prio<=minprio){
			minprio=pcb_table[pid].prio;
			minprio_pid=pid;//record a promising pid
		}
	}
	return minprio_pid;
}

int getEmpty(void){
	int pid_empty=-1;
	for(int pid=0;pid<100;pid++){
		if(pcb_table[pid].mod==TASKMOD_EMPTY){
			pid_empty=pid;
			break;
		}
	}
	assert(pid_empty!=-1);
	return pid_empty;
}

//ERR  have not release related page and update page-bitmap
void kill(int pid){
	pcb_table[pid].mod=TASKMOD_EMPTY;	
}
void create_kernel_process(u32 addr,int prio,int time_slice,char*p_name,int ring){
	int pid_empty=getEmpty();
	proc_init_pcb(pid_empty,addr,prio,time_slice,p_name,ring);
}

static void proc_init_pcb(int pid_empty,u32 addr,int prio,int time_slice,char*p_name,int ring){
	//got pid
	PCB* pt_pcb=pcb_table+pid_empty;
	//fill pt_pcb->regs
	pt_pcb->regs.ss=(selector_plain_d[ring]);
	pt_pcb->regs.esp=BASE_PROCSTACK+LEN_PROCSTACK*(pid_empty+1)-4;//leave out room 4-byte for return_errno
	pt_pcb->regs.eflags=eflags;//IOPL=1,STI
	pt_pcb->regs.cs=(selector_plain_c[ring]);
	pt_pcb->regs.eip=addr;

	pt_pcb->regs.gs=pt_pcb->regs.fs=pt_pcb->regs.es=pt_pcb->regs.ds=(selector_plain_d[ring]);
	//fill other members
	pt_pcb->prio=prio;
	pt_pcb->time_slice=pt_pcb->time_slice_full=time_slice;
	pt_pcb->p_name=p_name;
	pt_pcb->pid=pid_empty;
	pt_pcb->mod=TASKMOD_ACTIVE;
	pt_pcb->ring=ring;
	pt_pcb->fix_cr3=0;

	obuffer_init(&(pt_pcb->obuffer));
	pcb_table_info.num_task++;
	pcb_table_info.num_active++;
	
//	if(ring<3) pt_pcb->cr3=RING0_CR3;
//	else pt_pcb->cr3=RING0_CR3;//暂时这样处理
	if(ring<3){
		pt_pcb->cr3=RING0_CR3;
	}
	else{
		pt_pcb->cr3=ADDR_PROC_PGDIR(pid_empty);	
	}
}

void proc_init(void){
	loadbuf=kmalloc(0x100000);
}

//must be called within proc-body,because askhs...but this function is more safe,it's static
static void proc_load_seg(int pid,Elf32_Phdr*ph){
	if(ph->p_type!=1){
		oprintf("ph->p_type=%u,not a segment for loading..\n");
		return;
	}
	oprintf("got a segment for loading..\n");
	char*pt_read=loadbuf+ph->p_offset;
	char*pt_read_end=pt_read+ph->p_filesz-1;
	int curr_vpg=ph->p_vaddr>>12;
	//load into first ppg
	int ppg=alloc_page();
	int offset_in_pg=ph->p_vaddr&(0x1000-1);//取低12位，为页内偏移
	int pg_data_size=0x1000-offset_in_pg;

	memcp((char*)(ppg<<12)+offset_in_pg,pt_read,min(pg_data_size,pt_read_end-pt_read+1));
	proc_map_pg(pid,curr_vpg++,ppg,PG_USU,PG_RWW);
	pt_read+=pg_data_size;//至多移动这么多
	while(pt_read<=pt_read_end){
		ppg=alloc_page();//分配物理页
		memcp((char*)(ppg<<12),pt_read,min(0x1000,pt_read_end-pt_read+1));//将数据加载到物理页
		pt_read+=0x1000;//调整读指针
		proc_map_pg(pid,curr_vpg++,ppg,PG_USU,PG_RWW);//建立物理页和虚拟页的映射
	}
}

//must be called within proc-body,because askhs...but this function is more safe,it's static
//bug:proc_map_kpg,proc_map_stackpg should be implement in proc.c instead of mm.c
static int proc_init_vspace(int pid,char*exec_file){
	loadfile(exec_file,loadbuf);	

	Elf32_Ehdr*header=(Elf32_Ehdr*)loadbuf;
	WORKON(header,Elf32_Ehdr);
	struinfo();
	Elf32_Phdr* ph=(Elf32_Phdr*)((u32)header+header->e_phoff);
	//map kernel pages(0~16M) for normal proc
	proc_map_kpg(pid);
	//map stack pages for normal proc
	proc_map_stackpg(pid);
	proc_map_pg(pid,ADDR_KERNEL_INFO/0x1000,ADDR_KERNEL_INFO/0x1000,PG_USU,PG_RWR);//how ugly!
	//proc common pg-map done,including stack-pg and kernel-room-pg
	int ph_num=header->e_phnum;
	for(int ph_id=0;ph_id<ph_num;ph_id++){
		proc_load_seg(pid,ph);
		ph++;
	}
	oprintf("proc_init_vspace done,e_entry=%u\n",header->e_entry);
	return header->e_entry;
}

//must be called within proc-body,because askhs...but this function is more safe,it's static
void create_usr_process(char*exec_file,int prio,int time_slice,char*p_name,int father_pid){
	int pid_empty=getEmpty();
	int entry=proc_init_vspace(pid_empty,exec_file);
	proc_init_pcb(pid_empty,entry,prio,time_slice,p_name,3);
}

void obuffer_init(OBUFFER* pt_obuffer){
	for(int i=0;i<size_buffer;i++) pt_obuffer->c[i]=0;
	pt_obuffer->head=0;
	pt_obuffer->tail=size_buffer-1;
	pt_obuffer->num=0;
}

void obuffer_push(OBUFFER* pt_obuffer,char c){
	assert(pt_obuffer->num<size_buffer)//环形数组满后，禁止push
	int next=(pt_obuffer->tail+1)%size_buffer;
	pt_obuffer->c[next]=c;
	
	pt_obuffer->num++;//update num
	pt_obuffer->tail=next;//move tail
}

unsigned char obuffer_shift(OBUFFER* pt_obuffer){
	if(pt_obuffer->num==0) return 0;
	int head=pt_obuffer->head;
	int c=pt_obuffer->c[head];

	pt_obuffer->head=(head+1)%size_buffer;//move head
	pt_obuffer->num--;//update num
	return c;
}

