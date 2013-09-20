;ERR assure already enter plain-mod
%include "utils.inc"
global fire_asm
extern selector_plain_c3
extern selector_plain_d3
extern tss
extern size_stackframe
;void fire_asm(int addr_pcb);

[section .text]
fire_asm:
mov esp,[esp+4];point esp to addr_pcb,and reset kernel stack
mov ebx,[size_stackframe]  
lea eax,[esp+ebx]
mov dword [tss+tss_esp0_offset],eax;register pcb.stackframe.bottom in tss.esp0
;be prepared for next clock-interrupt

;ready for jump to ring3 and go ahead,send an EOI now 发送了也没事儿，不iretd，8259A的irq上不来
mov al,20h
out 20h,al

pop gs
pop fs
pop es
pop ds
popad
add esp,4

iretd













