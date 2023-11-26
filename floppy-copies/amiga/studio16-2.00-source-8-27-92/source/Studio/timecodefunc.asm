	INCLUDE	"exec/types.i"
	INCLUDE	"exec/nodes.i"
	INCLUDE	"exec/lists.i"
	INCLUDE	"exec/libraries.i"
	INCLUDE	"exec/funcdef.i"
*-----------------------------------------------------------------------
 STRUCTURE	SSNK,0
	STRUCT	SSNK_NODE,LN_SIZE
	ULONG	SSNK_FLAGS
	APTR	SSNK_FUNC
	APTR	SSNK_USERDATA1
	APTR	SSNK_USERDATA2
	LABEL   SSNK_SIZE
*-----------------------------------------------------------------------
 STRUCTURE	SSB,0
	STRUCT	SSB_Library,LIB_SIZE
	LONG	SSB_CurrentSmpteTime
	LONG	SSB_SmpteTimeOut
	APTR	SSB_CurrentSmpteSource
	STRUCT	SSB_SmpteSinks,LH_SIZE
	STRUCT	SSB_SmpteSources,LH_SIZE
	LONG	SSB_DEFAULTS_SMPTE_MODE
*-----------------------------------------------------------------------
AbsExecBase	EQU	4
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XREF	_StudioBase
	XREF	_LVODisable
	XREF	_LVOEnable
	XREF	_LastTC
	XDEF	_InjectTimeCode
_InjectTimeCode:
*-----------------------------------------------------------------------
* d1 = struct SmpteSouce *src;
* d2 = int NewTimeCode;
*-----------------------------------------------------------------------
	move.l  a5,-(a7)
	move.l	_StudioBase,a5
	cmp.l	SSB_CurrentSmpteSource(a5),d1
	bne	IAmNotTheSrc

	movem.l	d3/a2/a6,-(a7)
	move.l	(AbsExecBase),a6
	move.l	d2,INTC
***************************************************************************
Test_For_Validity:
	move.l	SSB_DEFAULTS_SMPTE_MODE(a5),d3
	jsr	_TestTimeCode
	tst.l	d0
	beq.s	Test_For_Sequence
*************************
	sub.w	#1,VFLAG
	beq.s	TCInvalid
*************************
	move.l	SSB_CurrentSmpteTime(a5),d0
	move.l	SSB_DEFAULTS_SMPTE_MODE(a5),d3
	jsr	_IncTimeCode
	move.l	d0,d2
	bra	NoError4		* !!!!
*************************
TCInvalid:
	move	#2,d3	* SMPTE_ERROR_INVALID
	jsr 	_InjectTimeCodeError
	move.w	#1,VFLAG
	movem.l	(a7)+,d3/a2/a6
	move.l	(a7)+,a5
	rts
***************************************************************************
Test_For_Sequence:
	tst.w	TOflag
	beq.s	NotTimeoutFirst
	move.w	#0,TOflag
	beq	NoError3
NotTimeoutFirst:
	move.l	SSB_CurrentSmpteTime(a5),d0
	move.l	SSB_DEFAULTS_SMPTE_MODE(a5),d3
	cmp.l	d0,d2
	beq	NoError3		* VITSI TRANSMITTING SAME TIME CODE
	jsr	_IncTimeCode
	cmp.l	d0,d2
	beq	NoError3		* Sequence Established!
*************************
	move.l	_LastTC,d0

	move.l	SSB_DEFAULTS_SMPTE_MODE(a5),d3
	jsr	_IncTimeCode
	cmp.l	d0,d2
	bne.s	TCGuess

	sub.w	#1,SFLAG
	beq.s	TCJump
*************************
TCGuess:
	sub.w	#1,TFLAG
	beq.s	TCNoVal
	move.l	SSB_CurrentSmpteTime(a5),d0
	move.l	SSB_DEFAULTS_SMPTE_MODE(a5),d3
	jsr	_IncTimeCode
	move.l	d0,d2
	bra.s	NoError4
*************************
TCJump:
	move.w	#3,SFLAG
	move.l	INTC,_LastTC
	move.l	d2,SSB_CurrentSmpteTime(a5)

	move	#4,d3	* SMPTE_ERROR_JUMP
	jsr _InjectTimeCodeError

	movem.l  (a7)+,d3/a2/a6
	move.l  (a7)+,a5
	rts
*************************
TCNoVal:
	move.w	#1,TFLAG
	move.l	INTC,_LastTC
	movem.l  (a7)+,d3/a2/a6
	move.l  (a7)+,a5
	rts
