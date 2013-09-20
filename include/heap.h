#ifndef HEAP_H
#define HEAP_H
#define CELL_FREE 0
#define CELL_USED 1
#define BASE_HEAP (0x400000+4096)//heap start at 4MB+4KB
#define SIZE_HEAP 0x1000000//heapSize 16 MB-SIZE_NODE_FREE
#define SIZE_HEADER_HEAPCELL (sizeof(HEADER_HEAPCELL))
typedef struct header_heapCell{
	struct header_heapCell* pt_prev;
	struct header_heapCell* pt_next;
	unsigned int size;	
	unsigned int type;
}HEADER_HEAPCELL;

int initHeap();
void* omalloc(unsigned  num4);
int ofree(void* pt);
#endif
