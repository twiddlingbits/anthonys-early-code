port		equ	8
buff		equ	12
end		equ	16
left		equ	20
chan		equ	24
s_start		equ	8
s_end		equ	12
*-----------------------------------------------------------------------

	csect text

	XDEF	_RecData
_RecData:
	movem.l	d1-d7/a1-a6,Reg		*Save Registers
	link	a1,#0
	movea.l	port(a1),a5
	movea.l	buff(a1),a6
	movea.l	end(a1),a3

	movea.l	a5,a4
	addq.l	#2,a5

waitR:
	btst.b	#1,(a4)
	beq.s	waitR

	move.w	(a5),d0
	and.w	#$FFF0,d0
	move.w	d0,(a6)+
	
	move.w	d0,$dff180

	cmp.l	a6,a3
	bne	waitR

	move.l	a3,d0

	unlk	a1
	movem.l	Reg,d1-d7/a1-a6		*Reset Registers
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_SendBlock
_SendBlock:
	movem.l	d1-d3/a1-a6,Reg		*Save Registers
	link	a1,#0
	movea.l	port(a1),a5
	movea.l	buff(a1),a6
	movea.l	end(a1),a3
	movea.l	left(a1),a2
	move.l	chan(a1),d3

	movea.l	a5,a4
	addq.l	#2,a5

loopP:
	addq.w	#1,$dff182

	move.w	(a6),d1
	addq.l	#2,a6
	and.w	#$FFF0,d1
	or.w	d3,d1

	subq.l	#1,(a2)
	beq	Xit

waitP:	btst.b	#0,(a4)			* wait  for write ok
	beq.s	waitP

	move.w	d1,(a5)			* send

	cmp.l	a6,a3
	bne	loopP

	move.l	a3,d0

	unlk	a1
	movem.l	Reg,d1-d3/a1-a6		*Reset Registers
	rts

Xit	move.l	#0,d0
	unlk	a1
	movem.l	Reg,d1-d3/a1-a6		*Reset Registers
	rts

*-----------------------------------------------------------------------

*-----------------------------------------------------------------------

	xdef	_DspXferInt

	xref	_SysBase
	xref	_Handler_sigbit
	xref	_tc_Handler

_DspXferInt:
	movea.l	(a1)+,a0
	btst.b	#2,(a0)
	beq.s	GoOd
	moveq	#0,d0
	rts

GoOd:
	move.w	d1,(a0)			* clear interupt
	adda.w	#2,a0
	move.w	(a0),d1			* read dsp command

	move.l	d2,SaveR

	move.w	(a1),d2		* read buffer pointer
	addq.w	#1,d2
	and.w	#$00FF,d2
	move.w	d2,(a1)+

	movea.l	(a1)+,a0
	lsl.w	#1,d2
	move.w	d1,$0(a0,d2.w)	* store dsp command

	move.w	d1,d2
	and.w	#$000f,d1
	cmp.w	#$000f,d1
	bne.s	NotBuff

	and.l	#$fff0,d2
	lsr.w	#4,d2
	move.w	(a1)+,d0		* Read Sound On info

	move.w	(a1),d1			* Read PlayCount[0]
	btst.l	#0,d0
	beq.s	NotOn0
	add.w	d2,d1
NotOn0	move.w	d1,(a1)+		* Write PlayCount[0]

	move.w	(a1),d1			* Read PlayCount[1]
	btst.l	#1,d0
	beq.s	NotOn1
	add.w	d2,d1
NotOn1	move.w	d1,(a1)+		* Write PlayCount[1]

	move.w	(a1),d1			* Read PlayCount[2]
	btst.l	#2,d0
	beq.s	NotOn2
	add.w	d2,d1
NotOn2	move.w	d1,(a1)+		* Write PlayCount[2]

	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#3,d0
	beq.s	NotOn3
	add.w	d2,d1
NotOn3	move.w	d1,(a1)+		* Write PlayCount[3]

NotBuff	move.l	_tc_Handler,a1
	move.l	_Handler_sigbit,d1
	moveq	#1,d0			* Signal handler task
	lsl.l	d1,d0
	move.l	_SysBase,a6
	jsr	-324(a6)

*	move.w	#0,$dff18E

	move.l	SaveR,d2
	moveq	#1,d0
	rts

	csect data

*-----------------------------------------------------------------------
SaveR:	dc.l	$00000000

Reg:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000

	END
