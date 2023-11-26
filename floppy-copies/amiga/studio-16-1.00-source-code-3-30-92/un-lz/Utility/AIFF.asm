*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
	XDEF	_DoubleToExtend
_DoubleToExtend:
	movem.l d2/d3/d4,-(sp)
	movem.l 16(sp),a0/a1
	movem.l	(a0),d0/d1
	move.l	d0,d2
	swap	d2
	move.l	d2,d3
	and.w	#$8000,d2
	and.w	#$7fff,d3
	lsr.w	#4,d3
	add.w	#15360,d3
	or.w	d3,d2
	lsl.l	#8,d0
	lsl.l	#3,d0
	rol.l	#8,d1
	rol.l	#3,d1
	move.l	d0,d3
	move.l	d1,d0
	and.w	#$7ff,d0
	or.w	d0,d3
	move.l	d1,d4
	and.l	#$fffff800,d4
	or.l	#$80000000,d3
	move.w	d2,(a1)+
	movem.l	d3/d4,(a1)
	movem.l	(sp)+,d2/d3/d4
	rts
*-----------------------------------------------------------------------
	XDEF	_ExtendToDouble
_ExtendToDouble:
	movem.l	d2/d3/d4,-(sp)
	movem.l	16(sp),a0/a1
	move.w	(a1)+,d2
	movem.l	(a1),d0/d1
	move.w	d2,d3
	and.w	#$8000,d2
	and.w	#$7fff,d3
	sub.w	#15360,d3
	lsl.w	#4,d3
	or.w	d2,d3
	swap	d3
	clr.w	d3			/* UNTESTED */
	ror.l	#3,d0
	ror.l	#8,d0
	lsr.l	#3,d1
	lsr.l	#8,d1
	move.l	d1,d4
	move.l	d0,d1
	and.l	#$000FFFFF,d0
	or.l	d0,d3
	and.l	#$FFF00000,d1
	or.l	d1,d4
	movem.l	d3/d4,(a0)
	movem.l	(sp)+,d2/d3/d4
	rts
	end
