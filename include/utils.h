#ifndef UTILS_H
#define UTILS_H
#include<ku_utils.h>
#include<mm.h>
#include<valType.h>
/**the following two macros aim at beautify code.
 * and,can you write a 'break_say'?
 */
#define return_say(msg)		do{oprintf("%s",msg);return;} while(0)
#define returnx_say(x,msg)	do{oprintf("%s",msg);return x;} while(0)
void detect_cpu(void);//send cpu information to specified area
extern void dispAX();
extern void dispEAX();
extern void dispStr(char const*const pt,unsigned int ahMod);
extern void dispInt(unsigned int);
extern void dispStrn(char const*const pt,unsigned int ahMod);
extern unsigned char in_byte(int port);
u32 in_dw(int port);
extern void out_byte(int port,unsigned value);
void out_dw(int port,u32 value);
extern int ring,path_ring0,ienter,stack_position,crack_eip;
void assert_func(char*exp,char*file,char*base_file,int line);
void port_read(unsigned port,void*buf,unsigned byte);
void port_write(unsigned port,void*buf,unsigned byte);
void send_hd_eoi(void);
int bt(u8*addr,int bits_scope);//bits_scope msut >=32 and %32=0,namely 32,64,128...
int bt0(u8*addr,int bits_scope);//bits_scope msut >=32 and %32=0,namely 32,64,128...
void bs(u8*addr,int offset);
int br(u8*addr,int offset);//搞清楚，bc不是bit clear，是bit complement,br caishi 
int bit1_count(char*addr,int bytes);
void memcpy(char*dest,char*src,int bytes);
int strlen(char*str);
char*strcpy(char*dest,char*src);
#define assert(exp)\
do{if(!(exp)) assert_func(#exp,__FILE__,__BASE_FILE__,__LINE__);} while(0);

#define DSI(str,i)\
dispStr(str,0x400);\
dispInt(i);

//留着以后测吧
#define POINTER_SHIFT(pt,type,len) (type*)((u32)pt+len) 

//for kmalloc
typedef struct empty_blockk{
	struct empty_blockk*prev;
	struct empty_blockk*next;
	int size;
}EMPTY_BLOCK;
#define BLOCK_DATA_END(block) ((int)((char*)block+sizeof(EMPTY_BLOCK)+block->size-1))
void*kmalloc(int byte);
void kfree(void*pt);
void heap_init(void);
void del_node(EMPTY_BLOCK*block);
void insert_after(EMPTY_BLOCK*mother,EMPTY_BLOCK*block);
boolean strmatch(char*seg,char*whole);
void info_heap(void);
#endif
