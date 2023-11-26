
	SECTION "Sum64",code
	
	xdef	_Sum64
	xdef	_ShiftResult64

; a0 - pointer to 64 bit SumData64 structure:
;	struct SumData64 {
;		long	op;
;		long  result_ls;
;		long  result_ms;
;		}

_Sum64:
		move.l	(a0),d1
		move.l	#31,d0
		asr.l		d0,d1			; sign extend 32 bit op to 64 bits
		move.l	(a0),d0		; d0/d1 contain op
		add.l		d0,4(a0)
		move.l	8(a0),d0
		addx.l	d1,d0
		move.l	d0,8(a0)
		rts

;
; shift result 15 bits to the right
;

_ShiftResult64:
		move.l	4(a0),d0			;ls
		move.l	8(a0),d1			;ms

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

		asr.l		#1,d1				; 64 bit shift right
		roxr.l	#1,d0	

; now make sure range doesn't excede 16 bits

		btst.l	#31,d1
		bne		negative

; here if positive number

		cmp.l		#0,d1
		bne		possat
		move.l	d0,d1
		and.l		#$FFF8000,d1
		cmp.l		#0,d1
		beq		done
possat:
		move.l	#0,d1
		move.l	#32767,d0
		bra		done

; here if negative number

negative:
		
		cmp.l		#-1,d1
		bne		negsat
		move.l	d0,d1
		and.l		#$FFFF8000,d1
		cmp.l		#$FFFF8000,d1
		beq		done
negsat:
		move.l	#-1,d1
		move.l	#-32767,d0
		bra		done

done:
		move.l	d0,4(a0)			;ls
;		move.l	d1,8(a0)			;ms (trashed by above saturate)
		rts

		END
