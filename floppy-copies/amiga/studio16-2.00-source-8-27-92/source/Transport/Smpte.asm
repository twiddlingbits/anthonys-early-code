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
	XREF	_sw_trigger_sigmask
	XREF	_sw_end_sigmask
	XREF	_EndTC
	XREF	_StartTC
	XDEF	_TCFunc
*-----------------------------------------------------------------------
_TCFunc:
	cmp.l	_StartTC,d2
	beq.s	TrigSig
	cmp.l	_EndTC,d2
	beq.s	EndSig
	rts
TrigSig:
	move.l	_task,a1
	move.l	_sw_trigger_sigmask,d0
	jmp	Signal(a6)		* Signal(task,preload_sigmask);

EndSig:
	move.l	_task,a1
	move.l	_sw_end_sigmask,d0
	jmp	Signal(a6)		* Signal(task,preload_sigmask);
*-----------------------------------------------------------------------
	csect data
*-----------------------------------------------------------------------
bitsv:	dc.l	$00000000
	END
