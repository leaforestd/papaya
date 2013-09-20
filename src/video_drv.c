#include<video_drv.h>
#include<utils.h>
#include<valType.h>
void set_cursor(unsigned pos){
	out_byte(CRTC_ADDR_REG,CURSOR_L);
	out_byte(CRTC_DATA_REG,pos&0xff);
	out_byte(CRTC_ADDR_REG,CURSOR_H);
	out_byte(CRTC_DATA_REG,pos>>8);
}

void set_start(u32 pos){
	out_byte(CRTC_ADDR_REG,START_ADDR_L);
	out_byte(CRTC_DATA_REG,pos&0xff);
	out_byte(CRTC_ADDR_REG,START_ADDR_H);
	out_byte(CRTC_DATA_REG,pos>>8);
}

int get_start(void){
	int pos=0;
	out_byte(CRTC_ADDR_REG,START_ADDR_L);
	pos+=in_byte(CRTC_DATA_REG);
	out_byte(CRTC_ADDR_REG,START_ADDR_H);
	pos+=in_byte(CRTC_DATA_REG)<<8;
	return pos;
}
