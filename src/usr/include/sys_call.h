#ifndef SYS_CALL_H
#define SYS_CALL_H
//void dotchar(unsigned ascii,int mod,int line,int col);
#include "valType.h"
void show_chars(char*pt_head,unsigned end_flag) ;
void sleep(int msg_type,int msg_bind);
int u_obuffer_shift();
void show_var(unsigned var,unsigned var_type);
void watch(u32 addr,int write_only);
#endif
