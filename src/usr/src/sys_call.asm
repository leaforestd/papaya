;这个模块的函数不难写，仅仅是几句传参操作的包装。
global dotchar,sleep,u_obuffer_shift,show_chars,show_var,open,read,write,close,watch,lseek
;不管调用int 80h时要传递几个参数，默认把eip下方的5个dword都写到对应的参数寄存器。
;这个操作用宏包装，缺点是体积大些(1~10kb),其实这个宏也可以写成一个函数
%macro int80h 1
	mov ebx,[esp+4]
	mov ecx,[esp+8]
	mov edx,[esp+12]
	mov esi,[esp+16]
	mov edi,[esp+20]
	mov eax,%1
	int 0x80;int 0x80之后直接返回，这样从ring0通过SET_PCB_EAX产生的返回值会顺利从函数出去
	ret
%endmacro

;也不一定所有的syscall函数封装都这个样子吧？
[section .text]
;eax=0 void dotcahr(unsigned ascii,int mod,int line,int col);
;dotchar:int80h 0

;eax=0 void show_chars(char*pt_head,u32 end_flag) ;
show_chars:int80h 0

;eax=1 void sleep(int msg_type,int msg_bind);
sleep:int80h 1

;eax=2 int u_obuffer_shift();
u_obuffer_shift: int80h 2

;eax=3 void show_var(unsigned var,unsigned var_type);
show_var:int80h 3
;eax=4 int open(char*path,int mod);
open:int80h 4
read:int80h 5
write:int80h 6
close:int80h 7
watch:int80h 8 ;void watch(u32 addr,int write_only);
lseek:int80h 9























