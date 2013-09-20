#include<kbd_drv.h>
#include<utils.h>
#include<proc.h>
/* Keymap for US MF-2 keyboard. */
unsigned char keymap[NR_SCAN_CODES * MAP_COLS]   = {
	/* scan-code !Shift Shift E0 XX */
	/* ==================================================================== */
	/* 0x00 - none */ 0, 0, 0,
	/* 0x01 - ESC */ ESC, ESC, 0,
	/* 0x02 - '1' */ '1', '!', 0,
	/* 0x03 - '2' */ '2', '@', 0,
	/* 0x04 - '3' */ '3', '#', 0,
	/* 0x05 - '4' */ '4', '$', 0,
	/* 0x06 - '5' */ '5', '%', 0,
	/* 0x07 - '6' */ '6', '^', 0,
	/* 0x08 - '7' */ '7', '&', 0,
	/* 0x09 - '8' */ '8', '*', 0,
	/* 0x0A - '9' */ '9', '(', 0,
	/* 0x0B - '0' */ '0', ')', 0,
	/* 0x0C - '-' */ '-', '_', 0,
	/* 0x0D - '=' */ '=', '+', 0,
	/* 0x0E - BS */ BACKSPACE, BACKSPACE, 0,
	/* 0x0F - TAB */ TAB, TAB, 0,
	/* 0x10 - 'q' */ 'q', 'Q', 0,
	/* 0x11 - 'w' */ 'w', 'W', 0,
	/* 0x12 - 'e' */ 'e', 'E', 0,
	/* 0x13 - 'r' */ 'r', 'R', 0,
	/* 0x14 - 't' */ 't', 'T', 0,
	/* 0x15 - 'y' */ 'y', 'Y', 0,
	/* 0x16 - 'u' */ 'u', 'U', 130,
	/* 0x17 - 'i' */ 'i', 'I', 0,
	/* 0x18 - 'o' */ 'o', 'O', 0,
	/* 0x19 - 'p' */ 'p', 'P', 0,
	/* 0x1A - '[' */ '[', '{', 0,
	/* 0x1B - ']' */ ']', '}', 0,
	/* 0x1C - CR/LF */ ENTER, ENTER, 0,
	/* 0x1D - l. Ctrl */ CTRL_L, CTRL_L, CTRL_R,
	/* 0x1E - 'a' */ 'a', 'A', 0,
	/* 0x1F - 's' */ 's', 'S', 0,
	/* 0x20 - 'd' */ 'd', 'D', 131,
	/* 0x21 - 'f' */ 'f', 'F', 0,
	/* 0x22 - 'g' */ 'g', 'G', 0,
	/* 0x23 - 'h' */ 'h', 'H', 0,
	/* 0x24 - 'j' */ 'j', 'J', 0,
	/* 0x25 - 'k' */ 'k', 'K', 0,
	/* 0x26 - 'l' */ 'l', 'L', 128,
	/* 0x27 - ';' */ ';', ':', 0,
	/* 0x28 - '\'' */ '\'', '"', 0,
	/* 0x29 - '`' */ '`', '~', 0,
	/* 0x2A - l. SHIFT */ SHIFT_L, SHIFT_L, 0,
	/* 0x2B - '\' */ '\\', '|', 0,
	/* 0x2C - 'z' */ 'z', 'Z', 0,
	/* 0x2D - 'x' */ 'x', 'X', 0,
	/* 0x2E - 'c' */ 'c', 'C', 129,
	/* 0x2F - 'v' */ 'v', 'V', 0,
	/* 0x30 - 'b' */ 'b', 'B', 0,
	/* 0x31 - 'n' */ 'n', 'N', 0,
	/* 0x32 - 'm' */ 'm', 'M', 0,
	/* 0x33 - ',' */ ',', '<', 0,
	/* 0x34 - '.' */ '.', '>', 0,
	/* 0x35 - '/' 		*/	'/',		'?',		0,
	/* 0x36 - r. SHIFT	*/	SHIFT_R,	SHIFT_R,	0,
	/* 0x37 - '*'		*/	'*',		'*',    	0,
	/* 0x38 - ALT		*/	ALT_L,		ALT_L,  	ALT_R,
	/* 0x39 - ' '		*/	' ',		' ',		0,
};

static int ctrl_down=0;
static int shift_down=0;
void  key_handler(void){
	assert(!(ctrl_down&&shift_down))
	int key_code=in_byte(0x60);//禁止ctrl，shift鍵同时按下
//		dispInt(key_code);
	//若接收到ctrl,shift鍵释放
	if(key_code>=NR_SCAN_CODES){
		(key_code==BC_CTRL_L)?ctrl_down=0:0;
		(key_code==BC_SHIFT_L||key_code==BC_SHIFT_R)?shift_down=0:0;
	}
	//若接收到ctrl,shift鍵按下	
	else if(key_code==MC_CTRL_L){
		ctrl_down=1;
	}
	else if(key_code==MC_SHIFT_L||key_code==MC_SHIFT_R){
		//assert(0)
		shift_down=1;
	}
	//处理正常ascii鍵
	else{
//		dispStr("normal_key_code:",0x400);
		//dispInt(key_code);
	//	assert(0);
		unsigned ascii=keymap[key_code*MAP_COLS+shift_down*1+ctrl_down*2];//根据状态字节调整ascii
		//dispStr("ascii in drv:",0x400);
//		dispInt(ascii);
		for(int pid=0;pid<MAX_TASK;pid++){
			if(pcb_table[pid].mod==TASKMOD_SLEEP&&pcb_table[pid].msg_type==MSGTYPE_CHAR){
				SLEEP_ACTIVE(pid);
				obuffer_push(&pcb_table[pid].obuffer,ascii);
			}
		}
	}
//	dispStr("end..key_code",0x400);
//		dispInt(key_code);
		//dispStrn("",0x400);
	proc_dispatch();//暂时先这样，看处理速度根的上就好
}
