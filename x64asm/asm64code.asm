; -----------------------------------------------------------------------------------------------------------	
; Code copyright Elias Bachaalany <lallousz-x86@yahoo.com>
; You may freely use this code. Please give credits where due.
; http://lallouslab.net


; -----------------------------------------------------------------------------------------------------------	
PUBLIC hello_world_asm
PUBLIC access_extern_data
PUBLIC add_by_ref
PUBLIC init_struct
PUBLIC cause_av_bad
PUBLIC cause_av_good
PUBLIC cause_av_good2

; -----------------------------------------------------------------------------------------------------------	
MyStruct struct
b  db ?
buf  dq ?
fptr  dq ?
sig dq ?
MyStruct ends

; -----------------------------------------------------------------------------------------------------------	
; Data segment
; -----------------------------------------------------------------------------------------------------------	
_DATA SEGMENT
	hello_msg db "Hello world from x64 ASM", 0
	info_msg  db "Info", 0
	buf dw 2048 DUP ('eb')
_DATA ENDS

; -----------------------------------------------------------------------------------------------------------	
; Text segment
; -----------------------------------------------------------------------------------------------------------	
_TEXT	SEGMENT

; -----------------------------------------------------------------------------------------------------------	
EXTERN MessageBoxA: PROC
EXTERN GetForegroundWindow: PROC

EXTERN c_ext_byte: byte
EXTERN c_ext_word: word
EXTERN c_ext_dword: dword
EXTERN c_ext_qword: qword
EXTERN c_ext_ptr: qword
EXTERN c_ext_my_function: PROC


; -----------------------------------------------------------------------------------------------------------	
; AV generating function
cause_av_bad PROC
	push rbp
	mov rbp, rsp

	push 1
	push 2
	push 3
	sub rsp, 0400h

	xor rax, rax
	mov rax, [rax] ; cause A/V

	add rsp, (8 * 3) + 0400h
	mov rsp, rbp
	pop rbp

	ret
cause_av_bad ENDP

; -----------------------------------------------------------------------------------------------------------	
; 
cause_av_good PROC FRAME
	push rbp ; save previous frame pointer
	.pushreg rbp ; encode unwind info
	mov rbp, rsp ; set new frame pointer
	.setframe rbp, 0 ; encode frame pointer
	.endprolog

	; feel free to modify the stack now
	push 1
	push 2
	push 3
	sub rsp, 0400h

	xor rax, rax
	mov rax, [rax] ; cause AV

	add rsp, (8 * 3) + 0400h
	mov rsp, rbp
	pop rbp

	ret
cause_av_good ENDP

; -----------------------------------------------------------------------------------------------------------	
cause_av_good2 PROC FRAME
	sub rsp, 020h ; allocate stack space
	.allocstack 020h ; encode that change
	push rbp ; save old frame pointer
	.pushreg rbp ; encode stack operation
	mov rbp, rsp ; set new frame pointer
	.setframe rbp, 0 ; encode frame pointer
	.endprolog

	; any stack pointer modifications here on are okay...
	sub rsp, 080h

	; we can unwind from the following AV because of the frame pointer   
	xor rax, rax
	mov rax, [rax] ; cause AV

	; properly restore the stack pointer (in case exception did not happen or the handler corrected the situation)
	add rsp, (080h + 020h)

	pop rbp
	ret
cause_av_good2 ENDP

; -----------------------------------------------------------------------------------------------------------	
; Private non public function. It is exposed by 'init_struct' to the external world.
non_public_func PROC
	ret
non_public_func ENDP

; -----------------------------------------------------------------------------------------------------------	
; void init_struct(MyStruct *st);
init_struct PROC
	; st->b = 1
	mov byte ptr MyStruct.b[rcx], 1

	; st->buf = &buf
	mov rax, offset buf
	mov qword ptr MyStruct.buf[rcx], rax
	
	; st->fptr = fptr
	lea rax, non_public_func
	mov qword ptr MyStruct.fptr[rcx], rax
	
	; st->sig = value
	mov rax, 0badbeefbabeh
	mov qword ptr MyStruct.sig[rcx], rax

	ret
init_struct ENDP

; -----------------------------------------------------------------------------------------------------------	
access_extern_data PROC
	; Dereference all the data according to each data's sizes
	mov	al, byte ptr [c_ext_byte]
	mov ax, word ptr [c_ext_word]
	mov eax, dword ptr [c_ext_dword]
	mov rax, qword ptr [c_ext_qword]
	
	; Remember, a pointer is just a QWORD
	mov rax, qword ptr [c_ext_ptr]

	; Similarily, a function pointer is also a QWORD
	mov rax, offset c_ext_my_function
	sub rsp, 4 * 8 ; Register shadow stack
	call rax ; call the C function
	add rsp, 4 * 8 ; Restore the stack

	ret
access_extern_data ENDP

; -----------------------------------------------------------------------------------------------------------	
hello_world_asm PROC

	push rbp ; save frame pointer
	mov rbp, rsp ; fix stack pointer
	sub rsp, 8 * (4 + 2) ; allocate shadow register area + 2 QWORDs for stack alignment

	; Get a window handle
	call GetForegroundWindow
	mov rcx, rax

	; WINUSERAPI int WINAPI MessageBoxA(
	;  RCX =>  _In_opt_ HWND hWnd,
	;  RDX =>  _In_opt_ LPCSTR lpText,
	;  R8  =>  _In_opt_ LPCSTR lpCaption,
	;  R9  =>  _In_ UINT uType);

	mov rdx, offset hello_msg
	mov r8, offset info_msg
	mov r9, 0 ; MB_OK

	and rsp, not 8 ; align stack to 16 bytes prior to API call
	;sub rsp, 8 ; mess up the alignment
	call MessageBoxA

	; epilog. restore stack pointer
	mov rsp, rbp
	pop rbp

	ret	
hello_world_asm ENDP

; -----------------------------------------------------------------------------------------------------------	
; __int64 add_by_ref(int a, int b, __int64 *r)
add_by_ref PROC
	
	movsxd rax, ecx
	movsxd rdx, edx
	add	rax, rdx ; result in RAX
	mov qword ptr [r8], rax ; store / deref
	
	ret	
		
add_by_ref ENDP

_TEXT	ENDS

END