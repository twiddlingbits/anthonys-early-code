*
*
* struct TimeCode {
*    unsigned char hours;
*    unsigned char minutes;
*    unsigned char seconds;
*    unsigned char frames;
*    };
STOP	equ	1
PLAY	equ	2
FF	equ	3
REW	equ	4
SSB_DEFAULTS_SMPTE_MODE	equ	470
		csect  text

		XREF		_LVOSignal
		XREF		_SysBase
		XREF		_tc_handler
		XREF		_smpte_sigmask
		XREF		_smpte_inc_value
		XREF		_SMPTE_REZ
		XREF		_SMPTEsrc
		XREF		_mode
		XREF		_InjectTimeCode
		XREF		_AddTimeCode
		XREF		_StudioBase
		XDEF		_SmpteTimer

* a1 points to interupt data

_SmpteTimer:
	movem.l	d2/d3/a0/a6,-(sp)				; save for graphics.lib bug

* only bump every other call

		add.w		#1,count
		and.w		#1,count
		bne		exit

* Add a tick to the passed time code struct

		move.l	(a1),d0				; Get time code
		move.l	_smpte_inc_value,d1
		move.l	_StudioBase,a6
		move.l	SSB_DEFAULTS_SMPTE_MODE(a6),d3
		jsr	_AddTimeCode

		move.l	d0,(a1)				; save bumped value

do_signal:
		move.b	_mode,d2
		sub.b	#PLAY,d2
		bne	ModeNotPlay

		move.l	d0,d2
		move.l	_SMPTEsrc,d1
		jsr	_InjectTimeCode

ModeNotPlay:
		move.l	_SysBase,a6			; Signal nees this in a6!
		move.l	_tc_handler,a1
		move.l	_smpte_sigmask,d0
		jsr	_LVOSignal(a6)             ; Signal (Task, SignalSet);

exit:
		movem.l	(sp)+,d2/d3/a0/a6					; restore
		moveq	#0,d0
		rts

;
; I Cut this out of Todd's TimeCode/timecodefunc.asm
;
; pass a time code in d0 & d1
; result returned in d0

count	dc.w	0

	END