***************************************************************************
NoError3:
	move.w	#3+1,VFLAG
	move.w	#3,SFLAG
	move.w	#30,TFLAG
NoError4:
	move.l	INTC,_LastTC
	moveq.l	#0,d3
	move.l	d2,SSB_CurrentSmpteTime(a5)	* StudioBase->CurrentSmpteTime=now;

	jsr	_LVODisable(a6)

	move.l	SSB_SmpteSinks(a5),a2
	tst.l	(a2)
	beq.s	NoListEntries

ITCloop:
	move.l	SSNK_FUNC(a2),a0
	jsr	(a0)
	move.l	(a2),a2
	tst.l	(a2)
	bne.s	ITCloop

NoListEntries:
	jsr	_LVOEnable(a6)
	movem.l  (a7)+,d3/a2/a6
IAmNotTheSrc:
	move.l  (a7)+,a5
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XDEF	_InjectTimeCodeError
_InjectTimeCodeError:
*-----------------------------------------------------------------------
* d1 = struct SmpteSouce *src;
* d3 = int SMPTE Errors;
*-----------------------------------------------------------------------
	move.l  a5,-(a7)
	move.l	_StudioBase,a5
	sub.l	SSB_CurrentSmpteSource(a5),d1
	bne.s	EIAmNotTheSrc

	movem.l	d2/a2/a6,-(a7)
	move.l	(AbsExecBase),a6
*******************************************
	move.l	SSB_CurrentSmpteTime(a5),d2	* StudioBase->CurrentSmpteTime=now;

	jsr	_LVODisable(a6)

	cmp.l	#1,d3
	bne.s	NotTimeout
	move.w	#1,TOflag
NotTimeout:
	move.l	SSB_SmpteSinks(a5),a2
	tst.l	(a2)
	beq.s	ENoListEntries

EITCloop:
	move.l	SSNK_FUNC(a2),a0
	jsr	(a0)
	move.l	(a2),a2
	tst.l	(a2)
	bne.s	EITCloop

ENoListEntries:
	jsr	_LVOEnable(a6)
	movem.l  (a7)+,d2/a2/a6
EIAmNotTheSrc:
	move.l  (a7)+,a5
	rts
*-----------------------------------------------------------------------
* d2= TimeCode
* d3= TimeCodeMode
*-----------------------------------------------------------------------
	XDEF	_TestTimeCode
_TestTimeCode:
	movem.l	d2,-(a7)

	tst.l	d3
	bgt	NotDFT
	jmp	TestDF
NotDFT:

	move.l	d2,d0
	and.l	#$000000FF,d0
	addq.l	#1,d0
	sub.l	d3,d0
	bgt	TCF

	move.l	d2,d0
	and.l	#$0000FF00,d0
	sub.l	#$00003b00,d0
	bgt	TCS

	move.l	d2,d0
	and.l	#$00FF0000,d0
	sub.l	#$003b0000,d0
	bgt	TCM

	move.l	d2,d0
	and.l	#$FF000000,d0
	sub.l	#$17000000,d0
	bgt	TCH

	movem.l	(a7)+,d2
	move.l	#0,d0
	rts

TCF:	movem.l	(a7)+,d2
	moveq.l	#1,d0
	rts

TCS:	movem.l	(a7)+,d2
	moveq.l	#2,d0
	rts

TCM:	movem.l	(a7)+,d2
	moveq.l	#3,d0
	rts

TCH:	movem.l	(a7)+,d2
	moveq.l	#4,d0
	rts
*-----------------------------------------------------------------------
TestDF:
	move.l	#30,d3
	Jsr	_TestTimeCode
	tst.l	d0
	beq	DFTest
	rts
DFTest
	move.l	d2,d0
	and.l	#$000000FF,d0
	cmp.l	#2,d0
	blt	CheckSecs
	movem.l	(a7)+,d2
	move.l	#0,d0
	rts
CheckSecs:
	move.l	d2,d0
	and.l	#$0000FF00,d0
	tst.l	d0
	beq	CheckMins
	movem.l	(a7)+,d2
	move.l	#0,d0
	rts
CheckMins:
	move.l	d2,d0
	swap	d0
	and.l	#$000000FF,d0
	divu	#10,d0
	swap	d0
	tst.w	d0
	bne	No0or1
	movem.l	(a7)+,d2
	move.l	#0,d0
	rts
