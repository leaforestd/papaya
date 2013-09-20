#ifndef TTY_H
#define TTY_H
#include<valType.h>
int global_text_ppg;
void reset_cmd_asciis();
int getchar(void);
void parse_cmd_asciis();//一个粗略的命令解析函数
void write_cmd_asciis(unsigned ascii);
void show_dir(char*buf);
#endif
