
	SECTION "split",code
	
	xdef	_ByteToWord
	xdef	_WordToByte

; a0 - destination word array
; a1 - source char array
; d0 - length

_ByteToWord:
		move.l	d1,-(sp)	; Save the registers
		subq.l	#1,d0
nextbyte:
		move.b	(a1)+,d1
		asl.w		#8,d1
		move.w	d1,(a0)+
		dbra		d0,nextbyte
		move.l	(sp)+,d1	; restore the registers.
		rts

; a0 - destination char array
; a1 - source word array
; d0 - length

_WordToByte:
		move.l	d1,-(sp)	; Save the registers
		subq.l	#1,d0
nextword:
		move.w	(a1)+,d1
		bpl.s		skp
		addq.w	#1,d1			/** +1 on neg nums makes asr **/
skp	asr.w		#8,d1			/** act like /2 instead of rndg wrong **/
		move.b	d1,(a0)+
		dbra		d0,nextword
		move.l	(sp)+,d1	; restore the registers.
		rts

		END
