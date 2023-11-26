port		equ	8
buff		equ	12
chan		equ	16
samps		equ	20
s_start		equ	8
s_end		equ	12
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_GetRecData
	xref	_port
	xref	_Rbuff
_GetRecData:
	movem.l	d1/a0-a2,RegGRD

	move.l	_Rbuff,a2
	movea.l	_port,a0
	movea.l	a0,a1
	addq.l	#2,a1

waitR2:	btst.b	#0,(a0)			*
	bne.s	waitR2			* Send RecDataReq

	move.w	#$000d,(a1)		*

waitR3:	btst.b	#0,(a0)			* DO NOT REMOVE ME, I prevent an ad1012
	bne.s	waitR3			* Interupt from being interpreted as Record Data!!

	move.l	#1023,d1		* BLOCKWORDS -1

waitRec btst.b	#1,(a0)
	bne.s	waitRec
	move.w	(a1),(a2)+
	dbra	d1,waitRec

	move.l	a2,_Rbuff
	movem.l	RegGRD,d1/a0-a2
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_GetHiLoData
	xref	_port
	xref	_HH
_GetHiLoData:
	movem.l	d1/a0-a2,-(a7)

	movea.l	_port,a0
	movea.l	a0,a1
	addq.l	#2,a1
	move.l	_HH,a2

waitH2:	btst.b	#0,(a0)			*
	bne.s	waitH2			* Send HiLoRequest

	move.w	#$001F,(a1)		*

waitH3:	btst.b	#0,(a0)			* DO NOT REMOVE ME, I prevent an ad1012
	bne.s	waitH3			* Interupt from being interpreted as Record Data!!

	move.l	#5,d1		* HiLow WORDS -1

waitHiLo:
	btst.b	#1,(a0)
	bne.s	waitHiLo
	move.w	(a1),(a2)+
	dbra	d1,waitHiLo

	movem.l	(a7)+,d1/a0-a2
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_SendBlock
_SendBlock:
	movem.l	d1/a0-a1/a4-a5,RegSB		*Save Registers
	link	a1,#0
	movea.l	port(a1),a5
	movea.l	buff(a1),a0
	move.l	samps(a1),d0

	movea.l	a5,a4
	addq.l	#2,a5

*****************************************
	move.l	d0,d1			*
	lsl.l	#3,d1			*
	add.l	chan(a1),d1		*
	lsl.l	#1,d1			* SendW((x<<1)+(y<<4));
waitP2:	btst.b	#0,(a4)			*
	bne.s	waitP2			*
	move.w	d1,(a5)			*
*****************************************

	subq.l	#1,d0

LoopQ:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	LoopQ
	move.w	(a0)+,(a5)		* send
	dbra	d0,LoopQ

	move.l	a0,d0

	unlk	a1
	movem.l	RegSB,d1/a0-a1/a4-a5		*Reset Registers
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_SendFirstBlock
_SendFirstBlock:
	movem.l	d1-d2/a0-a1/a4-a5,RegSFB		*Save Registers
	link	a1,#0
	movea.l	port(a1),a5
	movea.l	buff(a1),a0
	move.l	samps(a1),d0
	move.l	chan(a1),d2

	movea.l	a5,a4
	addq.l	#2,a5
*****************************************
	move.l	d2,d1			*
	lsl.w	#8,d1			*
	add.w	#$0031,d1		*
waitF1:	btst.b	#0,(a4)			* Send MovePlayPtr
	bne.s	waitF1			*
	move.w	d1,(a5)			*
*****************************************
	move.l	d0,d1			*
	lsl.l	#3,d1			*
	add.l	d2,d1			*
	lsl.l	#1,d1			* SendW((x<<1)+(y<<4));
waitF2:	btst.b	#0,(a4)			*
	bne.s	waitF2			*
	move.w	d1,(a5)			*
*****************************************
	subq.l	#1,d0

loopFP:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	loopFP
	move.w	(a0)+,(a5)		* send
	dbra	d0,loopFP

	move.l	a0,d0

	unlk	a1
	movem.l	RegSFB,d1-d2/a0-a1/a4-a5		*Reset Registers
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	xdef	_DspXferInt

	xref	_SysBase
	xref	_Hand_Signal
	xref	_SigbitFlag
	xref	_tc_Handler
	xref	_SMPTEsrc
	xref	_InjectTimeCode
	xref	_HL
	xref	_port
	xref	_RecSamps
	xref	_RecFrames
