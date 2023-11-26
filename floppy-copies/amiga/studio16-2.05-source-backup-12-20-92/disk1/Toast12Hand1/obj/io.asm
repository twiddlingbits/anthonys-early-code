port		equ	8
buff		equ	12
end		equ	16
left		equ	20
chan		equ	24
s_start		equ	8
s_end		equ	12
*-----------------------------------------------------------------------
BLOCKWORDS	equ	512
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	GetRecData
	xref	_port
	xref	_Rbuff
GetRecData:

	move.l	a2,SaveA2
	move.l	_Rbuff,a2
	movea.l	_port,a0
	movea.l	a0,a1
	addq.l	#2,a1

	move.l	#1023,d2		* BLOCKWORDS -1

waitRec btst.b	#1,(a0)
	bne.s	waitRec

	move.w	(a1),d0
	and.w	#$FFF0,d0
	move.w	d0,(a2)+

*	addq.w	#1,d1
*	move.w	d1,$dff180

	dbra	d2,waitRec

	move.l	a2,_Rbuff
	move.l	SaveA2,a2
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
*	addq.w	#1,$dff182

	move.w	(a6),d1
	addq.l	#2,a6
*	and.w	#$FFF0,d1
*	or.w	d3,d1

	subq.l	#1,(a2)
	beq	Xit			* this statement is REALY NEEDED

waitP:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	waitP

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
	xref	_SigbitFlag
	xref	_tc_Handler
	xref	_HL
	xref	_port
*----------------------------
_DspXferInt:
	move.l	(a1)+,a0
	btst.b	#2,(a0)
	beq.s	GoOd
	moveq	#0,d0
	rts
*----------------------------
GoOd:
	move.w	d1,(a0)			* clear interupt
	adda.w	#2,a0
	move.w	(a0),d1			* read dsp command
*----------------------------
	move.l	d2,SaveR

	move.w	(a1),d2		* read buffer pointer
	addq.w	#1,d2
	and.w	#$0FFF,d2
	move.w	d2,(a1)+

	movea.l	(a1)+,a0
	lsl.w	#1,d2
	move.w	d1,$0(a0,d2.w)	* store dsp command
*----------------------------
	move.w	d1,d2
	and.w	#$000f,d1

	cmp.w	#$000f,d1
	beq.s	Buff
	cmp.w	#$0009,d1
	beq.s	Rec
	cmp.w	#$0001,d1
	beq.s	HiLo
*----------------------------
*----------------------------
*----------------------------
Sig	move.w	_SigbitFlag,d1
	bne	SkipSig
	move.w	#1,_SigbitFlag
*----------------------------
	move.l	_tc_Handler,a1
	move.l	_Handler_sigbit,d1
	moveq	#1,d0			* Signal handler task
	lsl.l	d1,d0
	move.l	_SysBase,a6
	jsr	-324(a6)
*----------------------------
SkipSig	*jsr	_flash
*----------------------------
	move.l	SaveR,d2
	moveq	#1,d0
	rts
*----------------------------
*----------------------------
*----------------------------
Rec
	jsr GetRecData
	bra	Sig
*----------------------------
*----------------------------
*----------------------------
HiLo	lea	_HL,a1

	movea.l	_port,a0
	moveq.l	#5,d0

waitHL:	btst.b	#1,(a0)
	bne.s	waitHL
	adda.w	#2,a0

GetHL:
	move.w	(a0),(a1)+
	suba.w	#2,a0
	DBRA	d0,waitHL


	bra	Sig
*----------------------------
*----------------------------
*----------------------------
Buff	move.w	(a1)+,d0		* Read Sound On info

	move.w	(a1),d1			* Read PlayCount[0]
	btst.l	#0,d0
	beq.s	NotOn0
	addq.w	#1,d1
NotOn0	move.w	d1,(a1)+		* Write PlayCount[0]

	move.w	(a1),d1			* Read PlayCount[1]
	btst.l	#1,d0
	beq.s	NotOn1
	addq.w	#1,d1
NotOn1	move.w	d1,(a1)+		* Write PlayCount[1]

	move.w	(a1),d1			* Read PlayCount[2]
	btst.l	#2,d0
	beq.s	NotOn2
	addq.w	#1,d1
NotOn2	move.w	d1,(a1)+		* Write PlayCount[2]

	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#3,d0
	beq.s	NotOn3
	addq.w	#1,d1
NotOn3	move.w	d1,(a1)+		* Write PlayCount[3]
	bra	Sig
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XREF	_CardSoftRev
	XREF	_CardSoft
	XDEF	_SendPgm
_SendPgm:
	movem.l	d1-d3/a4-a6,Reg		*Save Registers
	link	a1,#0
	movea.l	port(a1),a5
	movea.l	buff(a1),a6

	movea.l	a5,a4
	addq.l	#2,a5

	move.l	#$03f1,d0

loopC:	move.w	(a6)+,d1
	move.w	(a6)+,d2
	lsr.w	#8,d2

waitc1:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	waitc1
	move.w	d2,(a5)			* send lsb

*	move.w	d1,$dff180

waitc2:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	waitc2
	move.w	d1,(a5)			* send msb

	subq.l	#1,d0
	bne	loopC

	move.w	d1,_CardSoftRev
	move.w	d2,_CardSoft   

	unlk	a1
	movem.l	Reg,d1-d3/a4-a6		*Reset Registers
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XDEF	_flash
_flash:
	subq.w	#1,Svclr
	move.w	Svclr,d0
	lsl.w	#8,d0
	move.w	d0,$dff18E
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	csect data
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
SaveR:	dc.l	$00000000
SaveA2:	dc.l	$00000000
Svclr	dc.l	$00000000
Cport	dc.l	$00000000

Reg:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000

	END
