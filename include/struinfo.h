#ifndef STRUINFO_H
#define STRUINFO_H
void workon(char*p_stru,char**p_names,int*p_widths,int *p_lens,int members);
void struinfo(void);
//arg1:pointer to stru-instace,arg2:STRU_TYPE ,这个宏依照mw生成信息数组的命名规则，即membername_##STRU_NAME
#define  WORKON(INSTANCE,STRU_NAME)	workon((char*)INSTANCE,membername_##STRU_NAME,memberwidth_##STRU_NAME,memberlen_##STRU_NAME,sizeof(membername_##STRU_NAME)/4)
#endif
