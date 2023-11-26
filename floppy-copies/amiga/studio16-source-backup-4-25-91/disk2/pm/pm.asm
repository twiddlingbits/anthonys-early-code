
		csect  text
	
		XREF		_vcount
		XREF		_dataptr
		XREF		_cpu
		XREF		_cpubuf
		XREF		_latch_size
		XREF		_tc_dograph
		XREF		_graph_sigbit
		XREF		_SysBase
		XDEF		_VertCount

_VertCount
	add.l		#1,(a1)				; a1 points to vcount
	move.l	(a1),d0
	cmp.l		_latch_size,d0
	bne		exit

	move.l	a0,-(sp)				; save for graphics.lib bug
	move.l	a2,-(sp)				; save for graphics.lib bug

	move.l	#0,(a1)
	move.l	_dataptr,d0
	move.l	_cpubuf,a1
	lsl.l		#2,d0					;*4
	move.l	_cpu,0(a1,d0.l)
	lsr.l		#2,d0
	addq.l	#1,d0
	and.l		#$ff,d0
	move.l	d0,_dataptr
	move.l	#0,_cpu
   move.l	_SysBase,a2
   move.l	_tc_dograph,a1
   move.l	_graph_sigbit,d1
   moveq    #1,d0
   lsl.l    d1,d0
   jsr      -324(a2)             ; Signal (Task, SignalSet);

	move.l			(sp)+,a2					; restore
	move.l			(sp)+,a0					; restore

exit:
	moveq		#0,d0
	rts
	END

