[BITS 32]
global start
start:
	mov esp, _sys_stack     ; This points the stack to our new stack area
	jmp stublet

; This part MUST be 4byte aligned, so we solve that issue using 'ALIGN 4'
ALIGN 4
mboot:
	; Multiboot macros to make a few lines later more readable
	MULTIBOOT_PAGE_ALIGN	equ 1<<0
	MULTIBOOT_MEMORY_INFO	equ 1<<1
	MULTIBOOT_AOUT_KLUDGE	equ 1<<16
	MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
	MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
	MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	EXTERN code, bss, end

	; This is the GRUB Multiboot header. A boot signature
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

	; AOUT kludge - must be physical addresses. Make a note of these:
	; The linker script fills in the data for these ones!
	dd mboot
	dd code
	dd bss
	dd end
	dd start

; call kernel main()
stublet:
	extern main
	call main
	jmp $

global gdt_flush
extern gp

; gdt entries
; 0x00 - our dummy gdt entry
; 0x08 - our gdt for data segment
; 0x10 - our gdt for code segment
gdt_flush:
	lgdt [gp] ; gp is declared on main.c
	mov ax,0x10 ; offset to our gtd data segment
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	jmp 0x08:flush2 ; far jump to code segment(0x08)
	                ; far jump implicitly changes CS as well
flush2:
	ret

; why 0x08 ?
;
; each entry on gdt entries is 64 bits.
; first entry is at 0x0 offset and it's our dummy(all zeroed entry)
; second entry is our kernel code segment, so we could access it at
; 0x08
; as our kernel data segment is available at 3rd position, we could 
; access it at 0x10, or 16 in decimal
;
; ds must point to our data segment, that's why we load it with
; 0x10

; load interrupt handlers
global idt_load
extern idtp
idt_load:
	lidt [idtp]
	ret

; interrupt service routines

; macro for service requests that do not
; pushed a value to stack
; in order to mantain compatibility, we
; push zero before request number
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common_stub
%endmacro


; macro for service requests that already
; pushed a value to stack
%macro ISR_ERRCODE 1
global isr%1
isr%1:
	cli
	push byte %1
	jmp isr_common_stub
%endmacro


ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; isrs.c
extern fault_handler

isr_common_stub:
	pusha       ; pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	push ds
	push es

	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, fault_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret


%macro IRQ_NOERRORCODE 2
global irq_%1
irq_%1:
	cli
	push byte 0
	push byte %2
	jmp irq_common_stub
%endmacro

IRQ_NOERRORCODE 0,32
IRQ_NOERRORCODE 1,33
IRQ_NOERRORCODE 2,34
IRQ_NOERRORCODE 3,35
IRQ_NOERRORCODE 4,36
IRQ_NOERRORCODE 5,37
IRQ_NOERRORCODE 6,38
IRQ_NOERRORCODE 7,39
IRQ_NOERRORCODE 8,40
IRQ_NOERRORCODE 9,41
IRQ_NOERRORCODE 10,42
IRQ_NOERRORCODE 11,43
IRQ_NOERRORCODE 12,44
IRQ_NOERRORCODE 13,45
IRQ_NOERRORCODE 14,46
IRQ_NOERRORCODE 15,47

extern irq_handler

irq_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov eax,esp
	push eax
	mov eax, irq_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp,8
	iret


SECTION .bss
	resb 8192 ; reserves 8KBytes of memory here
_sys_stack:

