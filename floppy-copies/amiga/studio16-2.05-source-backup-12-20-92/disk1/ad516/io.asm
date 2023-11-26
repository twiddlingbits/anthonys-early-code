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

	movea.l	_port,a0
	move.l	_Rbuff,a2
	movea.l	a0,a1
	addq.l	#4,a1


*	move.w	#$0FF0,$dff180		* REMOVE ME
	move.l	#32-1,d1		* BLOCKWORDS -1
LoopRec:
*****************************************
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
*****************************************
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
					*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
	move.w	(a1),(a2)+		*
*****************************************
	dbra	d1,LoopRec
*	move.w	#$0000,$dff180		* REMOVE ME

	move.l	a2,_Rbuff
	movem.l	RegGRD,d1/a0-a2
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_EmptyRecData
	xref	_port
	xref	_Rbuff
_EmptyRecData:
	movem.l	d1/a0-a2,RegGRD

	movea.l	_port,a0
	move.l	_Rbuff,a2
	movea.l	a0,a1
	addq.l	#4,a1

*	move.w	#$00F0,$dff180		* REMOVE ME
	move.l	#32-1,d1		* BLOCKWORDS -1
LoopMT:
*****************************************
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
*****************************************
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
					*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
	move.w	(a1),d0		*
*****************************************
	dbra	d1,LoopMT

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

	move.l	#9,d1		* HiLow WORDS -1

waitHiLoLeft:
	btst.b	#1,(a0)
	bne.s	waitHiLoLeft
	move.w	(a1),d0
	lsl.l	#8,d0

waitHiLoRight:
	btst.b	#1,(a0)
	bne.s	waitHiLoRight
	move.w	(a1),d0
	lsr.l	#8,d0

	move.w	d0,(a2)+
	dbra	d1,waitHiLoLeft

	movem.l	(a7)+,d1/a0-a2
	rts
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_SendSyncedBlock
	XREF	_DSP_BUFF_PTR
_SendSyncedBlock:
	link	a1,#0
	movem.l	d2/a4-a5,-(a7)
	movea.l	port(a1),a4
	movea.l	buff(a1),a0
	move.l	samps(a1),d2

*****************************************
	move.l	chan(a1),d1		*
	lsl.w	#8,d1			*
	add.w	#$0021,d1		*
waitS9:	btst.b	#0,(a4)			* Send MovePlayPtr
	bne.s	waitS9			*
	move.w	d1,2(a4)		*
*****************************************
waitS0:	btst.b	#0,(a4)			*
	bne.s	waitS0			* Send Buff Pointer
	move.w	_DSP_BUFF_PTR,2(a4)	*
*****************************************
	bra.s FirstBJump
*-----------------------------------------------------------------------

*-----------------------------------------------------------------------
	XDEF	_SendFirstBlock
	XREF	_DSP_BUFF_PTR
_SendFirstBlock:
	link	a1,#0
	movem.l	d2/a4-a5,-(a7)
	movea.l	port(a1),a4
	movea.l	buff(a1),a0
	move.l	samps(a1),d2

*****************************************
	move.l	chan(a1),d1		*
	lsl.w	#8,d1			*
	add.w	#$0031,d1		*
waitF9:	btst.b	#0,(a4)			* Send MovePlayPtr
	bne.s	waitF9			*
	move.w	d1,2(a4)		*
*****************************************
waitF0:	btst.b	#1,(a4)			*
	bne.s	waitF0			* Recieve Buff Pointer
	move.w	2(a4),d0		*
	move.w	d0,_DSP_BUFF_PTR	*
*****************************************
	bra.s FirstBJump
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XDEF	_SendBlock
_SendBlock:
	link	a1,#0
	movem.l	d2/a4-a5,-(a7)
	movea.l	port(a1),a4
	movea.l	buff(a1),a0
	move.l	samps(a1),d2

FirstBJump:
	move.l	a4,a5
	add.l	#4,a5

PlayDataLoop:
	cmp.l	#1024,d2
	bge.s	FullBlock

*****************************************
	move.l	d2,d0			* Going To Send Less Than 512 Samps
	move.l	#0,d2			*
*****************************************
	move.l	d0,d1			*
	lsl.l	#4,d1			* 
	add.l	chan(a1),d1		*
	lsl.l	#1,d1			*
*****************************************
	subq.l	#1,d0			*
*****************************************
waitP1:	btst.b	#3,(a4)			* Wait PLAY FIFO ½ MT
	beq.s	waitP1			*
*****************************************
*	move.w	#$0F0F,$dff180		* REMOVE ME
LoopP1:	move.w	(a0)+,4(a4)		* FILL FIFO
	dbra	d0,LoopP1		*
*****************************************
	bra	SendBPCmd

FullBlock:
*****************************************
	sub.l	#1024,d2		* Going To Send 1024 Samples
