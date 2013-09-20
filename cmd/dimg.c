/**1 you must compile it in debug mod as assert() contaion valid code
 * 2 you can only run dimg under 'cmd','src','bin'..
 **/
#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<errno.h>
int main(void){
//	int fd_sys=open("/home/wws/lab/asm1/cmd/400m.img",1,0);
	int fd_sys=open("../cmd/400m.img",1,0);
//	int fd_boot=open("/home/wws/lab/asm1/bin/boot.bin",0,0);
	int fd_boot=open("../bin/boot.bin",0,0);
	assert(fd_boot!=-1);
	if(fd_sys==-1){
		printf("open 400m.img error:%d",errno);
		return;
	}
	char mbr[446];
	assert(read(fd_boot,mbr,446)==446);
	assert(write(fd_sys,mbr,446)==446);
	assert(close(fd_boot)!=-1);
	assert(close(fd_sys)!=-1);

	system("dd if=../bin/kernel.elf of=../cmd/400m.img bs=512 conv=notrunc seek=1");
	printf("dimg done..");
}
