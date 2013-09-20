#include<elf.h>
static void kfreee(void);
static void kmallocc(void);
static void disk(void);
static void detect_pci(void);
static void cat(char*path);
static void ls(char*path);
static int cmd_asciis_nextword(void);
static int read_cmos(int addr);
static void time(void);
static void info_pcb_table_info(void);
static void ps(void);
static void loader(char*path);
static void kernelsectionn(void);
//tty运行在ring1,是内核进程,因为是从ring3移过来的，所以保留了较多的系统调用
#include<struinfo.h>
#include<ku_utils.h>
#include<sys_call.h>
#include<tty.h>
#include<ku_proc.h>
#include<disp.h>
#include<hs.h>
#include<proc.h>
#include<fs.h>
#include<mm.h>
#include<utils.h>
#include<video_drv.h>
char tmp[64];
char*buff;
extern unsigned p0_addr;
static int front_pid;
#define SIZE_LOAD_BUF (64*1024)
static char*loadbuf;
#define CMD_MAX_LEN 128
//#define CMD_MAX_NUM 5
static char cmd_asciis[CMD_MAX_LEN];
static int cmd_len=0;
static char* pt_cmd=0;
static char arg0[16];
static char*cmd_arr[]={
"ps",
"time",
"ls",
"loader",
"cat",
"infopci",
"malloc",
"free",
"kernelsection"
};

static void (*func_arr[])()={
	ps,time,ls,loader,cat,detect_pci,kmallocc,kfreee,kernelsectionn
};
int CMD_MAX_NUM=sizeof(cmd_arr)/4;

void tty(void){
	oprintf("try to mount device1:partation:3:%s\n",mount("/mnt/",1,3)?"success":"failed");
	int crt_start;
	__asm__("cli");
	loadbuf=kmalloc(SIZE_LOAD_BUF);
	oprintf("tty running..welcome^.^\n");
	buff=kmalloc(23);
	for(int j=0;j<0;j++){
		oprintf("j=%u,i will do open\n",j);
//		int fd=open("/mnt/home/wws/dimg",0);
		int fd=open("/mnt/short");
		oprintf("fd:%u,inode:%u\n",fd,fd_table[fd].inode);
		int newseek=lseek(fd,-2,2);
		int r_bytes=read(fd,tmp,7);
		oprintf("r_bytes:%u,newseek:%u\n",r_bytes,newseek);
//		while(1);
	}
	while(1){
new_prompt:
		reset_cmd_asciis();
		memsetw((u16*)loadbuf,SIZE_LOAD_BUF/2,0);
		oprintf("#   ");
wait_input:
		while(1){
			unsigned ascii=getchar();
			switch(ascii){
				case 128://ctrl+l
					k_screen_reset();
					goto new_prompt;
				case 129://ctrl+c
					kill(front_pid);
					oprintf("^C\n");
					goto new_prompt;
				case 130://ctrl+u
					crt_start=get_start();
					set_start(crt_start>=80?crt_start-80:crt_start);
					goto wait_input;
				case 131://ctrl+d
					crt_start=get_start();
					set_start(crt_start<(80*24*4-1)?crt_start+80:crt_start);
					goto wait_input;
			}
			oprintf("%c",ascii);//oprintf函数有能力打印控制字符，但更多的ctrl，shift组合还要跟进
			if(ascii=='\n'){//回车符执行命令
				parse_cmd_asciis();
				break;
			}
			//执行到这儿，说明是普通输入,写入cmd_asciis
			write_cmd_asciis(ascii);
		}
	}
}

static void kernelsectionn(void){
	kernelsection();
}
static void kfreee(void){
	char*hex_str=arg0;	
	int addr;
	eat_hex(hex_str,addr);
	if(addr==-1){
		oprintf("bad size format\n");
		return;
	}
	kfree((void*)addr);
	info_heap();
}
static void kmallocc(void){
	char*hex_str=arg0;	
	int byte;
	eat_hex(hex_str,byte);
	if(byte==-1){
		oprintf("bad size format\n");
		return;
	}
	else if(byte+4<sizeof(EMPTY_BLOCK)){//申请byte太少
		oprintf("you are timid,can you ask for more?\n");
	}
	else {//正常申请
		kmalloc(byte);
	}
	info_heap();
}

