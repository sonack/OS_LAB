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
    
    mov cx,#26        
    mov bx,#0x000c    
    mov bp,#msg
    mov ax,#0x1301      
    int 0x10

	! 读入光标位置 到 0x90000
	mov ax,#INITSEG
	mov ds,ax
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov [0],dx

	! 读入内存大小 到 0x90002	(扩展内存)
	mov ah,#0x88
	int 0x15
	mov [2],ax

	! 读入磁盘1的基本参数表 (0x41) 到 0x90004
	mov ax,#0x0000
	mov ds,ax
	lds si,[4*0x41]	! 高位存放在ds中，低位存放在si中
	mov ax,#INITSEG
	mov es,ax
	mov di,#0x0004
	mov cx,#0x10
	rep 
	movsb		! 拷贝字节

	! 恢复ds\es寄存器
	mov ax,#INITSEG
	mov ds,ax
	mov ax,#SETUPSEG
	mov es,ax

	! ---------------------------
	! 显示光标位置信息				|
	! ---------------------------

	call print_nl
	! 显示提示字符串
	mov cx,#23
	mov bp,#cursor_position
	call show_prompt
	! 显示实际信息
	mov bx,[0]
	call print_hex
	call print_nl
	
	! ---------------------------
	! 显示内存大小信息				|
	! ---------------------------
	call print_nl
	! 显示提示字符串
	mov cx,#19
	mov bp,#mem_size
	call show_prompt
	! 显示实际信息
	mov bx,[2]
	call print_hex
	mov cx,#2
	mov bp,#KB
	call show_prompt
	call print_nl

	! ---------------------------
	! 显示HD1参数信息				|
	! ---------------------------
	
	! --------- 柱面 ----------- |

	call print_nl
	! 显示提示字符串
	mov cx,#20
	mov bp,#cyl
	call show_prompt
	! 显示实际信息
	mov bx,[0x0004]
	call print_hex
	call print_nl

	! --------- 磁头 ----------- |
	call print_nl
	! 显示提示字符串
	mov cx,#18
	mov bp,#head
	call show_prompt
	! 显示实际信息
	mov bx,[0x0004 + 0x0002]
	xor bh,bh	! 清空高位字节
	call print_hex
	call print_nl

	! --------- 扇区(sectors per track) ----------- |
	call print_nl
	! 显示提示字符串
	mov cx,#18
	mov bp,#sect
	call show_prompt
	! 显示实际信息
	mov bx,[0x0004 + 0x000e]
	xor bh,bh
	call print_hex
	call print_nl

	! Say GoodBye~
	
	call print_nl

	mov cx,#32
	mov bp,#delimiter
	call show_prompt
	call print_nl

	mov cx,#31
	mov bp,#over1
	call show_prompt

	call print_nl

	mov cx,#13
	mov bp,#over2
	call show_prompt

	call print_nl

	mov cx,#32
	mov bp,#delimiter
	call show_prompt
	call print_nl

	! 死循环
	dead_loop:
		jmp dead_loop




	! ---------------------------
	! 工具函数					|
	! ---------------------------


	! 显示提示信息
	! 参数:	cx 字符串长度
	! 		bp 字符串基址
	show_prompt:
		push ax 	! protect regs
		push bx

		push cx
		mov ah,#0x03    ! read cursor position
	    xor bh,bh           
	    int 0x10
	    pop cx         
	     
	    mov bx,#0x0007  ! 属性
	    mov ax,#0x1301  ! 使用bl属性
	    int 0x10

	    pop bx
	    pop ax
	    ret

	! 以16进制方式打印bx
	print_hex:
		mov cx,#4
		mov dx,bx	
		print_digit:
			rol dx,#4	! 循环左移4位，使高4位bit到低4位bit
			mov ax,#0xe0f	! ah = 0x0e, 功能号 	al = 0x0f 取低半字节掩码
			and al,dl	! 取dl的3~0 bit
			add al,#0x30 	! 给数字加上0x30
			cmp al,#0x3a	! 判断字母
			jl outp		! 数字直接完成
			add al,#0x07 	! 处理字母A~F
		outp:
			int 0x10
			loop print_digit	! cx = 4 
		ret

	! 打印CRLF
	print_nl:
		mov ax,#0xe0d	! CR
		int 0x10
		mov al,#0xa 	! LF
		int 0x10
		ret

msg:
	.byte 13,10
    .ascii "Now We Are In SETUP..."
    .byte 13,10

! 提示信息

! 光标位置
cursor_position:
	.ascii "The Cursor Position is "

! 内存大小
mem_size:
	.ascii "The Memory Size is "

! 磁盘参数
! 柱面数
cyl:
	.ascii "Cylinders Number is "
! 磁头数
head:
	.ascii "Headers Number is "
! 扇区数
sect:
	.ascii "Sectors Number is "
! 单位KB
KB:
	.ascii "KB"
over1:
	.ascii "Print Hardware Parameters Over!"
over2:
	.ascii "Lab1 is Over!"
delimiter:
	.ascii "--------------------------------"
.text
endtext:
.data
enddata:
.bss
endbss:
