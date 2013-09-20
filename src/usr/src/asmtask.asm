global idle,p1,p2
p0_addr equ 0xb8000
global p0_addr
[section .text]
;runing at ring0
idle:
;mdispStr 'idle hlt..',0x400
hlt
inc byte [p0_addr]
jmp idle

p1:
;inc byte[p0_addr+1*2];这一句不能注释，否则papaya的系统时间就不准了
;mov ebx,'t'
;mov ecx,00000010b
;mov edx,10
;mov esi,0
;mov eax,0
;int 0x80
jmp p1

p2:
inc byte[p0_addr+2*2]
inc eax
push 100
;call cold
add esp,4
jmp p2