static void disk(void){
	oprintf("%10s%10s%8s%8s%10s%10s\n","device","boot","start","count","sys_id","sys_string");	
	int gmkb_start_lba[4];
	int gmkb_size[4];
	for(int i=1;i<MAX_PARTATION;i++){
		DP*dp=g_dp[1]+i;
		if(dp->start_lba==0) continue;//empty dp
		int c=dp->count/2;//x kb
		int s=dp->start_lba/2;//s kb
		/**
		int x_scale_count=0;
		int s_scale_count=0;
		while(x>=1024){
			x/=1024;
			x_scale_count++;	
		}
		while(s>=1024){
			s/=1024;
			s_scale_count++;
		}
		*/
		human_memsize_into(gmkb_start_lba,s,1);	
		human_memsize_into(gmkb_size,c,1);	
		int max0=0;
		int max1=0;
		while(gmkb_start_lba[max0]==0) max0++;
		while(gmkb_size[max1]==0) max1++;
//		oprintf("max0:%u,max1:%u\n",max0,max1);
		oprintf("%10u%10u%4u%4c%4u%4c%10u%10s\n",i,dp->boot,gmkb_start_lba[max0],mem_entity[max0],gmkb_size[max1],mem_entity[max1],dp->sys_id,sys_string[dp->sys_id]);
	}
}
#define MK_BDF(bus,dev,func) ((u32)(bus<<8|dev<<3|func))
#define MK_PCIADDR(bus,dev,func,reg) ((u32)(1<<31|MK_BDF(bus,dev,func)<<8|reg<<2))
#define PCI_BUS_MAX 0xff
#define PCI_DEV_MAX 0x1f
#define PCI_FUNC_MAX 0x7
#define PCI_CONFIG_ADDR 0XCF8
#define PCI_CONFIG_DATA 0xCFC
static void detect_pci(void){
	for(int bus=0;bus<=PCI_BUS_MAX;bus++){
		for(int dev=0;dev<=PCI_DEV_MAX;dev++){
			for(int func=0;func<=PCI_FUNC_MAX;func++){
				u32 addr=MK_PCIADDR(bus,dev,func,0);
				out_dw(PCI_CONFIG_ADDR,addr);

				u32 t=in_dw(PCI_CONFIG_ADDR);
//				oprintf("out_dw:%u,in_dw:%u\n",addr,t);
				u32 device_vender=in_dw(PCI_CONFIG_DATA);
				u16 device=device_vender>>16;
				u16 vender=device_vender&0xffff;
				if(vender!=0xffff){
					oprintf("bus:%u,dev:%u,func:%u,vender:%u,device:%u\n",bus,dev,func,vender,device);
				}
			}
		}
	}
}
static void ls(char*path){
	loadfile(path,loadbuf);
	show_dir(loadbuf);
//	oprintf("%s\n",loadbuf);
}
static void cat(char*path){
	loadfile(path,loadbuf);
	oprintf("%s\n",loadbuf);
}
static int cmd_asciis_nextword(void){//aim:adjust pt_cmd
	assert(*pt_cmd!=' '&&*pt_cmd!=0)
	while(*pt_cmd!=' '&&*pt_cmd!=0){
		pt_cmd++;
	}
	//now point to a empty seg,jump it
	if(*pt_cmd==0) return 0;//check if meet cmd end
	//to here,the empty is a space,safe
	while((*pt_cmd)==' '||(*pt_cmd)==0){
		pt_cmd++;
	}
	if(*pt_cmd==0) return 0;//whether space-serials followed by a \-
	return 1;
	//now point to a new word's head
}

static void time(void){
	int y=read_cmos(9);
	int m=read_cmos(8);
	int d=read_cmos(7);
	int h=read_cmos(4);
	int mi=read_cmos(2);
	int s=read_cmos(0);
	oprintf("20%u-%u-%u %u:%u:%u\n",y,m,d,h,mi,s);
}
static int read_cmos(int addr){
	out_byte(0x70,addr);
	int x=in_byte(0x71);
	return (x&0xf)+(x>>4)*10;
}
static void loader(char*path){
	char*file_name=path;
	create_usr_process(path,8,10,file_name,-1);
}
static void ps(void){
	info_pcb_table_info();
	oprintf("%5s%10s%10s%10s%10s%12s%5s\n","pid","name","mod","msg_type","msg_bind","time_slice","pri0");
	for(int pid=0;pid<100;pid++){
		if(pcb_table[pid].mod==TASKMOD_EMPTY) break;
		PCB*p=pcb_table+pid;
		oprintf("%5u%10s%10u%10u%10u%12u%5u\n",pid,p->p_name,p->mod,p->msg_type,p->msg_bind,p->time_slice,p->prio);
	}
}
static void info_pcb_table_info(void){
	PCB_TABLE_INFO*info=&pcb_table_info;
	oprintf("@pcb_table_info>>> num_active:%u,num_expire:%u,num_sleep:%u,curr_pid:%u,num_task:%u\n",info->num_active,info->num_expire,info->num_sleep,info->curr_pid,info->num_task);
}
void show_dir(char*buf){//dir block was pre-loaded to buf 
	int offset=0;
	DIRENT*dirent=(DIRENT*)buf;
	while(dirent->record_len!=0){
		oprintf("%*s ",dirent->name_len,dirent->name);
		offset+=dirent->record_len;
		dirent=(DIRENT*)(buf+offset);
	}
	oprintf("\n");
}

//return  value in two approach,very intersting
void reset_cmd_asciis(){
	cmd_len=0;
	memsetw((u16*)cmd_asciis,CMD_MAX_LEN/2,0);
	pt_cmd=cmd_asciis;
}

void parse_cmd_asciis(){//一个粗略的命令解析函数
	for(int cmd_id=0;cmd_id<CMD_MAX_NUM;cmd_id++){
		if(charscmp(cmd_arr[cmd_id],cmd_asciis,1)==1){
			if(cmd_asciis_nextword()){
				chars_to_str((char*)arg0,(char*)pt_cmd);
			}
			func_arr[cmd_id](arg0);
			return;
		}
	}
	oprintf("invalid command '%s'\n",cmd_asciis);
}

//void cmd_asciis_
void write_cmd_asciis(unsigned ascii){
	//先判别是否为控制字符
	if(ascii=='\b'){//处理退格鍵，删除缓冲区一个字符
		if(cmd_len>0){
			cmd_len--;
			cmd_asciis[cmd_len]=0;
		}
	}
	//若ascii是可打印字符，写入缓冲区
	else{
		cmd_asciis[cmd_len]=ascii;
		cmd_len++;
	}
}

int getchar(void){
	int ascii;
	while((ascii=obuffer_shift(&pcb_table[pcb_table_info.curr_pid].obuffer))==0){//注意，ring3层的函数通过int80h进入ring0,想在rign0返回数值是要SET_PCB_EAX的
		sleep(MSGTYPE_CHAR,0);
	}
	return ascii;
}

