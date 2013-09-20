#include<unistd.h>
#include<../../../include/ku_mm.h>
int getpid(void){
	return kinfo.curr_pid;
}
