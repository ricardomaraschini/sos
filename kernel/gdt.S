[GLOBAL gdt_flush]

; gdt entries
; 0x00 - our dummy gdt entry
; 0x08 - our gdt for data segment
; 0x10 - our gdt for code segment
gdt_flush:
	mov eax, [esp+4]
	lgdt [eax]

	mov ax,0x10 ; offset to our gtd data segment
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	jmp 0x08:.flush ; far jump to code segment(0x08)
	                ; far jump implicitly changes CS as well
.flush:
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
