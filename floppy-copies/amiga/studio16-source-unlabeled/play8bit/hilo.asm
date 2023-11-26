
		csect  text

		XREF		_LVOSignal	
		XREF		_SysBase
		XREF		_tc_handler
		XREF		_hilo_sigmask
		XREF 		_count
		XDEF		_HiLoTimer

* a1 points to interupt data

_HiLoTimer:
	add.w		#1,_count
	cmp.w		#3,_count
	bne		exit				;bne
	move.w	#0,_count

doit:
	move.l	a0,-(sp)				; save for graphics.lib bug
;	move.l	a2,-(sp)				; save for graphics.lib bug

;	move.w	#0,$dff180			; debug color flash

   move.l	_SysBase,a6			; Signal nees this in a6!
   move.l	_tc_handler,a1
   move.l	_hilo_sigmask,d0
   jsr      _LVOSignal(a6)             ; Signal (Task, SignalSet);

;	move.l			(sp)+,a2					; restore
	move.l			(sp)+,a0					; restore

exit:
	moveq		#0,d0
	rts
	END

