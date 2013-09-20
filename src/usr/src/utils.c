#include "utils.h"
#include "sys_call.h"
#include "valType.h"
boolean loadfile(char*path,char*addr){
	int fd=open(path,0);
	if(fd==-1) return 0;
	int size=lseek(fd,0,2)+1;
	lseek(fd,0,0);
	int r_bytes=read(fd,addr,size);
	close(fd);
	return r_bytes;//if you read a empty file,function will return false	
}

void printff(char*format,...){
	char*pt_curr=format;
	unsigned arg_id=0;
	int*pt_arg0=(int*)&format;//pt_arg0指向堆栈的第一个参数
	while(*pt_curr!=0){
		if(*pt_curr=='%'){
			arg_id++;
			pt_curr++;//跳到标志字符
			show_var(*(pt_arg0+arg_id),*pt_curr);//注意，pt_arg0在是最后一个入栈，所以参数寻址用pt_arg0+arg_id
			pt_curr++;//跳出当前的标志组合
		}
		else{
			show_chars(pt_curr,1);//显示下个标记前的所有字符
			while((*pt_curr!='%')&&(*pt_curr!=0)) pt_curr++;//同时，pt_curr也跳到下一个标记
		}
		//pt_curr现在处于字符串尾，或字符段首，或标记首
	}
}

/**
int getchar(void){
	int ascii;
	while((ascii=u_obuffer_shift())==-1){//注意，ring3层的函数通过int80h进入ring0,想在rign0返回数值是要SET_PCB_EAX的
		sleep(MSGTYPE_CHAR,0);
	}
	return ascii;
}
*/
