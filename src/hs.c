//hs是磁盘驱动进程，向上提供的读写接口以扇区为单位
//hs进程运行在ring1,papaye把ring1纳入内核，IOPL=1。  这样做是方便实现内核进程，如果内核进程运行在ring1,又需要改进进程调度。
//ring1之所以能调用ring0函数，并不是因为她链接了内核的函数，而是因为将来的内存管理会把她放到内核的地址空间。按照常规的jmp cs:eip，ring1的进程肯定是不能调用ring0函数，因为特权级不同。但函数调用只调整eip，这样就做到了
#include<hs.h>
#include<hd_drv.h>
#include<proc.h>
#include<func_table.h>
#include<disp.h>
#include<utils.h>
#include<sys_call.h>//ERR:这是一个ring3的头文件，直接包含在这里并不合适

//local var of identify_info,avoid ld-error
static char s[64];
static HS_CMD cmd;
void hs(void){
	__asm__("cli");//hs运行时候不响应中断，他会主动放弃时间片,内核进程受到信任
	oprintf("hs initing..\n");
	while(1){
		while(cmd.command==COMMAND_NULL){//,如果未收到命令，进程挂起，等待用户进程呼叫
//			oprintf("hs:cmd.command==COMMAND_NULL,sleep now,wait fs_ask..\n");
			sleep(MSGTYPE_FS_ASK,0);
		}
//		oprintf("@hs get a task-cmd\n");
		//用户指定的count可能大于256,将其拆解在256范围内
		u32 fraction=cmd.count%(SECTOR_THROUGHPUT);
		u32 reg_count=(fraction==0?SECTOR_THROUGHPUT:fraction);//reg_count 1~256
		cmd.reg_count=reg_count;
	//	__asm__("cli");//这儿必须关中断，hs向磁盘驱动器写端口之后，要确保休眠。如果不关中断，来不及休眠，磁盘中断可能就送上来了。
					   //而wake_hs函数是根据MSGTYPE_HD_DONE来定位hs进程的，其实这里可以直接找hs的名字，消息不是必须的。
		hs_cmd_out(&cmd);

		if(cmd.command==COMMAND_IDENTIFY){//hs起初发送的是identify请求
			sleep(MSGTYPE_HD_DONE,0);
			port_read(REG_DATA,cmd.buf,SIZE_SECTOR);
			identify_info();

			send_hd_eoi();
			in_byte(REG_STATUS);//activate the interrupt enable bit

			cmd.command=COMMAND_NULL;
			SLEEP_ACTIVE(cmd.asker);	//hs done..inform proc-fs,she is sleeping on MSGTYPE_HS_DONE			
		}
		else if(cmd.command==COMMAND_CHECK){
			assert(0)
		}
		else{
			while(STATUS_BSY) oprintf("hs:STATUS_REG BSY!\n");
//			assert(!STATUS_BSY);
//			oprintf("hs:now execute cmd\n");
//			cmd_info();
			while(cmd.reg_count>0){
				if(cmd.command==COMMAND_READ){
//					oprintf("hs:reading now..\n");
					sleep(MSGTYPE_HD_DONE,0);
//					oprintf("hs:waked by hd-irq\n");
					port_read(REG_DATA,cmd.pt_curr,SIZE_SECTOR);//break
					cmd.pt_curr+=SIZE_SECTOR;
					send_hd_eoi();
					in_byte(REG_STATUS);//activate the interrupt enable bit
					cmd.reg_count--;
				}
				else{//COMMAND_WRITE
					port_write(REG_DATA,cmd.pt_curr,SIZE_SECTOR);//break
					cmd.pt_curr+=SIZE_SECTOR;
					send_hd_eoi();
					in_byte(REG_STATUS);//activate the interrupt enable bit
					cmd.reg_count--;
					sleep(MSGTYPE_HD_DONE,0);//等待磁盘向磁缓读入一个扇区
				}
			}
			cmd.count-=reg_count;
			cmd.lba+=reg_count;
			if(cmd.count==0){//完成用户指定的扇区任务，进入空闲休眠
//				oprintf("hs:usr ask finished,pt_curr=%x\n",cmd.pt_curr);
				cmd.command=COMMAND_NULL;
				SLEEP_ACTIVE(cmd.asker);	//hs done..inform proc-fs,she is sleeping on MSGTYPE_HS_DONE			
//				oprintf("@hs already wake up cmd.asker:%u,now it's mod:%u\n",cmd.asker,pcb_table[cmd.asker].mod);
			}
			//没有完成用户制定的扇区任务，继续主循环
		}
	}
}

