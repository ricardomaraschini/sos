[BITS 32]
global start
start:
	cli
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
