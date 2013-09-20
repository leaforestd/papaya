#新增的.o文件都要在.phony里注册，这样它才会被编译
cc=gcc
cflags=-I../include/ -Wall -fno-builtin -std=c99
asmflags=-I../include/
asm=nasm
all:sys_call.o utils.o asmtask.o

.PHONY:all

sys_call.o:sys_call.asm
	$(asm) -felf -o ../lib/sys_call.o sys_call.asm $(asmflags)

utils.o:utils.c ../include/utils.h
	$(cc) -c -o ../lib/utils.o utils.c $(cflags)

asmtask.o:asmtask.asm
	$(asm) -felf -o ../lib/asmtask.o asmtask.asm $(asmflags)