*----------------------------
_DspXferInt:
	move.l	(a1)+,a0		* a1=CardAddress
	btst.b	#2,(a0)
	beq.s	GoOd			* Did I Cause the interupt?
	moveq	#0,d0
	rts				* No I did not.
*----------------------------
GoOd:
	move.w	d1,(a0)			* clear interupt Writing Status
	adda.w	#2,a0
	move.w	(a0),d1			* read dsp command
*----------------------------
* HANDLE SMPTE MSGS ...
*  d1 & 0x000F = 3 , Sec\Frame
*  d1 & 0x000F = 5 , Hour\Min
*----------------------------
	move.w	d1,d0
	and.w	#$000F,d0
	cmp.w	#$05,d0
	bne.s	NOT_SMPTE_hm

	lsr.w	#2,d1
	lsr.b	#2,d1
	move.w	d1,TimeCodeHM
	bra	SkipSig

NOT_SMPTE_hm:

	cmp.w	#$03,d0
	bne.s	NOT_SMPTE_sf

	lsr.w	#2,d1
	lsr.b	#2,d1
	move.w	d1,TimeCodeSF
	move.l	d0,-(a7)
	moveq.l	#0,d0

waitINT:
	btst.b	#1,-2(a0)
	bne.s	waitINT
	move.w	(a0),d0

	subq.w	#1,d1
	cmp.w	LASTTC,d1
	bne	SkipCNTR

	cmp.w	#$7fff,d0
	beq	SkipCNTR

	add.l	d0,_RecSamps
	addq.l	#1,_RecFrames

SkipCNTR:
	move.w	TimeCodeSF,LASTTC
	move.l	(a7)+,d0

	movem.l	d1/d2/a6,-(a7)
	move.l	TimeCode,d2
	move.l	_SMPTEsrc,d1
	jsr	_InjectTimeCode
	movem.l	(a7)+,d1/d2/a6

	bra	SkipSig

*----------------------------
*----------------------------
*----------------------------
NOT_SMPTE_sf:
	move.w	(a1),d0		* read buffer pointer , a1= CmdBuffPtr
	addq.w	#1,d0
	and.w	#$0FFF,d0
	move.w	d0,(a1)+	* a1= CmdBuffPtr

	movea.l	(a1)+,a0	* a1= DspCmd
	lsl.w	#1,d0		* Shift for word allignment
	move.w	d1,$0(a0,d0.w)	* store dsp command
*----------------------------
	sub.w	#$000f,d1
	beq.s	Buff
*----------------------------
Sig	tst.w	_SigbitFlag
	bne	SkipSig
	move.w	#1,_SigbitFlag
*----------------------------
	move.l	_tc_Handler,a1
	move.l	_Hand_Signal,d0
	move.l	_SysBase,a6
	jsr	-324(a6)
*----------------------------
SkipSig	moveq	#1,d0
	rts
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
Buff	move.w	(a1)+,d0		* a1=Sound On

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
	movem.l	d0-d3/a2-a6,Reg		*Save Registers
	link	a2,#0
	movea.l	port(a2),a5
	movea.l	buff(a2),a3

	movea.l	a5,a4
	addq.l	#2,a5

	move.l	#$03f1,d0

loopC:	move.w	(a3)+,d1
	move.w	(a3)+,d2
	lsr.w	#8,d2

waitc1:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	waitc1
	move.w	d2,(a5)			* send lsb

waitc2:	btst.b	#0,(a4)			* wait  for write ok
	bne.s	waitc2
	move.w	d1,(a5)			* send msb

	subq.l	#1,d0
	bne	loopC

	move.w	d1,_CardSoftRev
	move.w	d2,_CardSoft   

	unlk	a2
	movem.l	Reg,d0-d3/a2-a6		*Reset Registers
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
TimeCode:
TimeCodeHM:	dc.w	$0000
TimeCodeSF:	dc.w	$0000
*-----------------------------------------------------------------------
Svclr	dc.l	$00000000

Reg:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
RegSB:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
RegSFB:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
RegGRD:	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
	dc.l	$00000000,$00000000,$00000000,$00000000
GGSAMPLES dc.w	$0000
SAMPLES dc.w	$0000
LASTTC	dc.l	$00000000
	END

**********************************************
**********************************************
**********************************************
