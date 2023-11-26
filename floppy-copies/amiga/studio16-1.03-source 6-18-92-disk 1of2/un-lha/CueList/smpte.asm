	INCLUDE	"exec/types.i"
	INCLUDE	"exec/nodes.i"
*-----------------------------------------------------------------------
 STRUCTURE	SSNK,0
	STRUCT	SSNK_NODE,LN_SIZE
	ULONG	SSNK_FLAGS
	APTR	SSNK_FUNC
	APTR	SSNK_USERDATA
	LABEL   SSNK_SIZE
*-----------------------------------------------------------------------
 STRUCTURE	PPS,0
	APTR	PPS_TASK
	ULONG	PPS_TRIGGER
	ULONG	PPS_UNLOAD
	ULONG	PPS_GETCHAN
	ULONG	PPS_SEEK
	ULONG	PPS_STARTTC
	ULONG	PPS_ENDTC
	ULONG	PPS_TRIGTC
	ULONG	PPS_CHANTC
	ULONG	PPS_CMDTC
	ULONG	PPS_FLAGS
	ULONG	PPS_USER_DATA
	LABEL   PPS_SIZE
*-----------------------------------------------------------------------
Signal		EQU	-324
*-----------------------------------------------------------------------
	csect text
*-----------------------------------------------------------------------
* Sample Timecode Function
*
* PreInitalized Registers
*   d2 = Current TimeCode
*   a6 = ExecBase
*   a2 = Current List Node Pointer
*-----------------------------------------------------------------------
	XDEF	_SinkFunc
*-----------------------------------------------------------------------
_SinkFunc:
	move.l	a3,-(a7)
	move.l	SSNK_USERDATA(a2),a3

	cmp.l	PPS_CHANTC(a3),d2
	beq.s	GetChan

	cmp.l	PPS_TRIGTC(a3),d2
	beq.s	Trigger

	cmp.l	PPS_ENDTC(a3),d2
	bgt.s	UnLoad

	cmp.l	PPS_TRIGTC(a3),d2
	bgt.s	Seek

	cmp.l	PPS_CMDTC(a3),d2
	blt.s	UnLoad

	move.l	(a7)+,a3
	rts
*-----------------------------------------------------------------------
GetChan:
	move.l	PPS_TASK(a3),a1
	move.l	PPS_GETCHAN(a3),d0
	move.l	(a7)+,a3
	jmp	Signal(a6)		* Signal(task,sink_sigmask);

Trigger:
	move.l	PPS_TASK(a3),a1
	move.l	PPS_TRIGGER(a3),d0
	move.l	(a7)+,a3
	jmp	Signal(a6)		* Signal(task,sink_sigmask);

Seek:
	move.l	PPS_TASK(a3),a1
	move.l	PPS_SEEK(a3),d0
	move.l	(a7)+,a3
	jmp	Signal(a6)		* Signal(task,sink_sigmask);

UnLoad:
	move.l	PPS_TASK(a3),a1
	move.l	PPS_UNLOAD(a3),d0
	move.l	(a7)+,a3
	jmp	Signal(a6)		* Signal(task,sink_sigmask);

*-----------------------------------------------------------------------
	END
