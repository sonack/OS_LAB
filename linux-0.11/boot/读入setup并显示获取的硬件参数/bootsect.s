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
SETUPLEN = 4
SETUPSEG = 0x9020

entry _start
_start:
    mov ax,#BOOTSEG
    mov ds,ax      
    mov ax,#INITSEG
    mov es,ax       
    mov cx,#256    
    sub si,si       
    sub di,di       
    rep             
    movw            
    jmpi    go,INITSEG  

go: mov ax,cs    
    mov ds,ax
    mov es,ax

load_setup:
    mov dx,#0x0000     
    mov cx,#0x0002      
    mov bx,#0x0200     
    mov ax,#0x0200+SETUPLEN  
    int 0x13           
    jnc ok_load_setup      
    mov dx,#0x0000      
    mov ax,#0x0000      
    int 0x13
    j   load_setup     

ok_load_setup:
    mov ah,#0x03    
    xor bh,bh           
    int 0x10           
    
    mov cx,#57        
    mov bx,#0x0007     
    mov bp,#msg
    mov ax,#0x1301      
    int 0x10

    jmpi 0, SETUPSEG    

msg:
    .byte 13,10
    .ascii "Load Setup Sectors Success. Now Go To Setup Module."
    .byte 13,10

.org 510
boot_flag:
    .word 0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
