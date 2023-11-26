*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
* a0= buffer
* d0= TimeCode
* d3= SmpteMode
*-----------------------------------------------------------------------
	XDEF	_FillTimeCodeBuffer
_FillTimeCodeBuffer:

	movem.l	d1/d2/d3,-(a7)
******** Do Frames ********
	move.l	d0,d1
	and.l	#$000000FF,d1
	divu	#10,d1

	moveq.l	#0,d2
	move.w	d1,d2
	lsl.w	#8,d2
	swap	d1
	or.w	d1,d2

	tst.l	d3
	bne.s	NotDF
	or.w	#$0400,d2

NotDF:
	move	#15,d3
	jsr	Shift_n_Store
******** Do Seconds ********
	move.l	d0,d1
	and.l	#$0000FF00,d1
	lsr.l	#8,d1
	divu	#10,d1

	moveq.l	#0,d2
	move.w	d1,d2
	lsl.w	#8,d2
	swap	d1
	or.w	d1,d2

	move	#15,d3
	jsr	Shift_n_Store
******** Do Minutes ********
	move.l	d0,d1
	and.l	#$00FF0000,d1
	swap	d1
	divu	#10,d1

	moveq.l	#0,d2
	move.w	d1,d2	
	lsl.w	#8,d2
	swap	d1
	or.w	d1,d2

	move	#15,d3
	jsr	Shift_n_Store
******** Do Hours ********
	move.l	d0,d1
	and.l	#$FF000000,d1
	swap	d1
	lsr.l	#8,d1
	divu	#10,d1

	moveq.l	#0,d2
	move.w	d1,d2	
	lsl.w	#8,d2
	swap	d1
	or.w	d1,d2

	moveq.l	#15,d3
	jsr	Shift_n_Store
******** Do SyncWord ********
	move.l	#$BFFC,d2		* 1011 1111 1111 1100
	moveq.l	#15,d3
	jsr	Shift_n_Store
*************
	movem.l	(a7)+,d1/d2/d3
	rts
*-----------------------------------------------------------------------
Shift_n_Store:
** IN: d3 = Number of bits-1
** IN: d2 = Data

SNSLoop:
*************
	lsr	#1,d2
*************
	bcs	IsAOne
	bsr	BitOff
	bra	DoneWithIt
*************
IsAOne:
	bsr	BitOn
*************
DoneWithIt:
	dbra	d3,SNSLoop
*************
	rts
*-----------------------------------------------------------------------
BitOn:
	neg.b	STATE
	move.b	STATE,(a0)+
	neg.b	STATE
	move.b	STATE,(a0)+
	rts
*-----------------------------------------------------------------------
BitOff:
	neg.b	STATE
	move.b	STATE,(a0)+
	move.b	STATE,(a0)+
	rts
*-----------------------------------------------------------------------
	csect data
*-----------------------------------------------------------------------
STATE:	dc.b	$7f
*-----------------------------------------------------------------------
	END
