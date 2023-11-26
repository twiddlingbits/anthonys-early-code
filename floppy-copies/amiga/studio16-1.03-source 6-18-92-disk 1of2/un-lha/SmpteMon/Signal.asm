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
	XREF	_SmpteErrorNum
	XREF	_task
	XREF	_sink_sigmask
	XREF	_error_sigmask
	XDEF	_SinkFunc
*-----------------------------------------------------------------------
_SinkFunc:
	tst.l	d3
	beq.s	NoError
	move.l	d3,_SmpteErrorNum
	move.l	_task,a1
	move.l	_error_sigmask,d0
	jmp	Signal(a6)		* Signal(task,sink_sigmask);
NoError:
	move.l	_task,a1
	move.l	_sink_sigmask,d0
	jmp	Signal(a6)		* Signal(task,sink_sigmask);
*-----------------------------------------------------------------------
	END
