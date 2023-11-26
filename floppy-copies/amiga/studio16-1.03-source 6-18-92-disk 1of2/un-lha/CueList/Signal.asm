*-----------------------------------------------------------------------
Signal		EQU	-324
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
* Sample Timecode Function
*
* PreInitalized Registers
*   d2 = Current TimeCode
*   d3 = TimeCode Error Number
*   a6 = ExecBase
*   a2 = Pointer to List Node
*-----------------------------------------------------------------------
	XREF	_task
	XREF	_preload_sigmask
	XREF	_smpte_error_sigmask
	XREF	_SmpteErrorVal
	XDEF	_PreFunc
*-----------------------------------------------------------------------
_PreFunc:
	tst.l	d3
	bne.s	TimeOutSig
	move.l	d2,d0
	and.l	#$00001000,d0
	cmp.l	bitsv,d0
	bne.s	Pre2
	rts
Pre2:
	move.l	d0,bitsv
SigPre:
	move.l	_task,a1
	move.l	_preload_sigmask,d0
	jmp	Signal(a6)		* Signal(task,preload_sigmask);
TimeOutSig:
	move.l	d3,_SmpteErrorVal
	move.l	_task,a1
	move.l	_smpte_error_sigmask,d0
	jmp	Signal(a6)		* Signal(task,smpte_error_sigmask);
*-----------------------------------------------------------------------
	csect data
*-----------------------------------------------------------------------
bitsv:	dc.l	$00000000
	END
