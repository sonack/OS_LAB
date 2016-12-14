! 显示一些愚蠢的信息 By Sonack @ 2016 - 12 - 14
! 定义全局标识符
.globl begtext, begdata, begbss, endtext, enddata, endbss
! 代码段
.text
！ 后面带冒号为标号
begtext:
！ 数据段
.data
begdata:
！ 未初始化数据段
.bss
begbss:
.text

BOOTSEG  = 0x07c0           ! original address of boot-sector bootsect的原始地址 是段地址
INITSEG  = 0x9000           ! we move boot here - out of the way 将bootsect移动到9000

！ 迫使链接程序在生成的执行程序中包含指定的标识符


entry _start

!移动到0x90000处
_start:
! #是立即数常量前缀
    mov ax,#BOOTSEG ！立即数不能直接送至段寄存器
    mov ds,ax       ! ds = 0x07c0
    mov ax,#INITSEG
    mov es,ax       ! es = 0x9000
    mov cx,#256     ! 设置移动计数值 = 256 字 = 512 字节
    sub si,si       ! 设置si = 0  源地址 ds:si
    sub di,di       ! 设置di = 0  目的地址 es:di
    rep             
    movw            ! 重复执行并递减cx，直到cx为0为止 movw即 movs指令
    jmpi    go,INITSEG  ! 段间跳转 INITSEG + go

! 当BIOS将引导扇区加载到0x7c00处并把执行权交给引导程序时，ss=0x00,sp=0xfffe

go: mov ax,cs       ! 将ds和es和ss都置成移动后代码段 即 0x9000
    mov ds,ax
    mov es,ax

! 打印一些愚蠢的信息，通过调用BIOS 0x10中断
! Print some inane message

    ! 读光标位置
    mov ah,#0x03    
    xor bh,bh       !光标位置保存在dx中，此处清除bh=0,即页号=0
    int 0x10        !调用中断
    
    mov cx,#52      !共显示52个字符
    mov bx,#0x0007      ! page 0, attribute 7 (normal)
    mov bp,#msg
    mov ax,#0x1301      ! write string, move cursor al=01 使用bl中的属性  ah=0x13 功能号
    int 0x10

sectors:
    .word 0
msg:
    .byte 13,10
    .ascii "Launch Failed... Going To Blow Up In 1 minute!!!"
    .byte 13,10

! 设置有效引导扇区标志，供BIOS中的程序加载引导扇区时识别使用，必须位于引导扇区的最后两个字节中。


.org 510
boot_flag:
    .word 0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
    




