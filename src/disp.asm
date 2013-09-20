;nolonger protect eax,ebx,...
;require ds=selector_room_plain
global dispMem4;void dispMem4(unsigned*addr,int len);
global dispEAX
global dispAX
global dispStr
global dispInt
global dispStrn
extern base_room_info32
extern selector_room_plain
extern selector_room_info32
extern selector_video
[section .text]
dispMem4:
push ebp
mov ebp,esp

mov ebx,[esp+8];ebx=addr
mov ecx,[esp+12];ecx=len
lea ebx,[ebx+ecx*4-4];point ebx to highest-addr

dispDword:
push ebx
push ecx
mov eax,[ebx]
call dispEAX
pop ecx
pop ebx

sub ebx,4;point to next dword
loop dispDword

mov esp,ebp
pop ebp
ret

;unplain seg-base used,so this function should be called under ring0 to assure ds,fs.. able to switch descriptor
dispStr:
cld
push ebp
mov ebp,esp
add ebp,8;now bp point to the first arg passed
push ds
push es
push fs
push edi
push esi
push ecx
mov esi,[ss:ebp]
add ebp,4
mov ax,selector_room_plain
mov fs,ax;fs:edi acts pointer to string 
mov ax,selector_room_info32
mov ds,ax
mov edi,[ds:0] ;recorver text-pointer
mov ax,selector_video
mov es,ax;set gs point to seg_video,es:edi act text-pointer
mov eax,[ss:ebp];send arg-ahMod to ah
.trans_string_vm:
	mov al,[fs:esi]
	cmp al,0
	je meetEOF
	inc esi
	stosw
	jmp .trans_string_vm
meetEOF:
	mov ax,di 
	shr ax,1 ;ax store wordth edi point,edi itself store the byteth pointed
	mov cl,80
	div cl
	inc al 
	cmp al,23
	ja .scrollU_align_bottom;scroll up to align bottom if the line pointer locates cross 23
.meetEOFAhead:
	mov [ds:0],edi
	pop ecx
	pop esi
	pop edi
	pop fs
	pop es 
	pop ds
	pop ebp
	ret
.scrollU_align_bottom:
	sub al,23
	mov bh,80
	mul bh;now,ax stores words that pointer shall be backed
	mov bx,ax
	shl bx,1;now,ax stores bytes that pointer shall be backed
	
	sub di,bx;ERR if edi point beyond 2^16,this crack
	push edi 
	push ds
	mov ax,selector_video
	mov ds,ax
	mov esi,0;edi will not be used behind
	add si,bx
	mov edi,0
	cld
	mov ecx,0
	mov cx,bx
	shr ecx,1;aim to use movsw
	rep movsw
	
	pop ds
	pop edi
	jmp .meetEOFAhead


dispInt:
mov eax,[ss:esp+4]
call dispEAX 
ret 

dispAX:
push es
push edi
push ecx
push eax

;init es:edi at base_room_info32+4
mov edi,4;format 11112222h,a 32-bit register cost 8 bytes for descriping,and 4 bit for others
mov ax,selector_room_info32
mov es,ax
mov al,'0'
stosb
mov al,'x' ;the value shall end with a space
stosb

mov ecx,4
.trans_al_char:
	pop eax
	rol ax,4
	push eax
	and al,0fh
	cmp al,9
	jna .plus48
.plus7:
	add al,7
.plus48:
	add al,48
	stosb
	loop .trans_al_char		

	mov al,' '
	stosb
	mov al,0
	stosb
	push 0x200
	push base_room_info32+4
	call dispStr
	add esp,8
pop eax
pop ecx
pop edi
pop es
ret

dispStrn:
cld
push ebp
mov ebp,esp
add ebp,8;now bp point to the first arg passed
push ds
push es
push fs
push edi
push esi
push ecx
mov esi,[ss:ebp]
add ebp,4
mov ax,selector_room_plain
mov fs,ax;fs:edi acts pointer to string 
mov ax,selector_room_info32
mov ds,ax
mov edi,[ds:0] ;recorver text-pointer
mov ax,selector_video
mov es,ax;set gs point to seg_video,es:edi act text-pointer
mov eax,[ss:ebp];send arg-ahMod to ah
.trans_string_vm:
	mov al,[fs:esi]
	cmp al,0
	je .enterPointer;start enterPointer if the string output finished
	inc esi
	stosw
	jmp .trans_string_vm
.enterPointer:
	mov ax,di 
	shr ax,1 ;ax store wordth edi point,edi itself store the byteth pointed
	mov cl,80
	div cl
	push eax
	mov cl,79
	sub cl,ah 
	inc cl
	and ecx,000000ffh	
	mov ah,00000000b
	mov al,' '
	cld
	.trans_space_vm:
		stosw
		loop .trans_space_vm
		jmp .meetEOF
;enterPointer_END	
.meetEOF:
	pop eax
	inc al
	cmp al,23
	ja .scrollU_align_bottom;scroll up to align bottom if the line pointer locates cross 23
.meetEOFAhead:
	mov [ds:0],edi
	pop ecx
	pop esi
	pop edi
	pop fs
	pop es 
	pop ds
	pop ebp
	ret
.scrollU_align_bottom:
	sub al,23
	mov bl,ah 
	mov bh,80
	mul bh;now,ax stores words that pointer shall be backed
	mov bx,ax
	shl bx,1;now,ax stores bytes that pointer shall be backed
	
	sub di,bx;ERR if edi point beyond 2^16,this crack
	push edi 
	push ds
	mov ax,selector_video
	mov ds,ax
	mov esi,0;edi will not be used behind
	add si,bx
	mov edi,0
	cld
	mov ecx,0
	mov cx,bx
	shr ecx,1;aim to use movsw
	rep movsw
	
	pop ds
	pop edi
	jmp .meetEOFAhead

	
dispEAX:
push es
push edi
push ecx
push eax

;init es:edi at base_room_info32+4
mov edi,4;format 11112222h,a 32-bit register cost 8 bytes for descriping,and 4 bit for others
mov ax,selector_room_info32
mov es,ax
mov al,'0'
stosb
mov al,'x' ;the value shall end with a space
stosb

mov ecx,8 ;ATTEN dispEAX.asm differ from dispAX.asm just at here,this 8,with that 4
.trans_al_char:
	pop eax
	rol eax,4
	push eax
	and al,0fh
	cmp al,9
	jna .plus48
.plus7:
	add al,7
.plus48:
	add al,48
	stosb
	loop .trans_al_char		

	mov al,' '
	stosb
	mov al,0
	stosb
	push 0x200
	push base_room_info32+4
	call dispStr
	add esp,8
pop eax
pop ecx
pop edi
pop es
ret




					

	
	





