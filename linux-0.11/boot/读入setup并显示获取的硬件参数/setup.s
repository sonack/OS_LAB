.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

BOOTSEG  = 0x07c0
INITSEG  = 0x9000
SETUPSEG = 0x9020

entry _start
_start:
	mov ax,cs
    mov ds,ax
    mov es,ax

     mov ah,#0x03    
    xor bh,bh           
    int 0x10           
    
    mov cx,#28        
    mov bx,#0x000c    
    mov bp,#msg
    mov ax,#0x1301      
    int 0x10
msg:
	.byte 13,10
    .ascii "Now We Are In SETUP..."
    .byte 13,10
.text
endtext:
.data
enddata:
.bss
endbss:
