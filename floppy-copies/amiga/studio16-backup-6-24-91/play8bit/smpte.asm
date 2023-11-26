*
*
* struct TimeCode {
*    unsigned char hours;
*    unsigned char minutes;
*    unsigned char seconds;
*    unsigned char frames;
*    };

		csect  text

		XREF		_LVOSignal	
		XREF		_SysBase
		XREF		_tc_handler
		XREF		_smpte_sigmask
		XDEF		_SmpteTimer

* a1 points to interupt data

_SmpteTimer:
	move.l	a0,-(sp)				; save for graphics.lib bug

* Add one too the passed time code struct

		move.b	3(a1),d0				; Get frames
		addq.b	#1,d0					; bump it
		cmp.b		#30,d0				; frames = 30 ?
		beq		newsec
		move.b	d0,3(a1)				; save bumped value
		bra		do_signal
newsec:
		moveq.l	#0,d0
		move.b	d0,3(a1)				; save bumped value

		move.b	2(a1),d0				; Get seconds
		addq.b	#1,d0					; bump it
		cmp.b		#60,d0				; seconds = 60?
		beq		newmin
		move.b	d0,2(a1)				; save bumped value
		bra		do_signal
newmin:
		moveq.l	#0,d0
		move.b	d0,2(a1)				; save bumped value

		move.b	1(a1),d0				; Get mintues
		addq.b	#1,d0					; bump it
		cmp.b		#60,d0				; minutes = 60?
		beq		newhour
		move.b	d0,1(a1)				; save bumped value
		bra		do_signal
newhour:
		moveq.l	#0,d0
		move.b	d0,1(a1)				; save bumped value

		move.b	0(a1),d0				; Get hours
		addq.b	#1,d0					; bump hours
		cmp.b		#24,d0				; hours = 24?
		bne		done
		moveq.l	#0,d0
done:
		move.b	d0,0(a1)				; save bumped value

do_signal:
	   move.l	_SysBase,a6			; Signal nees this in a6!
	   move.l	_tc_handler,a1
	   move.l	_smpte_sigmask,d0
	   jsr      _LVOSignal(a6)             ; Signal (Task, SignalSet);

		move.l			(sp)+,a0					; restore

exit:
		moveq		#0,d0
		rts
		END

