.define __brk
.define __sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__sbrk:		move.l .limhp,a0
		add.l  4(sp),a0
		move.l #0x11,d0
		trap #0
		bcs Icerror
		move.l .limhp,d0
		move.l d0,a0
		add.l  4(sp),a0
		move.l a0,.limhp
		rts
Icerror:	jmp cerror
__brk:		move.l #0x11,d0
		move.l 4(sp),a0
		trap #0
		bcs Icerror
		move.l 4(sp),.limhp
		clr.l d0
		rts