No0or1:
	movem.l	(a7)+,d2
	moveq.l	#5,d0
	rts

*-----------------------------------------------------------------------
* d0= Time code 1
* d1= Time code 2
* d3= Time code mode
*-----------------------------------------------------------------------
	XDEF	_AddTimeCode
_AddTimeCode:
	tst.l	d3
	bgt	NotADF
	jmp	_AddDFTC
NotADF:
	add.l	d1,d0

	move.l	d0,d1
	and.l	#$000000FF,d1
	addq.l	#1,d1
	sub.l	d3,d1
	ble	FOK

	sub.l	d3,d0
	add.l	#$00000100,d0

FOK:	move.l	d0,d1
	and.l	#$0000FF00,d1
	sub.l	#$00003b00,d1
	ble	SOK

	sub.l	#$00003c00,d0
	add.l	#$00010000,d0

SOK:	move.l	d0,d1
	and.l	#$00FF0000,d1
	sub.l	#$003b0000,d1
	ble	MOK

	sub.l	#$003c0000,d0
	add.l	#$01000000,d0

MOK:	move.l	d0,d1
	and.l	#$FF000000,d1
	sub.l	#$17000000,d1
	ble	HOK

	sub.l	#$18000000,d0
HOK	rts
*-----------------------------------------------------------------------
* d0= Time code 1
* d1= Time code 2
* d3= Time code mode
*-----------------------------------------------------------------------
	XDEF	_SubTimeCode
_SubTimeCode:
	tst.l	d3
	bgt	NotSDF
	jmp	_SubDFTC
NotSDF:
	sub.l	d1,d0

	btst.l	#7,d0
	beq	FOOK
	move.l	d0,d1
	and.l	#$FFFFFF00,d0
	and.l	#$000000FF,d1
	add.l	#$FFFFFF00,d1
	add.l	d3,d1
	add.l	d1,d0

FOOK:	btst.l	#15,d0
	beq	SOOK
	move.l	d0,d1
	and.l	#$FFFF00FF,d0
	and.l	#$0000FF00,d1
	add.l	#$FFFF3c00,d1
	add.l	d1,d0

SOOK:	btst.l	#23,d0
	beq	MOOK
	move.l	d0,d1
	and.l	#$FF00FFFF,d0
	and.l	#$00FF0000,d1
	add.l	#$FF3c0000,d1
	add.l	d1,d0

MOOK:	btst.l	#31,d0
	beq	HOOK
	move.l	d0,d1
	and.l	#$00FFFFFF,d0
	and.l	#$FF000000,d1
	add.l	#$18000000,d1
	add.l	d1,d0

HOOK:	rts
*-----------------------------------------------------------------------
* d0= Time Code
* d3= Time Code Mode
*-----------------------------------------------------------------------
	XDEF	_IncTimeCode
_IncTimeCode:
	tst.l	d3
	bgt	NotDF
	jmp	IncDF
NotDF:
	move.l	d1,-(a7)		/* DO NOT REMOVE! */

	add.l	#1,d0

	move.l	d0,d1
	and.l	#$000000FF,d1
	cmp.l	d3,d1
	bne.s	IXIT

	sub.l	d3,d0
	add.l	#$00000100,d0

	move.l	d0,d1
	and.l	#$0000FF00,d1
	cmp.l	#$00003c00,d1
	bne.s	IXIT

	sub.l	#$00003c00,d0
	add.l	#$00010000,d0

	move.l	d0,d1
	and.l	#$00FF0000,d1
	cmp.l	#$003c0000,d1
	bne.s	IXIT

	sub.l	#$003c0000,d0
	add.l	#$01000000,d0

	move.l	d0,d1
	and.l	#$FF000000,d1
	cmp.l	#$18000000,d1
	bne.s	IXIT

	sub.l	#$1800000,d0
IXIT:	move.l	(a7)+,d1
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
IncDF:
	move.l	#30,d3
	Jsr	_IncTimeCode
	move.l	d1,-(a7)
	move.l	d0,d1
	and.l	#$000000FF,d1
	cmp.l	#2,d1
	blt	CheckSecsI
	move.l	(a7)+,d1
	rts
CheckSecsI:
	move.l	d0,d1
	and.l	#$0000FF00,d1
	tst.l	d1
	beq	CheckMinsI
	movem.l	(a7)+,d1
	rts
