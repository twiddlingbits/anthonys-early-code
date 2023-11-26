*-----------------------------------------------------------------------
* greekfit.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   fit (destdata, destoff, desth, destw, destmodulo);
*
*-----------------------------------------------------------------------
*
* Function:
*
*   Fills the destination bitmap with a checkerboard pattern.
*   h is in lines
*   w is in bits
*   off is the bit offset from the start of the line to the start
*       of the image
*   modulo is the number of bytes on a physical line
*   Note: to start on other than the first line, adjust data to the
*         beginning of the starting line.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*   d0   = temporary                                (temp)
*   d1   = temporary                                (temp)
*   d2.w = dest offset in bytes                     (var)
*   d3.w = dest offset in leftover complemented bits(var)
*   d4.w = dest offset in bits                      (const)
*   d5.w = destw in bits                            (const)
*   d6.w = destination line #                       (var)
*   d7.l = data value at destination                (var)
*
*   a2 = dest address of start of current line      (var)
*   a6 = frame pointer
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
destdata          equ 8    * address of dest               l
destoff           equ 14   * destination h offset in bits  w
desth             equ 18   * destination height            w
destw             equ 22   * dest width in bits            w
destmodulo        equ 26   * dest modulo in bytes          w
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
destoffsetbytes   equ -2    * dest offset in bytes       (const)     w
destoffsetbits    equ -4    * dest offset in bits        (const)     w
maxframe          equ -4    * stack frame size
*
*-----------------------------------------------------------------------

      link     a6,#maxframe
      movem.l  d2-d7/a2,-(a7)

      move.w   destoff(a6),d0             * compute dest offset in bytes
      move.w   d0,d1
      lsr.w    #3,d1
      andi.w   #$fffc,d1
      move.w   d1,destoffsetbytes(a6)
      andi.w   #31,d0
      eori.w   #31,d0                     * invert bit count
      move.w   d0,destoffsetbits(a6)

      move.w   destw(a6),d5               * save dest width for loop test

      movea.l  destdata(a6),a2

      clr.w    d6                         * line number
vlooptop:
      move.w   desth(a6),d0
      cmp.w    d6,d0                      * test for vertical loop
      ble      vloopend

differentline:
      clr.l    d4                         * clear dest bit count
      move.w   destoffsetbytes(a6),d2
      move.w   destoffsetbits(a6),d3
      move.l   0(a2,d2.w),d7

hlooptop:
      cmp.w    d4,d5                * test for loop end
      ble.s    hloopend

      move.w   d4,d1                * in or out of checkerboard?
      eor.w    d6,d1
      btst     #0,d1
      beq      notset               * out
   IFNE inverse
      bclr     d3,d7
   ELSE
      bset     d3,d7
   ENDC
notset:
      addq.w   #1,d4                * count next bit
      subq.w   #1,d3                * decrement bit count
      bpl.s    hlooptop
      move.l   d7,0(a2,d2.w)
      moveq    #31,d3               * increment byte count
      addq.w   #4,d2
      move.l   0(a2,d2.w),d7
      bra      hlooptop

hloopend:
vloopbot:
      move.l   d7,0(a2,d2.w)
clipped:
      addq.w   #1,d6
      adda.w   destmodulo(a6),a2
      bra      vlooptop

vloopend:
      movem.l  (a7)+,d2-d7/a2
      unlk     a6
      rts

