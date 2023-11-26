* a1 points to interupt data

	XREF		_int_index
	XREF		_buffer
	XREF		_dd524_base
	XREF		_half_count

	XDEF		_DD524IntHandler

_DD524IntHandler:

	movem.l	d2/d3/a0/a1/a6,-(sp)			; save for graphics.lib bug
	move.l	_dd524_base,a0

	move.w	2(a0),d0
	btst.l	#3,d0								; > = half full?
	beq		exit
	move.l	_buffer,a1						; data to xmit
	move.l	_int_index,d1
	lea		0(a1,d1.l),a1
	move.l	#0,d2

halfway:
	move.b	(a1)+,(a0)			; put in fifo
	move.b	(a1)+,(a0)			; put in fifo
	move.w	#0,(a0)				; put in fifo
	move.w	#0,(a0)				; put in fifo
	addq.l	#2,d2					; bump index
	move.w	2(a0),d0
	btst.l	#3,d0					; > = half full?
	beq		halfway

	or.l		d2,_half_count
	add.l		#510,d2				; fix index
	add.l		d2,_int_index

	move.l	#254,d1				; number of words to xfer

xmit:
	move.b	(a1)+,(a0)			; put in fifo
	move.b	(a1)+,(a0)			; put in fifo
	move.w	#0,(a0)				; put in fifo
	move.w	#0,(a0)				; put in fifo
	dbra		d1,xmit

exit:
		movem.l	(sp)+,d2/d3/a0/a1/a6					; restore
		moveq	#0,d0
		rts

		END
