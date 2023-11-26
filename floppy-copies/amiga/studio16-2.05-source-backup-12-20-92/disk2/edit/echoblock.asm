
	SECTION "echoblock",code
	
	xdef	_EchoBlock
	xref	_circbuf

; a1 - data block to echo
; d0 - block to process length in samples
;
; a0 - Delay (circ) buffer 
; d3 - Circ buffer length, in bytes
;
; d1 - dlevel (fixed, Unity := 1<<14)
; d2 - feedback (fixed, Unity := 1<<14)

_EchoBlock
		movem.l	d0/d3-d7,-(sp)		; Save the registers
		subq.l	#1,d0
		move.l	_circbuf,d6		; Get Circ buffer index
nextsamp:
		move.w	0(a0,d6.l),d7	; get oldest entry
		move.w	d7,d4				; d4 is for for output calc
		muls		d2,d7				; scale X feedback
		bpl.s		skp1				; make asr==/2
		addq.l	#1,d7
skp1	asr.l		#8,d7				; scale fixed point
		asr.l		#6,d7				; scale fixed point
		move.w	(a1),d5			; get input buffer value
		add.w		d5,d7				; buffer + feedbacked value
		bvc		nov1				; if overflow, then staturate
		or.w		d5,d5				; determine overflow direction
		bmi		neg1
		move.w	#32767,d7
		bra		nov1
neg1	move.w	#-32768,d7
nov1	move.w	d7,0(a0,d6.l)	; write new value to buffer
		addq.l	#2,d6
		cmp.l		d3,d6
		bne		nowrap
		moveq.l	#0,d6
nowrap
		muls		d1,d4				; X DLevel
		bpl.s		skp2				; make asr==/2
		addq.l	#1,d4
skp2	asr.l		#8,d4				; scale fixed point
		asr.l		#6,d4				; scale fixed point
		add.w		d4,d5				; delay'd+input
		bvc		nov2				; if overflow, then staturate
		or.w		d4,d4				; determine overflow direction
		bmi		neg2
		move.w	#32767,d5
		bra		nov2
neg2	move.w	#-32768,d5
nov2	move.w	d5,(a1)+			; put new value back into buffer
		dbra		d0,nextsamp

		move.l	d6,_circbuf		; Get Circ buffer ptr.
		movem.l	(sp)+,d0/d3-d7	; restore the registers.
		rts

		END
