#ifndef HS_H
#define HS_H
#include<utils.h>
#include<hd_drv.h>
#include<valType.h>
#define SIZE_SECTOR 512

#define OFFSET2 (0x0)
#define REG_DATA (0x1f0-OFFSET2)

#define REG_ERROR (0X1F1-OSFSET2)
#define REG_FEATURES (0X1F1-OFFSET2)

#define REG_COUNT (0X1F2-OFFSET2)
#define REG_LBA_LOW (0X1F3-OFFSET2)
#define REG_LBA_MID (0X1F4-OFFSET2)
#define REG_LBA_HIGH (0x1F5-OFFSET2)
#define REG_DEVICE (0X1F6-OFFSET2)

#define REG_STATUS (0x1F7-OFFSET2)
#define REG_COMMAND (0x1F7-OFFSET2)

#define REG_CONTROL (0X3F6-OFFSET2)

#define MASK_BSY 0x80

#define COMMAND_IDENTIFY 0xec
#define COMMAND_NULL 0x0
#define COMMAND_READ 0x20
#define COMMAND_WRITE 0x30
#define COMMAND_CHECK 0X90
#define COMMAND_OPEN 0x1
#define COMMAND_CLOSE 0x2
#define COMMAND_SEEK 0x3
#define STATUS_BSY (in_byte(REG_STATUS)&MASK_BSY)

#define MAKE_REG_DEVICE(lba,drv,lba_highest) ((lba<<6)|(drv<<4)|(lba_highest)|0XA0)

#define SECTOR_THROUGHPUT 256
typedef struct{
	union{
		struct{
			u8 low;
			u8 mid;
			u8 high;
			u8 padden;
		}lba_stru;
		u32 lba;
	};
	u8 features;
	u32 reg_count;//不被用户进程制定，而由hs自动计算出来,1~256,当它是256时，hs_cmd_out函数会向REG_COUNT寄存器写入0
	u8 device;
	u8 command;
	//1,buf并不会传递给硬盘驱动器的端口，而是给hs进程用，所以HD_CMD这个名字，应该理解为外部传递给hs的工作参数，而非hs传给硬盘驱动器的工作参数
	//2,改成HS_CMD，意思就好了
	char*buf;
	u32 count;//record the sector-num usr wants to r/w,it will be decreased when a local-loop of r/w finished,and hs will sleep again when count=0

	char*pt_curr;//不被用户进程制定，由hs内部使用
	int asker;
}HS_CMD;


void hs_cmd_out(HS_CMD*cmd);
void hs_cmd_init(u32 lba,u32 count,u8 command,char*buf);
void hs(void);
void identify_info(void);
void cmd_info(void);
#endif
