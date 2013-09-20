#include<debug.h>
#include<utils.h>
#include<disp.h>
#define KERNEL_CACHE_BASE 0x80000
#define k_eheader ((Elf32_Ehdr*)KERNEL_CACHE_BASE)
static Elf32_Shdr* k_sheader;
static char*shstrtab;
static NLIST*stab;
static int stabidx;
static char*stabstr;
void init_debug(void){
	assert(*(int*)k_eheader=0x464c457f);
	k_sheader=(Elf32_Shdr*)(KERNEL_CACHE_BASE+k_eheader->e_shoff);	
	shstrtab=(char*)(KERNEL_CACHE_BASE+k_sheader[k_eheader->e_shstrndx].sh_offset);	
	//locate .stab .stabstr
	/** 	if you want to use stab in kernel,two bug have to be fixed:
	 * 		1,extend int13h can not load more than 120*2 sectors
	 * 		2,choose a suitable cache_room for kernel.elf,0x80000~0xa0000 is two crowd
	int i;
	for(i=0;i<k_eheader->e_shnum;i++){
		if(strcmp(shstrtab+k_sheader[i].sh_name,".stab")){
			stab=(NLIST*)(KERNEL_CACHE_BASE+k_sheader[i].sh_offset);
			stabidx=i;
		}
		if(strcmp(shstrtab+k_sheader[i].sh_name,".stabstr")){
			stabstr=(char*)(KERNEL_CACHE_BASE+k_sheader[i].sh_offset);
			oprintf("stabidx:%u,stabstridx:%u\n",stabidx,i);
		}
	}
	*/
}
//terrible bochs!it seems not to be sensitive to dr0~dr7
void debug_watch(u32 addr,int write_only){
	int dr7_var=write_only?0x10001:0x30001;
	//active dr7,dr0	attention:L0~L3 of dr7 will be cleared when task-switch,but we don't use intel's tss,so it's safe
	__asm__ __volatile__(
		"movl %%dr7,%%eax\n\t"
		"or %1,%%eax\n\t"
		"movl %%eax,%%dr7\n\t"
		"movl %0,%%eax\n\t"
		"movl %%eax,%%dr0\n\t"
		:
		:"r"(addr),"r"(dr7_var)
		:
	);
}
void kernelsection(void){
	oprintf("kernelsection...");
	Elf32_Shdr*sheader=k_sheader;
	int i;
	for(i=0;i<k_eheader->e_shnum;i++){
		//only print SHF_ALLOC section,namely .text,.data,.bss etc
		if(sheader->sh_flags&SHF_ALLOC) oprintf("%s %x~%x\n",shstrtab+sheader->sh_name,sheader->sh_addr,sheader->sh_addr+sheader->sh_size);
		sheader++;	
	}
	/**
	for(i=0;i<k_sheader[stabidx].sh_size/sizeof(NLIST);i++){
		if(stab[i].n_type==N_SO) oprintf("%s ",stabstr+stab[i].n_strx);
		else if(stab[i].n_type==N_LINE) oprintf("%u ",stab[i].n_value);
		else if(stab[i].n_type==N_LBRAC) oprintf("{");
		else if(stab[i].n_type==N_RBRAC) oprintf("}");
		else oprintf(".");
	}
	*/
}

