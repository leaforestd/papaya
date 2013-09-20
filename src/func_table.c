#include<disp.h>
#include<proc.h>
#include<hs.h>		//COMMAND_READ..
#include<fs.h>
/**
void k_dotchar(unsigned ascii,int mod,int line,int col){
	char*pt_0=(char*)0xb8000;
	char*position=pt_0+(line*80+col)*2;
//	*position=ascii;
	(*position)++;
	*(position+1)=mod;
	fire(pcb_table_info.curr_pid);//返回ring3
}
*/
//eax=0,写屏函数，在disp.c里
//对void k_show_chars(char*pt_head,unsigned var_type)的包装
void _k_show_chars(char*pt_head,unsigned end_flag){
	k_show_chars(pt_head,end_flag);
	fire(pcb_table_info.curr_pid);
}

//eax=1,阻塞函数，使当前进程进入休眠，并放弃时间片
//far-away
//内核进程要想休眠，不要使用这个函数------------》好在papaya的内核进程都在ring1
void k_sleep(int msg_type,int msg_bind){
	ACTIVE_SLEEP1(pcb_table_info.curr_pid,msg_type,msg_bind);
//	oprintf("k_sleep:pcb_table[1].msg_type:%u\n",pcb_table[1].msg_type);
	//进程已设置成休眠状态，马上放弃时间片
	proc_dispatch();
}

//eax=2
int k_obuffer_shift(void){
	int shift_result=obuffer_shift(&pcb_table[pcb_table_info.curr_pid].obuffer);
	SET_PID_EAX(pcb_table_info.curr_pid,shift_result);
	fire(pcb_table_info.curr_pid);//直接返还到调用者，不做进程调度
	return 0;//这条return函数只是摆设，返回值早已写到pcb里
}

//eax=3
//包装了void k_show_var(unsigned var,unsigned var_type);在屏幕上打印一个类型变量，定义在disp.c
void _k_show_var(unsigned var,unsigned var_type){
	k_show_var(var,var_type);
	fire(pcb_table_info.curr_pid);//直接返还到调用者，不做进程调度
}

//eax=4
void k_open(char*path,int mod){
	oprintf("@k_open pcb_table_info.curr_pid=%u\n",pcb_table_info.curr_pid);
	ACTIVE_SLEEP(pcb_table_info.curr_pid);	
	askfs(COMMAND_OPEN,path,0,0,0,0,0,0);
}
//eax=5
void k_read(int fd,char*buf,int size){
	ACTIVE_SLEEP(pcb_table_info.curr_pid);
	askfs(COMMAND_READ,0,0,fd,buf,size,0,0);
}
//eax=6
void k_write(void){

}
//eax=7
void k_close(fd){
	ACTIVE_SLEEP(pcb_table_info.curr_pid);
	askfs(COMMAND_CLOSE,0,0,fd,0,0,0,0);
}
//eax=8		attention:kernel-process need this syscall,for they live in ring1 and can not touch dr0~7
void k_watch(u32 addr,int write_only){
	debug_watch(addr,write_only);
	fire(pcb_table_info.curr_pid);
}
void k_seek(int fd,int offset,int whence){
	ACTIVE_SLEEP(pcb_table_info.curr_pid);
	askfs(COMMAND_SEEK,0,0,fd,0,0,offset,whence);
}

















