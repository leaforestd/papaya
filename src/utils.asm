%include "utils.inc"
global in_byte,out_byte,port_read,send_hd_eoi,port_write,bt,bs,bc,br,bt0,detect_cpu,in_dw,out_dw
extern cpu_string
global init8259A
global init8253
[section .text]
detect_cpu:;void detect_cpu(void);
	mov eax,0
	cpuid
	mov [cpu_string],ebx
	mov [cpu_string+4],edx
	mov [cpu_string+8],ecx
	ret
bs:;void bs(char*addr,int offset);
	mov ebx,[esp+4]
	mov eax,[esp+8]
	bts [ebx],eax
	ret 

br:;void btc(char*addr,int offset);
	mov ebx,[esp+4]
	mov eax,[esp+8]
	btr [ebx],eax
	mov eax,1
	jc .return
	mov eax,0
	.return:
	ret 

bt:;int bt(char*addr,int bits_scope);
	jmp .entrance
	.return:
	add eax,edx
	ret
	.entrance:
	mov ecx,[esp+8];ERR bits_scope must %8=0
	shr ecx,5;ecx/=32,convert bit-scope to byte scope
	mov edx,0
	mov ebx,[esp+4]
	.1:
	bsf eax,[ebx]
	jnz .return
	add ebx,4
	add edx,32
	loop .1
	;can not scan out a 1-bit
	mov edx,0
	mov eax,-1;is a special digit meaning can not find value-1-bit.
	jmp .return

bt0:;int bt0(char*addr,int bits_scope);
	jmp .entrance
	.return:
	add eax,edx
	ret
	.entrance:
	mov ecx,[esp+8];ERR bits_scope must %8=0
	shr ecx,5;ecx/=32,convert bit-scope to byte scope
	mov edx,0
	mov esi,[esp+4]
	.1:
	mov ebx,[esi]
	xor ebx,0xffffffff	
;	xor ebx,0
	bsf eax,ebx
	jnz .return
	add esi,4
	add edx,32
	loop .1
	;can not scan out a 1-bit
	mov edx,0
	mov eax,-1;is a special digit meaning can not find value-1-bit.
	jmp .return
port_read:;void port_read(unsigned port,void*buf,unsigned byte);
	mov edx,[esp+4]
	mov edi,[esp+8]
	mov ecx,[esp+12]
	shr ecx,1
	cld
	rep insw	
	ret
port_write:;void port_write(unsigned port,void*buf,unsigned byte);
	mov edx,[esp+4]
	mov esi,[esp+8]
	mov ecx,[esp+12]
	shr ecx,1
	cld
	rep outsw
	ret

in_byte: ;char in_byte(int port)
	xor eax,eax
	mov dx,[esp+4]
	in al,dx
	iodelay
	ret
in_dw: ;u32 in_dw(int port)
	mov dx,[esp+4]
	in eax,dx
	ret
out_byte:;void out_byte(int port,unsigned value)
	mov dx,[esp+4]
	mov al,[esp+8]
	out dx,al
	ret
out_dw: ;void out_dw(int port,u32 value)
	mov dx,[esp+4]
	mov eax,[esp+8]
	out dx,eax
	ret
;void init8259A(int mask);
send_hd_eoi:
	mov al,20h
	out 20h,al
	out 0xa0,al
	iodelay
	ret

init8259A:
	mov al,11h
	out 20h,al		;send icw1 to 0x20		arg meaning:[icw4 needed]
	iodelay;io_delay belong to the same segment,so just call label(namely jmp near ptr label) is ok.
	out 0a0h,al		;send icw1 to 0xa0
	iodelay

	mov al,20h
	out 21h,al		;send icw2 to 0x21.		arg meaning:[irq0=0x20]
	iodelay
	mov al,28h
	out 0a1h,al		;send icw2 to 0xa1.		arg meaning:[irq8=0x28]
	iodelay

	mov al,4
	out 21h,al		;send icw3 to 0x21		arg meaning:[link slave chip at ir2]
	iodelay
	mov al,2
	out 0a1h,al 	;send icw3 to 0xa1		arg meaning:[link master chip from ir2]
	iodelay

	mov al,1
	out 21h,al		;arg meaning[80X86 mod,normal EOI]
	iodelay
	out 0a1h,al
	iodelay
	;initial word port finished..

	;send ocw1,set interrupt mash register
	mov al,[esp+4];ERR by default,all irq-port was masked
	out 21h,al
	iodelay
	mov al,10111111b;		arg meaing:[AT hard-disk irq open]  
	out 0a1h,al
	iodelay
	ret

init8253:
	mov al,0x34
	out 0x43,al

	mov ax,[esp+4]
	out 0x40,al
	shr ax,8
	out 0x40,al
	ret












