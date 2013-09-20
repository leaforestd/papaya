tss_esp0_offset equ 4
%macro iodelay 0
nop
nop
nop
nop
%endmacro

%macro save 0
	sub esp,4
	pushad
	push ds
	push es
	push fs
	push gs
	mov  dword [outofproc],1
%endmacro

%macro TSS1 6
;link ss0 esp0 ss1 esp1 ss2 esp2
%%tss_start:
	dd 0
	dd %2
	dd %1
	dd %4
	dd %3
	dd %6
	dd %5
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
;	dw 0
;	dw $-%%tss_start
;	db 0ffh
%endmacro
































