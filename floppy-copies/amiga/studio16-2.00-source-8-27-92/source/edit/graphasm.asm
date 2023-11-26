

	SECTION "graphasm",code
	
	xdef	_graphasm


*
* Duplicates the following 'C' code
* 
*	maxval=0;
*	for (i=0; i < ei->gb_scale; i++) {
*		if (abs(buf[index+i]) > maxval)
*			maxval=abs(buf[index+i]);
*		}
*	maxval=maxval/256;
*	if (maxval > 127) maxval=127;

_graphasm	; a0 - buf.l
				; d1 - index.l
				; d2 - ei->gb_scale.l
				; d0 - maxvalue.l

		move.l	d3,-(sp)	; save working reg
		moveq.l	#0,d0		; maxval = 0
		subq.l	#1,d2		; ei->gb_scale -1
		lsl.l		#1,d1		; *2 (index is in words)
		adda.l	d1,a0		; buf=buf+index

lp:	move.w	(a0)+,d3		;temp=buf[index++]
		bpl.s		pos			; positive
		addq.w	#1,d3			; -128 must be 127
		neg.w		d3				; temp = -temp
pos:	cmp.w		d3,d0			; temp < max?
		bcc.s		skip	
		move.w	d3,d0			; new max
skip	dbra		d2,lp			;ei->gb_scale--;
		lsr.l		#8,d0			;maxval/256
		move.l	(sp)+,d3
		rts

		END