CheckMinsI:
	move.l	d0,d1
	swap	d1
	and.l	#$000000FF,d1
	divu	#10,d1
	swap	d1
	tst.w	d1
	bne	No0or1I
	move.l	(a7)+,d1
	rts
No0or1I:
	and.l	#$FFFFFF00,d0
	add.l	#2,d0
	move.l	(a7)+,d1
	rts

*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XDEF _DF2Frm
_DF2Frm:
	movem.l	d1/d2,-(a7)
	move.l	d0,d1
	swap	d1
	and.l	#$000000FF,d1
	move.l	d1,d2
	divu	#10,d1
	and.l	#$0000FFFF,d1
	sub.l	d1,d2
	lsl.l	#1,d2
	neg.l	d2

	move.l	d0,d1
	swap	d1
	lsr.l	#8,d1
	and.l	#$000000FF,d1
	mulu	#108,d1
	sub.l	d1,d2


	move.l	d0,d1
	and.l	#$000000FF,d1
	add.l	d1,d2

	move.l	d0,d1
	lsr.l	#8,d1
	and.l	#$000000FF,d1
	mulu	#30,d1
	add.l	d1,d2

	move.l	d0,d1
	swap	d1
	and.l	#$000000FF,d1
	mulu	#(30*60),d1
	add.l	d1,d2

	move.l	d0,d1
	swap	d1
	lsr.l	#8,d1
	and.l	#$000000FF,d1
	mulu	#(30*60*60)/16,d1
	lsl.l	#4,d1
	add.l	d1,d2
	move.l	d2,d0
	movem.l	(a7)+,d1/d2
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	XDEF _Frm2DF
_Frm2DF:
	movem.l	d1/d2,-(a7)
	move.l	d0,d1
	divu	#(18000-18),d1
	move.l	d1,d2
	and.l	#$0000FFFF,d1		* Required?
	mulu	#18,d1
	add.l	d1,d0

	swap	d2
	and.l	#$0000FFFF,d2
	sub.l	#2,d2
	divs	#(1800-2),d2
	and.l	#$0000FFFF,d2
	lsl.l	#1,d2
	add.l	d2,d0

	move.l	d0,d2
	and.l	#$0000000F,d2
	lsr.l	#4,d0

	divs	#(30*60*60)/16,d0
	move.l	d0,d1
	and.l	#$0000FFFF,d0
	swap	d0
	lsl.l	#8,d0

	swap	d1
	and.l	#$0000FFFF,d1
	lsl.l	#4,d1
	or.l	d2,d1

	move.l	d0,d2

	divu	#(30*60),d1
	move.l	d1,d0
	and.l	#$0000FFFF,d1
	swap	d1
	add.l	d1,d2

	swap	d0
	and.l	#$0000FFFF,d0
	divu	#(30),d0
	move.l	d0,d1
	and.l	#$0000FFFF,d0
	lsl.l	#8,d0
	add.l	d0,d2
	swap	d1
	and.l	#$0000FFFF,d1
	add.l	d1,d2

	move.l	d2,d0
	movem.l	(a7)+,d1/d2
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
_AddDFTC:
	movem.l	d0,-(a7)
	move.l	d1,d0
	jsr _DF2Frm
	move.l	d0,d1

	movem.l	(a7)+,d0
	jsr _DF2Frm

	add.l	d1,d0

	cmp.l	#(24*6*17982),d0
	ble	NotOverA
	sub.l	#(24*6*17982),d0
NotOverA:
	tst.l	d0
	bge	NotUnderA
	add.l	#(24*6*17982),d0
NotUnderA:
	jsr	_Frm2DF
	rts
*-----------------------------------------------------------------------
_SubDFTC:
	movem.l	d0,-(a7)
	move.l	d1,d0
	jsr	_DF2Frm
	move.l	d0,d1

	movem.l	(a7)+,d0
	jsr	_DF2Frm

	sub.l	d1,d0

	cmp.l	#(24*6*17982),d0
	blt	NotOverS
	sub.l	#(24*6*17982),d0
NotOverS:
	tst.l	d0
	bge	NotUnderS
	add.l	#(24*6*17982),d0
NotUnderS:
	jsr	_Frm2DF
	rts
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
	csect data
*-----------------------------------------------------------------------
*-----------------------------------------------------------------------
ITCa6:	dc.l	$00000000
ITCa5:	dc.l	$00000000
INTC:	dc.l	$00000000
VFLAG:	dc.w	5
SFLAG:	dc.w	3
TFLAG:	dc.w	256
TOflag	dc.w	0
	END
