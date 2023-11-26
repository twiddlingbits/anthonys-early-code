*-----------------------------------------------------------------------
* show.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   show.asm (buffer, p0, p1, p2, p3)
*
* Function:
*
*     fast render of 6 bit data into 4 bitplanes
*
*
buffer           equ 8     * address of source             l
p0               equ 12    * address of source             l
p1               equ 16    * address of source             l
p2               equ 20    * address of source             l
p3               equ 24    * address of source             l
*
port             equ   $bfe101
dir              equ   $bfe301
*
*-----------------------------------------------------------------------

      XDEF  _show

      csect text

_show
      link     a0,#0
      movem.l  a1-a6/d1-d2,-(a7)

      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5
      clr.l    d0
      move.l   #7,d1                * bit position in planes
      move     #63999,d2            * number of bytes to update - 1

loop: 
      move.b   (a1)+,d0
      andi.b   #$3C,d0              * drop bottom two data bits
      lea      jtab,a6
      movea.l  0(a6,d0),a6
      jmp      (a6)


b1:   bset     d1,(a2)
      bra      next
b2:   bset     d1,(a3)
      bra      next
b3:   bset     d1,(a2)
      bset     d1,(a3)
      bra      next
b4:   bset     d1,(a4)
      bra      next
b5:   bset     d1,(a2)
      bset     d1,(a4)
      bra      next
b6:   bset     d1,(a3)
      bset     d1,(a4)
      bra      next
b7:   bset     d1,(a2)
      bset     d1,(a3)
      bset     d1,(a4)
      bra      next
b8:   bset     d1,(a5)
      bra      nex2
b9:   bset     d1,(a2)
      bra      nex2
b10:  bset     d1,(a3)
      bra      nex2
b11:  bset     d1,(a2)
      bset     d1,(a3)
      bra      nex2
b12:  bset     d1,(a4)
      bra      nex2
b13:  bset     d1,(a2)
      bset     d1,(a4)
      bra      nex2
b14:  bset     d1,(a3)
      bset     d1,(a4)
      bra      nex2
b15:  bset     d1,(a2)
      bset     d1,(a3)
      bset     d1,(a4)
nex2: bset     d1,(a5)
next:
b0:   subq     #1,d1
      andi     #7,d1
      cmp      #7,d1
      bne      skip
      addq.l   #1,a2
      addq.l   #1,a3
      addq.l   #1,a4
      addq.l   #1,a5
skip: dbra     d2,loop

      movem.l  (a7)+,a1-a6/d1-d2
      unlk     a0
      rts

jtab: dc.l  b0
      dc.l  b1
      dc.l  b2
      dc.l  b3
      dc.l  b4
      dc.l  b5
      dc.l  b6
      dc.l  b7
      dc.l  b8
      dc.l  b9
      dc.l  b10
      dc.l  b11
      dc.l  b12
      dc.l  b13
      dc.l  b14
      dc.l  b15
      END

