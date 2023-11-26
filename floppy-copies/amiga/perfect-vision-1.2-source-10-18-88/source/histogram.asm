*-----------------------------------------------------------------------
* histogram.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   pix (data, table)
*
* Function:
*
*     creates a 16 word table for each color
*     data points to nyble packed array 160 bytes by 200
*     This routine ignores the interlaced field
*
data        equ 8      * data                      l
table       equ 12     * pointer to table          l
*
*
*-----------------------------------------------------------------------

      XDEF  _histogram

      csect text

_histogram
      link     a0,#0
      movem.l  d1/a1/a2,-(a7)

      move.l   data(a0),a1
      move.l   table(a0),a2

      move.l   #0,(a2)            * clear destination count
      move.l   #0,4(a2)
      move.l   #0,8(a2)
      move.l   #0,12(a2)
      move.l   #0,16(a2)           * clear destination count
      move.l   #0,20(a2)
      move.l   #0,24(a2)
      move.l   #0,28(a2)

      move.w   #15999,d1            * do 32000 pixels

loop: moveq.l  #0,d0
      move.b   (a1),d0
      andi.b   #$f,d0
      lsl.w    #1,d0
      addq.w   #1,0(a2,d0.w)          * bump this pixel up
      moveq    #0,d0
      move.b   (a1)+,d0
      lsr.l    #4,d0
      lsl.w    #1,d0
      addq.w   #1,0(a2,d0.w)
      dbra     d1,loop


done: movem.l  (a7)+,d1/a1/a2
      unlk     a0
      rts
      END

