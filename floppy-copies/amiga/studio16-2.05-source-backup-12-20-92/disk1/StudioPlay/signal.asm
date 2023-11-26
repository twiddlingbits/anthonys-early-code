*-----------------------------------------------------------------------
Signal		EQU	-324
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
*
* PreInitalized Registers
*   d2 = Current TimeCode
*   d3 = TimeCode Error Number
*   a6 = ExecBase
*   a2 = Pointer to List Node
*-----------------------------------------------------------------------
	XREF	_HitTime
	XREF	_task
	XREF	_sink_sigmask
	XDEF	_SinkFunc
*-----------------------------------------------------------------------
_SinkFunc:
	move.l	_HitTime,d0
	cmp.l		d2,d0
	beq.s		dosig
	rts
dosig:
	move.l	_task,a1
	move.l	_sink_sigmask,d0
	jmp	Signal(a6)		* Signal(task,sink_sigmask);
*-----------------------------------------------------------------------
	END
