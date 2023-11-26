
*-----------------------------------------------------------------------
* invtextfit.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   invtextfit (srcdata,  srcoff,  srcmodulo, destoff);
*
*-----------------------------------------------------------------------
*
* Function:
*
*   Resizes the source bitmap to fit in the destination bitmap.
*   h is in lines* w is in bits
*   off is the bit offset from the start of the line to the start
*        of the image
*   modulo is the number of bytes on a physical line
*   Note: to start on other than the first line, adjust data to the
*        beginning of the starting line.
*   Global variables _fitp_ystep and _fitp_xstep must be set up prior
*        to call (=srch/desth or srcw/destw)
*   Srch and srcw are used only for calculating hstep and vstep.  Since
*        they are pre-calculated for this version, srch and srcw are not
*        needed and so are not passed.
*   _fitp_yfudge is the starting point for the y destination.  This fixes
*        the problem with chopping off descenders.  Ditto with _fitp_xfudge.
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
*   d7.l = hstep                                    (const)
*
*   a1 = src address of start of current line       (var)
*   a2 = dest address of start of current line      (var)
*   a3 = (unused)
*   a4 = original source address                    (const)
*   a5 = temporary                                  (temp)
*
*-----------------------------------------------------------------------
*
* Global Parameters:
*
      XREF  _fitp_destdata    ; address of dest                l
      XREF  _fitp_desth       ; destination height             w
      XREF  _fitp_destw       ; dest width in bits             w
      XREF  _fitp_destmodulo  ; dest modulo in bytes           w
      XREF  _fitp_italicshift ; italic shift (***not used***)  l
      XREF  _fitp_xstep       ; srcw/destw                     l
      XREF  _fitp_ystep       ; srch/desth                     l
      XREF  _fitp_yfudge      ; destination starting point     l
      XREF  _fitp_xfudge      ; dest starting point for x      l
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
srcdata           equ 8    * address of source             l
srcoff            equ 14   * source h offset in bits       w
srcmodulo         equ 18   * src modulo in bytes           w
destoff           equ 22   * destination h offset in bits  w
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
destline          equ -2    * destination line #         (var)       w
srcline           equ -6    * source line in COORD<<16   (var)       l
srclineH          equ -6    * hi order of srcline.l      (**trick**) (w)
destoffsetbytes   equ -8    * dest offset in bytes       (const)     w
destoffsetbits    equ -10   * dest offset in bits        (const)     w
maxframe          equ -10   * stack frame size
*
*-----------------------------------------------------------------------
*
     
      XDEF  _invtextfit
      XREF  _CXD22
     
      csect text
     
_invtextfit:
      link     a6,#maxframe
      movem.l  d2-d7/a1-a2/a4-a5,-(a7)
     
      move.w   destoff(a6),d0             * compute dest offset in bytes
      move.w   d0,d1
      lsr.w    #3,d1
      move.w   d1,destoffsetbytes(a6)
      andi.w   #7,d0
      eori.w   #7,d0                      * invert bit count
      move.w   d0,destoffsetbits(a6)
     
      move.w   _fitp_destw,d5             * save dest width for loop test
     
      movea.l  srcdata(a6),a4             * set up register constants
      movea.l  _fitp_destdata,a2
      move.l   _fitp_xstep,d7
     
      clr.w    destline(a6)
      move.l   _fitp_yfudge,srcline(a6)   * Fudge factor for descenders
vlooptop:
      move.w   _fitp_desth,d0
      cmp.w    destline(a6),d0            * test for vertical loop
      ble      vloopend
     
differentline:
      move.w   srclineH(a6),d0            * no, so fit this line
      mulu     srcmodulo(a6),d0           * calculate src line start addr
      movea.l  a4,a1
      adda.l   d0,a1
     
      moveq.l  #0,d4                      * clear dest bit count
      move.w   destoffsetbytes(a6),d2
      move.w   destoffsetbits(a6),d3
     
*      clr.l    d6
      move.w   srcoff(a6),d6
      swap     d6
      add.l    _fitp_xfudge,d6
      sub.l    d7,d6                * adjust for add below
hlooptop:
      cmp.w    d4,d5                * test for loop end
      ble.s    hloopend
     
      add.l    d7,d6                * get next source bit
      move.l   d6,d0
      swap     d0                   * integer part of fixed pt. number
      move.w   d0,d1
      lsr.w    #3,d1                * byte offset
      andi.w   #7,d0                * bit number
      eori.w   #7,d0                * invert bit count
      btst     d0,0(a1,d1.w)
      beq      notset
      bclr     d3,0(a2,d2.w)
notset:
      addq.w   #1,d4                * count next bit
      subq.w   #1,d3                * decrement bit count
      bpl.s    hlooptop
      moveq    #7,d3                * increment byte count
      addq.w   #1,d2
      bra      hlooptop
     
hloopend:
vloopbot:
      move.l   _fitp_ystep,d0
      add.l    d0,srcline(a6)
      addq.w   #1,destline(a6)
      adda.w   _fitp_destmodulo,a2
      bra      vlooptop
     
vloopend:
      movem.l  (a7)+,d2-d7/a1-a2/a4-a5
      unlk     a6
      rts
      END
     
