#include<disp.h>
#include<utils.h>
#include<sys_call.h>
#include<proc.h>
//ERR read big
#define VALUE(pt,width) (*(unsigned*)(pt)&(0xffffffff>>((4-(width))*8)))
#define printf oprintf
#define OUTPUT_DELAY 50
int id=0;
int*widths=0;
int*lens=0;
char**names=0;
char*stru=0;
int num_members=0;
int get_offset(int m_id);
void workon(char*p_stru,char**p_names,int*p_widths,int *p_lens,int members);
void struinfo(void);
unsigned read_member(int m_id);
void show_bigmember(int m_id);
//read_bigmember;

void struinfo(void){
//	printf("num_members:%d\n",num_members);
	printf("@struinfo:\n");
	for(int id=0;id<num_members;id++){
		sleep(MSGTYPE_TIMER,OUTPUT_DELAY);	
//		oprintf("id:%u,lens[id]:%u\n",id,lens[id]);
		if(lens[id]>1){
			show_bigmember(id);
			continue;
		}
		printf("%s: %x \n",names[id],read_member(id));
	}
	
}
void workon(char*p_stru,char**p_names,int*p_widths,int *p_lens,int members){
	num_members=members;
	id=0;
	stru=p_stru;
	names=p_names;
	widths=p_widths;
	lens=p_lens;
}

void show_bigmember(int m_id){
	assert(lens[m_id]>1)
	int width=widths[m_id];
	int len=lens[m_id];
	char*pt_cell=stru+get_offset(m_id);
	printf("%s:",names[m_id]);
	for(int i=0;i<len;i++){
		printf(" %x  ",VALUE(pt_cell,width));
		pt_cell+=width;
	}
	printf("\n");
}
unsigned read_member(int m_id){
	int offset=get_offset(m_id);
	return VALUE(stru+offset,widths[m_id]);
	/**
	switch(lens[m_id]){
		case 1:
			return *(unsigned char*)(stru+offset);
			break;
		case 2:
			return *(unsigned short*)(stru+offset);
			break;
		case 4:
			return *(unsigned *)(stru+offset);
			break;
		default:
			assert(0);
	}
	*/
}

int get_offset(int m_id){
	int x=0;
	for(int i=0;i<m_id;i++){
		x+=widths[i]*lens[i];		
	}
	return x;
}









