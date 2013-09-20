#ifndef DISP_H
#define DISP_H
#define edi (*((int*)0x7e00))//多么安全

void k_screen_reset(void);
void k_show_chars(char*pt_head,unsigned end_flag);
void oprintf(char*format,...);
void k_scroll(void);
void k_checkbound(void);

//show_var模块，是printf函数的子模块，解析变量flag
void k_show_var(unsigned x,int val_type);
void show_asciis_buffer(void);
void write_asciis_buffer(unsigned x,unsigned val_type);
void init_asciis_buffer(void);
//end show_var
#endif
