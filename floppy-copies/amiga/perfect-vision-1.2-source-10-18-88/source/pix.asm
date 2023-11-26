*-----------------------------------------------------------------------
* pick.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   pix (data, x, y)
*
* Function:
*
*     returns nyble at x,y in data
*     data points to nyble packed array 160 bytes by 200/400
*
data        equ 8      * data                      l
x           equ 14     * horizontal pixal          w
y           equ 18     * vertical pixal            w
*
*
*-----------------------------------------------------------------------

      XDEF  _pix

      csect text

_pix
      link     a0,#0
      movem.l  d1/a1/d2,-(a7)

* byte = y*160 + x/2
* if (x is even), take msn
* if (x is odd), take lsn

      move.w   x(a0),d1                * Get x position
      move.w   d1,d2
      lsr.w    #1,d1                   * divide by two
      move.w   y(a0),d0
      mulu     #160,d0
      add.w    d0,d1                   * d1 holds index
      movea.l  data(a0),a1
*   moveq    #0,d0
      move.b   0(a1,d1.w),d0           * now d0 holds actual data
      btst     #0,d2
      beq      even
      andi.l   #$f,d0
      bra      done

even: lsr.w    #4,d0
      andi.l   #$f,d0

done: movem.l  (a7)+,d1/a1/d2
      unlk     a0
      rts
      END

