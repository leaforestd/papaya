#ifndef FS_H
#define FS_H
#include<hs.h>
#include<fs_ext.h>
#include<sys_call.h>

#define MOUNTPOINT_LEN 16
typedef struct{
	char mountpoint[MOUNTPOINT_LEN];
	short device;
	short partation;
}MOUNT_INFO;
typedef struct{
	union{
		int location;
		struct{
			short device;
			short partation;
		};
	};
	int inode;
	int flags;
	int seek;
	int filesize;
}FILE_DESC;
typedef struct{
	int command;
	int asker;
	int fd;
	char* shortpath;
	char* addr;
	int size;
	int handler_pid;
	int achieved;
}FS_COMMAND;
#define DPT_OFFSET (0x1be)
#define SYSID_EXTEND 0X5
#define SYSID_LINUX 0X83
#define SYSID_FAT16 0X6
#define SYSID_FAT32 1 
#define SYSID_NTFS 0x7
typedef struct{
	u8 boot;
	u8 start_head;
	u8 start_sector;
	u8 start_cylender;

	u8 sys_id;
	u8 end_head;
	u8 end_sector;
	u8 end_cylender;

	u32 start_lba;
	u32 count;
}DP;

char* sys_string[256];
FS_COMMAND*is_there_cmd_wait(void);
int askfs(int command,
		char* path,int flags,					/**open(char*path,int flag,int mod); */
		int fd,char*addr,int size,			/**write/read(int fd,char*addr,int size); */
		int offset,int whence				/**lseek(int fd,int seek,int whence)*/
		);

#define MAX_FD 30
FILE_DESC fd_table[MAX_FD];

/**global partation information of all devices*/
#define MAX_DEVICE 5
#define MAX_PARTATION 16
DP g_dp[MAX_DEVICE][MAX_PARTATION];

/**device number*/
#define DEVICE_NULL 0
#define DEVICE_HD 1
#define DEVICE_CD 2
#define DEVICE_RD 3
#define DEVICE_USB 4

void init_fs(void);
boolean mount(char*mountpoint,short device,short partation);
int new_fd(void);
FS_COMMAND* new_cmd(void);
void releasefd(int fd);
#endif
