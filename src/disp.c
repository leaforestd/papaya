//为内核提供基本的输出功能，更丰富的输出由tty进程实现
//有一部分函数被tty做系统调用,并不是核心的内核接口，只是避免代码重写
#include<ku_utils.h>
#include<video_drv.h>
#include<disp.h>
#include<utils.h>
#include<valType.h>
#define buffer_len 12
#define EDI_ENTER edi=(((edi/160)+1)*160);
//一个页的字节数
#define PAGE_SIZE (80*25*2)
//末尾的字节偏移
#define BOUND (PAGE_SIZE*2-2)
//1,k_show_chars函数的主体实现，这个代码块没有通用功能，只是让k_show_chars函数更好看一些
//2,目前是唯一的写屏接口，要具备upsend_page,滚屏，光标跟随，转义符识别功能
//3,先判别当前指向是否为控制字符，若不是，则简单输出到屏幕即可
//4,while每写入一个字符后，或回车,退格后，都要检查edi是否越界，并调整之
//5,普通字符会让edi增2,控制字符就复杂了。
//6,写屏函数要做两样事：按需要写显存；调整edi。至于边界检查，滚屏，光标跟随都是完善性的函数。
#define LOOP(exp)\
while(exp){\
	if(*pt_read=='\n'){\
		EDI_ENTER\
	}\
	else if(*pt_read=='\b'){\
		if(edi>=start_line*160+2){\
			edi-=2;\
			*(pt_video+edi)=0;\
		}\
	}\
	else{\
		*(pt_video+edi)=*pt_read;\
		edi+=2;\
	}\
	pt_read++;\
	k_checkbound();	\
}

//		*(pt_video+edi+1)=0x4;
static char* pt_video=(char*)0xb8000;
static char asciis_buffer[buffer_len];//存储数字分解后各位的ascii码
static int start_line=0;//当前是从第几行开始显示
static int width=-1;//开始必须<0  支持%4s这样的printf格式

//1,当前只支持[%]和[\\]标记,还有就是\0标记了
//2,\标记看来不必要，c编译器会帮助转义\r,\n,\s...
void oprintf(char*format,...){
	char*pt_curr=format;
	unsigned arg_id=0;
	int*pt_arg0=(int*)&format;//pt_arg0指向堆栈的第一个参数
	while(*pt_curr!=0){
		if(*pt_curr=='%'){
			arg_id++;
			pt_curr++;//跳到下一个字符（标记或宽度字符）
			//下一个字符是否是*？
			if(*pt_curr=='*'){
				width=*(pt_arg0+arg_id);//取出宽度变量参数
				arg_id++;
				pt_curr++;
				goto just_show_var;
			}
			//如果有宽度限制，读取宽度
			eat_dec(pt_curr,width);
			/**
			int two=-1;//boolen，宽度是两位数字吗？
			while(*pt_curr>='0'&&*pt_curr<='9'){
				pt_curr++;
				two++;
			}	
			if(two>=0){
				width=*(pt_curr-1)-48;
				if(two==1) width+=(*(pt_curr-2)-48)*10;
				if(two>1){
					assert(0)
				}
			}
			*/
		just_show_var:
			k_show_var(*(pt_arg0+arg_id),*pt_curr);//注意，pt_arg0在是最后一个入栈，所以参数寻址用pt_arg0+arg_id
			pt_curr++;//跳出当前的标志组合
		}
		else{
			k_show_chars(pt_curr,1);//显示下个标记前的所有字符
			while((*pt_curr!='%')&&(*pt_curr!=0)) pt_curr++;//同时，pt_curr也跳到下一个标记
		}
		//pt_curr现在处于字符串尾，或字符段首，或标记首
	}
}

void k_show_var(unsigned x,int val_type){
	switch(val_type){
		case 's'://val_type=string,just call k_show_chars
			k_show_chars((char*)x,0);//字符串指针是以unsigned传进来的，不妨
			break;
		default://case 'c','u','d','x',val_type=digit,use asciis_buffer to convert
			write_asciis_buffer(x,val_type);
			show_asciis_buffer();
			break;
	}
}



void k_scroll(void){
	int new_start_line=(edi/160)-25+1;
	if(new_start_line>start_line){
		set_start(new_start_line*80);
		start_line=new_start_line;
	}

}
//void k_scroll(void){
//	kk
//}
//该函数把edi纠正回规定显存范围，但指示显示页下面一行。要紧接着调用scroll
//这种函数不好没必要做成系统调用，因为它没有执行特权指令，完全可以链接给用户程序
void k_checkbound(void){
	if(edi>BOUND){
		memcp(pt_video,pt_video+PAGE_SIZE,PAGE_SIZE);//page-1复制到page-0
		set_start(0);//从page-0开始显示
		memsetw((u16*)(pt_video+PAGE_SIZE),PAGE_SIZE/2,0x200);
		edi-=PAGE_SIZE;//调整写入指针，上浮一个页的byte	,这样就会到规定显存内
		start_line=0;
	}
}
//1,if end_flag!=0,k_show_chars may crack for width-support
void k_show_chars(char*pt_head,u32 end_flag){//这个函数直接支持printf的%*s，即制定宽度,看width-->0
	char*pt_read=pt_head;

	int chars_len=strlen(pt_head);
	int padden=width-chars_len;
	switch(end_flag){
		case 0://普通\0结尾
			LOOP(((width--!=0)&&*pt_read!=0))					
			break;
		case 1:
			LOOP((width--!=0)&&(*pt_read!=0)&&(*pt_read!='%'))					
			break;
		default:
			assert(0);
	}
	if(padden>0) edi+=padden*2;
	
	//字符序列写入完毕，做滚屏处理
	k_scroll();
	//光标跟随edi
	set_cursor(edi/2);
	width=-1;
}

void init_asciis_buffer(void){
	for(int i=0;i<buffer_len;i++){
		asciis_buffer[i]=0;
	}
}

void show_asciis_buffer(void){
	int i=0;
	while(asciis_buffer[i]==0) i++;
	assert(i<buffer_len)
	//dispStr(asciis_buffer+i,0x400);
	k_show_chars(asciis_buffer+i,0);
}

void write_asciis_buffer(unsigned x,unsigned val_type){
	init_asciis_buffer();
	unsigned offset=buffer_len-1-1;
	unsigned temp=x;
	switch(val_type){
		case 'c':
			asciis_buffer[offset]=x;
			break;
		case 'u'://分解无符号整数成ascii码到asciis_buffer
			while(temp>9){
				asciis_buffer[offset]=temp%10+48;
				temp/=10;
				offset--;
			}
			asciis_buffer[offset]=temp+48;//写入最后一个ascii
			break;
		case 'x':
			while(temp>0xf){
				unsigned i=temp%16;
				asciis_buffer[offset]=i<=9?i+48:i+87;
				temp/=16;
				offset--;
			}
			//写入最后一个数字ascii和0X前缀
			asciis_buffer[offset]=temp<=9?temp+48:temp+87;
			asciis_buffer[offset-1]='X';
			asciis_buffer[offset-2]='0';
			break;
		default:
			assert(0)
			break;
	}
}

void k_screen_reset(void){
	memsetw((u16*)pt_video,PAGE_SIZE/2*2,0x700);
	set_start(0);
	set_cursor(0);
	edi=0;
}














