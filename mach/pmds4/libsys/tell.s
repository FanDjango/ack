
.define _tell
.extern _tell
_tell:		
			pea	1
			clrl.l	-(sp)
			mov.l	12(sp),-(sp)
			jsr	_lseek
			add	#12,sp
			rts
