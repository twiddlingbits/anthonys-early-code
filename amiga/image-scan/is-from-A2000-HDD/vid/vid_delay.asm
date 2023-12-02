*-----------------------------------------------------------------------
* delay x clock cycles
*-----------------------------------------------------------------------
* Calling sequence:
*  vid_delay(x);
*
x                equ 8     * number of delays
*
*-----------------------------------------------------------------------

      XDEF  _vid_delay

      csect text

_vid_delay
      link     a0,#0
      movem.l  d1,-(a7)

      move.l   x(a0),d1
      lsr.w    d1,d0        *delay 6+2x  clock cycles

loop: move.b   $bfe101,d1
      bra      loop
      movem.l  (a7)+,d1
      unlk     a0
      rts
      END

