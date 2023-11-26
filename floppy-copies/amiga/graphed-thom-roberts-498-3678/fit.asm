;-----------------------------------------------------------------------
; fit.asm
;-----------------------------------------------------------------------
; Calling sequence:
;   fit (srcdata,  srcoff,  srch,  srcw,  srcmodulo,
;        destdata, destoff, desth, destw, destmodulo);
;
;-----------------------------------------------------------------------
;
; Function:
;
;   Resizes the source bitmap to fit in the destination bitmap.
;   h is in lines; w is in bits
;   off is the bit offset from the start of the line to the start
;       of the image
;   modulo is the number of bytes on a physical line
;   Note: to start on other than the first line, adjust data to the
;         beginning of the starting line.
;
;-----------------------------------------------------------------------
;
; Register Usage:
;
;   d0   = temporary                                (temp)
;   d1   = temporary                                (temp)
;   d2.w = dest offset in bytes                     (var)
;   d3.w = dest offset in bits                      (var)
;   d4.w = destw in bytes                           (const)
;   d5.w = destw in bits                            (const)
;   d6.l = src offset in fixed pt COORD<<16         (var)
;   d7.l = hstep                                    (const)
;
;   a1 = src address of start of current line       (var)
;   a2 = dest address of start of current line      (var)
;   a3 = dest address of start of prev line         (var)
;   a4 = original source address                    (const)
;   a5 = temporary                                  (temp)
;
;-----------------------------------------------------------------------
;
; Calling Parameters:
;
srcdata           equ 8    ; address of source             l
srcoff            equ 14   ; source h offset in bits       w
srch              equ 18   ; source height                 w
srcw              equ 22   ; src width in bits             w
srcmodulo         equ 26   ; src modulo in bytes           w
destdata          equ 28   ; address of dest               l
destoff           equ 34   ; destination h offset in bits  w
desth             equ 38   ; destination height            w
destw             equ 42   ; dest width in bits            w
destmodulo        equ 46   ; dest modulo in bytes          w
;
;-----------------------------------------------------------------------
;
; Temporary Variables on Stack:
;
*** oldsrcline        equ -2    ; old source line #          (var)       w
destline          equ -4    ; destination line #         (var)       w
vstep             equ -8    ; vstep in COORD<<16         (const)     l
hstep             equ -12   ; hstep in COORD<<16         (const)     l
srcline           equ -16   ; source line in COORD<<16   (var)       l
srclineH          equ -16   ; hi order of srcline.l      (**trick**) (w)
destoffsetbytes   equ -20   ; dest offset in bytes       (const)     w
destoffsetbits    equ -22   ; dest offset in bits        (const)     w
maxframe          equ -22   ; stack frame size
;
;-----------------------------------------------------------------------
;
; Constants:
;
round       equ   $8000          ; round fixed pt number to integer
;
;-----------------------------------------------------------------------

      XDEF  _fit
      XREF  _CXD22

_fit:
      link     a6,#maxframe
      movem.l  d2-d7/a1-a5,-(a7)

      clr.l    d0                         ; compute hstep =
      move.w   srcw(a6),d0                ;  (srcw<<16) / destw
      swap     d0
      clr.l    d1
      move.w   destw(a6),d1
      jsr      _CXD22                     ; d0(32)/d1(32) = d0(32) divide
      move.l   d0,hstep(a6)

      clr.l    d0                         ; compute vstep =
      move.w   srch(a6),d0                ;  (scrh<<16) / desth
      swap     d0
      clr.l    d1
      move.w   desth(a6),d1
      jsr      _CXD22                     ; divide
      move.l   d0,vstep(a6)

      move.w   destoff(a6),d0             ; compute dest offset in bytes
      move.w   d0,d1
      lsr.w    #3,d1
      move.w   d1,destoffsetbytes(a6)
      andi.w   #7,d0
      move.w   d0,destoffsetbits(a6)

      move.w   destw(a6),d0               ; compute dest width in bytes
      add.w    destoff(a6),d0             ; add in destination offset
      move.w   d0,d1
      lsr.w    #3,d1
      move.w   d1,d4                      ; destw in bytes
      andi.w   #7,d0
      move.w   d0,d5                      ; destw in leftover bits
      eori.w   #7,d5                      ; invert bit count

      movea.l  srcdata(a6),a4             ; set up register constants
      movea.l  destdata(a6),a2
      move.l   hstep(a6),d7

***      move.w   #-1,oldsrcline(a6)         ; initialize vertical loop
      clr.w    destline(a6)
      clr.l    srcline(a6)
vlooptop:
      move.w   desth(a6),d0
      cmp.w    destline(a6),d0            ; test for vertical loop
      ble      vloopend

***      move.w   srclineH(a6),d0
***      cmp.w    oldsrcline(a6),d0          ; is this the same source line?
***      bne.s    differentline

***      move.w   destmodulo(a6),d0          ; yes, copy the prev dest line
***      subq.w   #1,d0
***      movea.l  a2,a5
***copyloop:
***      move.b   (a3)+,(a5)+
***      dbra     d0,copyloop
***      bra.s    vloopbot

differentline:
      move.w   srclineH(a6),d0            ; no, so fit this line
      mulu     srcmodulo(a6),d0           ; calculate src line start addr
      movea.l  a4,a1
      adda.l   d0,a1

      move.w   destoffsetbytes(a6),d2
      move.w   destoffsetbits(a6),d3
      eori.w   #7,d3                ; invert bit count
      clr.l    d6
      move.w   srcoff(a6),d6
      swap     d6
      sub.l    d7,d6                ; adjust for add below
hlooptop:
      cmp.w    d2,d4                ; test for loop end
      bgt.s    .1
      cmp.w    d3,d5
      bge.s    hloopend

.1    add.l    d7,d6                ; get next source bit
      move.l   d6,d0
***      add.l    #round,d0
      swap     d0                   ; integer part of fixed pt. number
      move.w   d0,d1
      lsr.w    #3,d1                ; byte offset
      andi.w   #7,d0                ; bit number
      eori.w   #7,d0                ; invert bit count
      btst.b   d0,0(a1,d1.w)
      beq      notset
      bset.b   d3,0(a2,d2.w)
notset:
      subq.w   #1,d3                ; decrement bit count
      bpl.s    hlooptop
      moveq    #7,d3                ; increment byte count
      addq.w   #1,d2
      bra      hlooptop

hloopend:
vloopbot:
***      move.w   srclineH(a6),oldsrcline(a6)
      move.l   vstep(a6),d0
      add.l    d0,srcline(a6)
      addq.w   #1,destline(a6)
      move.l   a2,a3
      adda.w   destmodulo(a6),a2
      bra      vlooptop

vloopend:
      movem.l  (a7)+,d2-d7/a1-a5
      unlk     a6
      rts

