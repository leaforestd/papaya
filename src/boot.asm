;1 single segment should be smaller than 64KB
;old 2 the LOAD-TYPE segment of kernel.elf should be smaller than (0x80000-0x30400=)300KB...for kernel-cache-room starts at 0x80000 and kernel-entry locates at 0x30400
;old 3 kernel-cache-room should be never merged...because debug-module will use .stab&.strstab section
;4 extend 13h can only load 120*2 sectors for temprory(i don't know why),so kernel-size is limited within 120KB
%include "../include/pm.inc"
org 0x7c00
base_kernel_loaded equ  0x8000
base_entrance_kernel equ 0x30400
mbrHead:jmp entrance
[section .gdt]
gdt:Descriptor 0,0,0
.desc_plain_c0:Descriptor 0,0fffffh,DA_32+DA_C+DA_LIMIT_4K
.desc_plain_d0:Descriptor 0,0fffffh,DA_DRW|DA_LIMIT_4K+DA_32
len_gdt equ $-gdt
selector_plain_c0 equ gdt.desc_plain_c0-gdt
selector_plain_d0 equ gdt.desc_plain_d0-gdt
gdtPtr:
	dw len_gdt-1
	dd gdt;this indicates that gdt must locate at mbr
dap:
	db 16		;packetsize ==16
	db 0		;reserved ==0
	dw 120*2		;sector count,=120KB
	dw 0		;offset
	dw 0x8000	;seg
	dd 1		;start-sector
	dd 0
entrance:
mov ax,0
mov ds,ax

mov ah,42h
mov dl,80h
mov si,dap
int 13h
jnc read_ok
read_error:

jmp $
read_ok:
resetKernel:
;load kernel from 0x8000h
mov ax,base_kernel_loaded
mov ds,ax
mov bx,0
mov  dx,[bx+42];e_phentsz
mov  cx,[bx+44];e_phnum
mov  bx,[bx+28];ERR e_phoff should be 32-bit,here ignore high-16 bit
search_ph_typeLoaded:
	cmp dword [bx],1
	jne doNothing	
cySegment:
	push cx
	mov cx,[bx+16]
	mov si,[bx+4]	;segment offset,ds=base_kernel_loaded				 ERR ignore high-16 bit	
	mov eax,[bx+8]
	mov di,ax
	and di,1111b	;get bit 0~3
	shr eax,4
	mov es,ax
	
	cld
	rep movsb 	;ERR kernel should be smaller than 64kb,for we use cx as loop-seed
	pop cx			
	
doNothing:
	add bx,dx
loop search_ph_typeLoaded

;load gdt
mov ax,0
mov ds,ax
lgdt [gdtPtr]
;switch ds point to [.data] in kernel.asm

cli
openA20:
	in al,92h
	or al,00000010b
	out 92h,al

switch_proMode:
	mov eax,cr0
	or eax,1
	mov cr0,eax	

mov ax,selector_plain_d0
mov ds,ax
jmp dword selector_plain_c0:base_entrance_kernel
db 0xaa,0xbb,0xcc,0xff
times 510-($-$$)-4 db 0
dw 0xaa55
















