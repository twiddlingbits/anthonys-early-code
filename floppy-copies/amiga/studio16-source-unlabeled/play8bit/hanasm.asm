* split_copy
*-----------------------------------------------------------------------
* Calling sequence:
*   split_copy(to, from, maxvalptr, numbytes)
*
*-----------------------------------------------------------------------
*
* Function:
*
* Copy's the high order byte of each word in an array of words into
* an array of bytes. Array size is "numbytes".  Also, while copying
* each byte is checked against *maxvalptr and maxvalptr is updated
* if needed (so it contains the largest byte xfered so far.) 
* 
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
to						equ 8    * destination array of bytes		l
from					equ 12   * source array of words				l
maxvalptr			equ 16   * pointer to byte						l
numbytes				equ 22	* # bytes to xfer						w
*
*-----------------------------------------------------------------------
*
      XDEF  _split_copy

      csect text

_split_copy
      link     a0,#0
	   movem.l  a2-a3/d2,-(sp)	         	; save 

		move.l	to(a0),a1					; pull parameters off stack
		move.l	from(a0),a2
		move.l	maxvalptr(a0),a3
		move.w	numbytes(a0),d0
		

		move.b	(a3),d2						; get max sample value
		subq.w	#1,d0							; dbra needs count-1
				
loop	move.w	(a2)+,d1						; get source sample
		lsr.w		#8,d1							; select high order byte
		cmp.b		d2,d1							; is this sample < max?
		ble.s		skip							; if so skip, else
		move.b	d1,d2							; update new max value
skip	move.b	d1,(a1)+						; save byte
		dbra		d0,loop						; do next sample

		move.b	d2,(a3)						; save new max sample

	   movem.l  (sp)+,a2-a3/d2
		unlk		a0
		rts

		END