void cmd_info(void){
	char*cmd_str=0;
	switch(cmd.command){
		case COMMAND_READ:
			cmd_str="READ";
			break;
		case COMMAND_WRITE:
			cmd_str="WRITE";
			break;
		default:
			assert(0)
	}
	oprintf("cmd info>>>command:%s,lba:%u,count:%u\n",cmd_str,cmd.lba,cmd.count);
}
void identify_info(void){
	//printf hard disk num
	oprintf("hd num:%u  ",*(u8*)0x475);
	oprintf("hd(0) info>>>");
	u16*info=(u16*)cmd.buf;//看来磁盘参数序列是以字为单位的，书上说的偏移也是以字为单位
	oprintf("lba support:%s  ",(info[49]&0x0200)?"yes":"no");
	oprintf("48bit-lba support:%s  ",(info[83]&0x0400)?"yes":"no");
	oprintf("size:%uM  ",(((int)info[61]<<16)+info[60])*SIZE_SECTOR/0X100000);

	//printf serial-asciis,model-asciis 
	struct {
		int idx;//word idx
		int len;//byte len
		char*desc;
	}asciis[]={
	{10,20,"serial"},
	{27,40,"model"}
	};
	for(int i=0;i<sizeof(asciis)/sizeof(asciis[0]);i++){
		//convert special-word-format to normal asciis,and store in s[64].
		char*char_info=(char*)(info+asciis[i].idx);
		int word_id;
		for(word_id=0;word_id<asciis[i].len/2;word_id++){
			s[word_id*2]=char_info[word_id*2+1];
			s[word_id*2+1]=char_info[word_id*2];
		}
		s[word_id*2]=0;
		oprintf("%s:%s  ",asciis[i].desc,s);
	}
}

void hs_cmd_out(HS_CMD*cmd){
	while(STATUS_BSY) oprintf("@hs_cmd_out wait staus-reg clear BSY-bit\n");
	//activate the interrupt enable bit
	out_byte(REG_CONTROL,0);	

	out_byte(REG_LBA_LOW,cmd->lba_stru.low);
	out_byte(REG_LBA_MID,cmd->lba_stru.mid);
	out_byte(REG_LBA_HIGH,cmd->lba_stru.high);

	out_byte(REG_FEATURES,cmd->features);
	out_byte(REG_COUNT,cmd->reg_count==256?0:cmd->reg_count);
	out_byte(REG_DEVICE,cmd->device);
	//write 0x3f6 at tail,this will trigger hd_drive to work
	out_byte(REG_COMMAND,cmd->command);
}

void hs_cmd_init(u32 lba,u32 count,u8 command,char*buf){//这个函数对外开放，所以直接操作cmd，不用参数指定cmd，因为外界访问不到hs的cmd。
	assert(count!=0)
	cmd.features=0;
	cmd.device=MAKE_REG_DEVICE(1,0,0);//papaya的磁盘驱动，硬代码指定操作0磁盘，使用lba模式，且lba最高4位不用。
		
	cmd.lba=lba;//init 3 regs at one blow
	cmd.count=count;
	cmd.command=command;

	cmd.buf=buf;
	cmd.pt_curr=buf;
	cmd.asker=pcb_table_info.curr_pid;
}

void wake_hs(void){
//	oprintf("wake_hs...\n");
	int pid=0;
//	oprintf("pcb_table[1].msg_type:%u\n",pcb_table[1].msg_type);
	while(pcb_table[pid].msg_type!=MSGTYPE_HD_DONE){
		pid++;
	}
	assert(pid<100);
//	oprintf("got pid..");
	//got hs's pid
	SLEEP_ACTIVE(pid);
	proc_dispatch();
}



















