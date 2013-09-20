#include"../include/utils.h"
#include<proc.h>
#include<disp.h>
//kmalloc
//原则是：节点数据结构存储的size都是纯粹的空闲数据块大小纯粹的used数据块大小,区别于real_xx(它包含了信息节点本身大小）
static EMPTY_BLOCK*block0=(EMPTY_BLOCK*)HEAP_BASE;
void heap_init(void){
	block0->size=HEAP_SIZE-sizeof(EMPTY_BLOCK);
}
void info_heap(void){
	int block_id=0;
	EMPTY_BLOCK*block=block0;
	while(block){
		oprintf("%u(%x~%x)--->",block_id++,(int)block,BLOCK_DATA_END(block));
		block=block->next;
	}
}
void*kmalloc(int byte){
	assert(byte+4>=sizeof(EMPTY_BLOCK));
	int real_byte=byte+4;
	EMPTY_BLOCK*block=block0;
	while(block&&block->size<real_byte) block=block->next;
	assert(block);
	int real_alloc=BLOCK_DATA_END(block)-real_byte+1;
	*(int*)real_alloc=byte;
	block->size-=real_byte;
	void*return_alloc=(void*)(real_alloc+4);
	memset((char*)return_alloc,0,byte);
//	oprintf("kmalloc return %x\n",return_alloc);
	return return_alloc;
}
void kfree(void*pt){
	int real_used_addr=(int)pt-4;
	int real_used_size=*(int*)(real_used_addr)+4;
//	oprintf("real_used_addr:%x,real_used_size:%x\n",real_used_addr,real_used_size);
	EMPTY_BLOCK*block=block0;
	while(block->next&&(int)block->next<real_used_addr) block=block->next;
	if(BLOCK_DATA_END(block)+1==real_used_addr){//右面是否跟empty_block接壤
		block->size+=real_used_size;
		//可能打通了左右的empty_block
		if(BLOCK_DATA_END(block)+1==(int)block->next){
			block->size+=block->next->size+sizeof(EMPTY_BLOCK);
			del_node(block->next);
		}
	}
	else{
		EMPTY_BLOCK*new=(EMPTY_BLOCK*)real_used_addr;
		new->size=real_used_size-sizeof(EMPTY_BLOCK);
		if((int)block->next==real_used_addr+real_used_size){//左面是否与empty_block接壤
			new->size+=(block->next->size+sizeof(EMPTY_BLOCK));//吞并左面的empty_block
			del_node(block->next);//删除左面的empty_block
		}
		EMPTY_BLOCK*next=block->next;
		new->prev=block;
		block->next=new;
		new->next=next;	
		if(next) next->prev=new;
	}
}
void del_node(EMPTY_BLOCK*block){
	EMPTY_BLOCK*prev=block->prev;
	assert(prev);
	prev->next=block->next;
	if(block->next) block->next->prev=prev;
}
void insert_after(EMPTY_BLOCK*mother,EMPTY_BLOCK*block){
	EMPTY_BLOCK*next=mother->next;

	block->prev=mother;
	mother->next=block;
	if(next){
		next->prev=block;
		block->next=next;
	}
}
//end kmalloc
void dump_sys(){
}

void assert_func(char*exp,char*file,char*base_file,int line){
//	dispStr("assert_failue:exp,file,base_file,line----",0x400);
	oprintf("assert failure>>>exp:%s,file:%s,base_file:%s,line:%u\n",exp,file,base_file,line);
	while(1);
}

int bit1_count(char*addr,int bytes){
	int count=0;
	for(int offset=0;offset<bytes;offset++){
//		oprintf("addr[offset]:%u\n",addr[offset]);
		for(int x=0;x<8;x++){
//				oprintf("1<<x:%u\n",1<<x);
			if(((1<<x)&addr[offset])!=0){
				count++;
			}	
		}
	}
	return count;
}

void memcpy(char*dest,char*src,int bytes){
	for(int i=0;i<bytes;i++){
		dest[i]=src[i];
	}
}

int strlen(char*str){
	int len=0;
	while(*str!=0){
		str++;
		len++;	
	}
	return len;
}

boolean strmatch(char*seg,char*whole){
	for(int i=0;i<strlen(seg);i++){
		if(seg[i]!=whole[i]) return false;
	}
	return true;
}

char*strcpy(char*dest,char*src){
	char*d=dest;
	while((*dest++=*src++));
	return d;
}