*****************************************
	move.l	#1024<<4,d1		*
	add.l	chan(a1),d1		*  
	lsl.l	#1,d1			*
*****************************************
	move.l	#16-1,d0		*
*	move.w	#$0F00,$dff180		* REMOVE ME
*****************************************
waitPF:	btst.b	#3,(a4)			* Wait PLAY FIFO ½ MT
	beq.s	waitPF			*
*****************************************
*	move.w	#$0FFF,$dff180		* REMOVE ME
*****************************************
LoopPF:	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
*****************************************
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
					*
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
	move.l	(a0)+,(a5)		* FILL FIFO
*****************************************
	dbra	d0,LoopPF		*
*	move.w	#$0000,$dff180		* REMOVE ME
*****************************************


*****************************************
*****************************************
SendBPCmd:
*****************************************
waitP2:	btst.b	#0,(a4)			*
	bne.s	waitP2			* SendW((x<<1)+(y<<4));
	move.w	d1,2(a4)		*
*****************************************

	tst.l	d2
	bne	PlayDataLoop

	move.l	a0,d0

	movem.l	(a7)+,d2/a4-a5		*Reset Registers
	unlk	a1
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
	xref	_PlayCntr
	xref	_SmpteStartCount
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
*---------------------------------------------------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*----------------------------
*---------------------------------------------------------------------
*---------------------------------------------------------------------
* HANDLE SMPTE MSGS ...
*  d1 & 0x000F = 3 , Sec\Frame
*  d1 & 0x000F = 5 , Hour\Min
*  d1 & 0x000F = 7 , More Than 85 Bits
*---------------------------------------------------------------------
	move.w	d1,d0
	and.w	#$000F,d0
*----------------------------
	cmp.w	#$05,d0
	bne.s	NOT_SMPTE_hm
*----------------------------
Is_Smpte_HM
	lsr.w	#2,d1
	lsr.b	#2,d1
	move.w	d1,TimeCodeHM
	bra	SkipSig
*----------------------------
NOT_SMPTE_hm:
	cmp.w	#$03,d0
	bne.s	NOT_SMPTE_sf
*----------------------------
	lsr.w	#2,d1
	lsr.b	#2,d1
	move.w	d1,TimeCodeSF
	moveq.l	#0,d0
*----------------------------
waitINT:			* Waiting for SmpteSamplingRate Cntr
	btst.b	#1,-2(a0)
	bne.s	waitINT
*----------------------------
	move.w	(a0),d0

	subq.w	#1,d1
	cmp.w	LASTTC,d1
	bne	SkipCNTR

	cmp.w	#$7fff,d0
	beq	SkipCNTR
	add.l	d0,_RecSamps
	addq.l	#1,_RecFrames
*----------------------------
SkipCNTR:
*----------------------------
	tst.l	_SmpteStartCount
	bne	SkipInject

	move.w	TimeCodeSF,LASTTC

	movem.l	d1/d2/d3/a6,-(a7)
	move.l	TimeCode,d2
	move.l	_SMPTEsrc,d1
	jsr	_InjectTimeCode
	movem.l	(a7)+,d1/d2/d3/a6

	bra	SkipSig

SkipInject:
	move.l	_SmpteStartCount,d0
	subq.l	#1,d0
	move.l	d0,_SmpteStartCount
	bra	SkipSig
*----------------------------
NOT_SMPTE_sf:
*---------------------------------------------------------------------
*---------------------------------------------------------------------
*---------------------------------------------------------------------
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
Buff	add.l	#1,_PlayCntr


	move.w	(a1)+,d0		* a1=Sound On

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

********* REMOVE IF ONLY 4 CHANNELS ***********


	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#4,d0
	beq.s	NotOn4
	addq.w	#1,d1
NotOn4	move.w	d1,(a1)+		* Write PlayCount[3]

	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#5,d0
	beq.s	NotOn5
	addq.w	#1,d1
NotOn5	move.w	d1,(a1)+		* Write PlayCount[3]

	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#6,d0
	beq.s	NotOn6
	addq.w	#1,d1
NotOn6	move.w	d1,(a1)+		* Write PlayCount[3]

	move.w	(a1),d1			* Read PlayCount[3]
	btst.l	#7,d0
	beq.s	NotOn7
	addq.w	#1,d1
NotOn7	move.w	d1,(a1)+		* Write PlayCount[3]
	bra	Sig
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XREF	_CardSoftRev
	XREF	_CardSoft
	XDEF	_SendPgm
_SendPgm:
	link	a0,#0
	movem.l	d3/a4-a5,-(a7)		*Save Registers
	movea.l	port(a0),a5
	movea.l	buff(a0),a1

	movea.l	a5,a4
	addq.l	#2,a5

	move.l	#$03f1,d0

loopC:	move.w	(a1)+,d1
	move.w	(a1)+,d2
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

	movem.l	(a7)+,d3/a4-a5		*Reset Registers
	unlk	a0
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
