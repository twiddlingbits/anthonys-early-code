
*-----------------------------------------------------------------------
* fit.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   fit (srcdata,  srcoff,  srch,  srcw,  srcmodulo,
*        destdata, destoff, desth, destw, destmodulo);
*
*-----------------------------------------------------------------------
*
* Function:
*
*   Resizes the source bitmap to fit in the destination bitmap.
*   h is in lines* w is in bits
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
*   d6.l = src offset in fixed pt COORD<<16         (var)
*   d7.l = data value at destination                (var)
*
*   a1 = src address of start of current line       (var)
*   a2 = dest address of start of current line      (var)
*   a3 = hstep                                      (const)
*   a4 = original source address                    (const)
*   a5 = temporary                                  (temp)
*   a6 = frame pointer
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
srcdata           equ 8    * address of source             l
srcoff            equ 14   * source h offset in bits       w
srch              equ 18   * source height                 w
srcw              equ 22   * src width in bits             w
srcmodulo         equ 26   * src modulo in bytes           w
destdata          equ 28   * address of dest               l
destoff           equ 34   * destination h offset in bits  w
desth             equ 38   * destination height            w
destw             equ 42   * dest width in bits            w
destmodulo        equ 46   * dest modulo in bytes          w
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
destline          equ -2    * destination line #         (var)       w
vstep             equ -6    * vstep in COORD<<16         (const)     l
hstep             equ -10   * hstep in COORD<<16         (const)     l
srcline           equ -14   * source line in COORD<<16   (var)       l
srclineH          equ -14   * hi order of srcline.l      (**trick**) (w)
destoffsetbytes   equ -16   * dest offset in bytes       (const)     w
destoffsetbits    equ -18   * dest offset in bits        (const)     w
maxframe          equ -18   * stack frame size
*
*-----------------------------------------------------------------------
*
* Clipping boundaries
*
      XREF  _fitp_cliptop     * addr of first line not to clip on top
      XREF  _fitp_clipbot     * addr of first line to clip on bottom
*
*-----------------------------------------------------------------------
*
* Constants:
*
round       equ   $8000          * round fixed pt number to integer
*
*-----------------------------------------------------------------------

      XREF  _CXD22

      link     a6,#maxframe
      movem.l  d2-d7/a2-a5,-(a7)

      clr.l    d0                         * compute hstep =
      move.w   srcw(a6),d0                *  (srcw<<16) / destw
      swap     d0
      clr.l    d1
      move.w   destw(a6),d1
      jsr      _CXD22                     * d0(32)/d1(32) = d0(32) divide
      move.l   d0,hstep(a6)

      clr.l    d0                         * compute vstep =
      move.w   srch(a6),d0                *  (scrh<<16) / desth
      swap     d0
      clr.l    d1
      move.w   desth(a6),d1
      jsr      _CXD22                     * divide
      move.l   d0,vstep(a6)

      move.w   destoff(a6),d0             * compute dest offset in bytes
      move.w   d0,d1
      lsr.w    #3,d1
      andi.w   #$fffc,d1
      move.w   d1,destoffsetbytes(a6)
      andi.w   #31,d0
      eori.w   #31,d0                     * invert bit count
      move.w   d0,destoffsetbits(a6)

      move.w   destw(a6),d5               * save dest width for loop test

      movea.l  srcdata(a6),a4             * set up register constants
      movea.l  destdata(a6),a2
      move.l   hstep(a6),a3

      clr.w    destline(a6)
      clr.l    srcline(a6)
vlooptop:
      move.w   desth(a6),d0
      cmp.w    destline(a6),d0            * test for vertical loop
      ble      vloopend

   IFNE clipping
      cmpa.l   _fitp_cliptop,a2           * clipped on top
      blt      clipped
      cmpa.l   _fitp_clipbot,a2           * clipped on bottom
      bge      vloopend                   * (might as well stop)
   ENDC

differentline:
      move.w   srclineH(a6),d0            * no, so fit this line
      mulu     srcmodulo(a6),d0           * calculate src line start addr
      movea.l  a4,a1
      adda.l   d0,a1

      clr.l    d4                         * clear dest bit count
      move.w   destoffsetbytes(a6),d2
      move.w   destoffsetbits(a6),d3
      move.l   0(a2,d2.w),d7

      clr.l    d6
      move.w   srcoff(a6),d6
      swap     d6
      sub.l    a3,d6                * adjust for add below
hlooptop:
      cmp.w    d4,d5                * test for loop end
      ble.s    hloopend

      add.l    a3,d6                * get next source bit
      move.l   d6,d0
***      add.l    #round,d0
      swap     d0                   * integer part of fixed pt. number
      move.w   d0,d1
      lsr.w    #3,d1                * byte offset
      andi.w   #7,d0                * bit number
      eori.w   #7,d0                * invert bit count
      btst     d0,0(a1,d1.w)
      beq      notset
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
      move.l   vstep(a6),d0
      add.l    d0,srcline(a6)
      addq.w   #1,destline(a6)
      adda.w   destmodulo(a6),a2
      bra      vlooptop

vloopend:
      movem.l  (a7)+,d2-d7/a2-a5
      unlk     a6
      rts

