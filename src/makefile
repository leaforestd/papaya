cc=gcc
cflags=-I../include/ -I./usr/include/ -Wall -fno-builtin -g -O0
asmflags=-I../include/ -I./usr/include/
#$(usro)
usro= usr/lib/asmtask.o usr/lib/sys_call.o usr/lib/utils.o
kernelRelyO= ../lib/kernel.o ../lib/kernel_c.o  ../lib/proc_asm.o ../lib/utils.o ../lib/proc.o ../lib/kbd_drv.o ../lib/utils_asm.o   ../lib/func_table.o $(usro) ../lib/disp_c.o ../lib/video_drv.o ../lib/ku_utils.o ../lib/hs.o  ../lib/fs_ext.o ../lib/struinfo.o  ../lib/mm.o ../lib/fs.o ../lib/tty.o ../lib/debug.o

asm=nasm
asmHeader=../include/*.inc
ccHeader=../include/*.h
all:../bin/boot.bin ../bin/kernel.elf

.PHONY:all

../bin/kernel.elf:$(kernelRelyO) 
	ld  -s  -Ttext 0x30400  -o ../bin/kernel.elf $(kernelRelyO);\
	ld -Ttext 0x30400 -o ../bin/g_kernel.elf $(kernelRelyO)

../lib/mm.o:mm.c ../include/mm.h 
	$(cc) -c -o ../lib/mm.o  mm.c $(cflags) -std=c99

../lib/struinfo.o:struinfo.c ../include/struinfo.h
	$(cc) -c -o ../lib/struinfo.o struinfo.c $(cflags) -std=c99

../lib/fs.o:fs.c ../include/fs.h
	$(cc) -c -o ../lib/fs.o fs.c $(cflags) -std=c99  -Wno-return-type

../lib/fs_ext.o:fs_ext.c  ../include/fs_ext.h
	$(cc) -c -o ../lib/fs_ext.o fs_ext.c $(cflags) -std=c99 -Wno-unused-variable

../lib/hs.o:hs.c ../include/hs.h
	$(cc) -c -o ../lib/hs.o hs.c $(cflags) -std=c99 

../lib/ku_utils.o:ku_utils.c ../include/ku_utils.h ../include/valType.h
	$(cc) -c -o ../lib/ku_utils.o ku_utils.c $(cflags) -std=c99

../lib/video_drv.o:video_drv.c ../include/video_drv.h
	$(cc) -c -o ../lib/video_drv.o video_drv.c $(cflags)

../lib/disp_c.o:disp.c ../include/disp.h
	$(cc) -c -o ../lib/disp_c.o disp.c $(cflags) -std=c99

../lib/func_table.o:func_table.c
	$(cc)  -c -o ../lib/func_table.o func_table.c $(cflags)

../lib/tty.o:tty.c ../include/tty.h ../include/kbd_drv.h ../include/proc.h ../include/elf.h ../include/ku_utils.h ../include/utils.h
	$(cc)  -c -o ../lib/tty.o tty.c $(cflags) -std=c99 -Wno-unused-variable

../lib/utils_asm.o:utils.asm 
	$(asm) -f elf -o ../lib/utils_asm.o utils.asm  $(asmflags) 

../lib/kbd_drv.o:kbd_drv.c ../include/kbd_drv.h ../include/utils.h  ../include/proc.h
	$(cc)  -c -o ../lib/kbd_drv.o kbd_drv.c -std=c99 $(cflags)

../lib/utils.o:utils.c ../include/utils.h
	$(cc)  -c -o ../lib/utils.o utils.c $(cflags) -std=c99

../lib/kernel_c.o:kernel.c  ../include/proc.h
	$(cc)  -c -o ../lib/kernel_c.o kernel.c  $(cflags) -std=c99

../lib/kernel.o:kernel.asm $(asmHeader)
	$(asm)  -f elf -o ../lib/kernel.o kernel.asm $(asmflags) 

../lib/proc_asm.o:proc.asm  ../include/utils.inc
	$(asm)  -f elf -o ../lib/proc_asm.o proc.asm $(asmflags)

../lib/proc.o:proc.c ../include/proc.h ../include/utils.h ../include/kbd_drv.h ../include/mm.h
	$(cc)  -c -o ../lib/proc.o proc.c  -std=c99 $(cflags)

../bin/boot.bin:boot.asm $(asmHeader)
	$(asm)  -o ../bin/boot.bin boot.asm   $(asmflags)
clean:
	
