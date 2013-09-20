/**
  *why we need a fs layer?
  *1  it is a virtual layer of different filesystem,and it will solve common filesystem requests.
  *2  if needed,it will make the choice which filesystem to call
  *3  sometimes,it communicates with fs_xx-layer to finish one job,we take open() as example:fs creates a file-desc and do some initialization,
  *and let fs_xx-layer finish the remaining job.
  */
#include<fs.h>
#include<fs_ext.h>
#include<ku_utils.h>
#include<disp.h>
#include<proc.h>
#define MAX_CMD 10
static FS_COMMAND empty_cmds[MAX_CMD]; 
#define MAX_MOUNT_INFO 20
static MOUNT_INFO mountinfo[MAX_MOUNT_INFO];
/**
1,receive massive arguments,because it will be called by k_open,k_read,k_write,k_colse.
2,choose which filesystem  to call by parsing fd
3,if COMMAND_OPEN,argument 'fd' will be pre-set,as you see within body of if(command==COMMAND_OPEN),that 'fd=new fd'
*/
int askfs(int command,
		char* path,int flags,					/**open(char*path,int flag,int mod); */
		int fd,char*addr,int size,			/**write/read(int fd,char*addr,int size); */
		int offset,int whence							/**lseek(int fd,int offset,int whence)*/
		){
	FS_COMMAND*cmd=new_cmd();
	if(!cmd){
		oprintf("kernel error:@fs empty_cmds used out\n");
		SYSCALL_RET(-1,2);
	}
	if(command==COMMAND_CLOSE){
		fd_table[fd].device=DEVICE_NULL;
		SYSCALL_RET(0,0);
	}
	else if(command==COMMAND_SEEK){
		oprintf("@fs handle COMMAND_SEEK..\n");
		int newseek=-1;
		switch(whence){
			case 0:
				oprintf("case0 cmd.offset=%u\n",offset);
				newseek=offset;
				break;
			case 1:
				newseek=fd_table[fd].seek+offset;
				break;
			case 2:
				newseek=fd_table[fd].filesize-1+offset;
				break;
			default:
				;//if you pass a bad location-flag,'newseek' will not be touched and keeps being -1
		}
		if(newseek>=0){
			fd_table[fd].seek=newseek;
			oprintf("@fs_ext seek sucess,return now=%u",newseek);
		}
		SYSCALL_RET(newseek,0xff);
	}
	else if(command==COMMAND_OPEN){
		/**
		  *what does an 'open' do?		---it create a file-desc for target file.
		  *this job was finished by two steps:
		  *1,at fs-layer(namely fs.c),file-desc's member 'flags','device','partation','seek' are initialized.of course,fs-layer can not get inode.
		  *so she pass shortpath to fs_xx_layer(namely fs_ext.c and so on).
		  *2,at fs_xx_layer,'shortpath' will be used to get 'inode' of file-desc.thus,a good file-desc is created and open() is done.
		  */
		fd=new_fd();
		if(fd==-1){
			oprintf("kernel error:@fs file-desc used out\n");
			SYSCALL_RET(-1,2);
		}
		fd_table[fd].flags=flags;
		fd_table[fd].seek=0;
		int i;
		for(i=0;i<MAX_MOUNT_INFO;i++){
			oprintf("mountpoint:%s,device:%u,partation:%u\n",mountinfo[i].mountpoint,mountinfo[i].device,mountinfo[i].partation);
			if(!strmatch(mountinfo[i].mountpoint,path)) continue;
			fd_table[fd].device=mountinfo[i].device;
			fd_table[fd].partation=mountinfo[i].partation;
			/**pass shortpath to fs_xx,fs-layer can not get inode */
			cmd->shortpath=path+strlen(mountinfo[i].mountpoint);oprintf("mountpoint:%s,strlen:%u,pass shortpath:%s\n",mountinfo[i].mountpoint,strlen(mountinfo[i].mountpoint),cmd->shortpath);
			break;
		}
		if(i==MAX_MOUNT_INFO){
			oprintf("bad mountpoint of path '%s'\n",path);
			SYSCALL_RET(-1,2);
		}
	}
	else if(command==COMMAND_READ||command==COMMAND_WRITE){
		cmd->addr=addr;
		cmd->size=size;
	}
	else	assert(0);
	/**cmd's common part init */
	cmd->fd=fd;
	cmd->asker=pcb_table_info.curr_pid;
	cmd->command=command;
	
	FILE_DESC*pfd=fd_table+fd;	
	switch(g_dp[pfd->device][pfd->partation].sys_id){
		case SYSID_LINUX:
			cmd->handler_pid=FS_EXT_PID;
			if(pcb_table[FS_EXT_PID].mod==TASKMOD_SLEEP&&pcb_table[FS_EXT_PID].msg_type==MSGTYPE_USR_ASK) SLEEP_ACTIVE(FS_EXT_PID);
			proc_dispatch();
			break;
		case SYSID_FAT16:
			break;
		case SYSID_FAT32:
			break;
		case SYSID_NTFS:
			break;
		case SYSID_EXTEND:
			oprintf("entend partation is not partation..\n");
			return 0;
		default:
			oprintf("askfs:unknown partation partition sys_id...\n");
			return 0;
	}
}
//bug:empty_cmds should be all 0 at first,but it proved to not
void init_fs(void){
	for(int i=0;i<MAX_CMD;i++) empty_cmds[i].command=COMMAND_NULL;
}
int new_fd(void){
	int i;
	for(i=0;i<MAX_FD;i++){
		//device==0 indicates a empty-file_desc
		if(fd_table[i].device==DEVICE_NULL) break;
	}
	if(i>=MAX_FD) return -1;
	return i;
}
void releasefd(int fd){
	fd_table[fd].device=DEVICE_NULL;
}
//function should be called within filesystem
FS_COMMAND*is_there_cmd_wait(void){
	int i;
	for(i=0;i<MAX_CMD;i++){
		if(empty_cmds[i].handler_pid==pcb_table_info.curr_pid&&empty_cmds[i].command!=COMMAND_NULL) break;
	}
	if(i==MAX_CMD) return 0;
	return &empty_cmds[i];
}

boolean mount(char*mountpoint,short device,short partation){
	if(device<1||device>4||partation<1||partation>15) return false;
	int i;
	for(i=0;i<MAX_MOUNT_INFO;i++){
		if(mountinfo[i].device!=DEVICE_NULL) continue;
		strcpy(mountinfo[i].mountpoint,mountpoint);
		mountinfo[i].device=device;
		mountinfo[i].partation=partation;
		break;
	}
	if(i==MAX_MOUNT_INFO) return false;

	int tail_offset=strlen(mountpoint)-1;
	if(mountpoint[tail_offset]=='/'){
		mountinfo[i].mountpoint[tail_offset]=0;
		oprintf("warning:mountpoint should not end with a '/',trip it,and it changes into '%s'\n",mountinfo[i].mountpoint);
	}
	return true;
}

FS_COMMAND* new_cmd(void){
	int i;
	for(i=0;i<MAX_CMD;i++)	if(empty_cmds[i].command==COMMAND_NULL) break;
	if(i==MAX_CMD)	return 0;
	return empty_cmds+i;
}











