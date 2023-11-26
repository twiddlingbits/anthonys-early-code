*-----------------------------------------------------------------------
* show32.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   show.asm (buffer, p0, p1, p2, p3)
*
* Function:
*
*     fast render of 5 bit data into 5 bitplanes
*
*
buffer           equ 8     * address of source             l
p0               equ 12    * address of source             l
p1               equ 16    * address of source             l
p2               equ 20    * address of source             l
p3               equ 24    * address of source             l
p4               equ 28    * address of source             l
*
*
port             equ   $bfe101
dir              equ   $bfe301
*
*-----------------------------------------------------------------------

      XDEF  _show32

      csect text

_show32
      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)

      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5
      movea.l  p3(a0),a6

      move     #1999,d5             * number of bytes to update - 1

main:
      moveq    #31,d6
loop: 
      move.b   (a1)+,d0
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d1
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d2
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d3
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d4
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d7
      dbra     d6,loop

      move.l   d1,(a2)+
      move.l   d2,(a3)+
      move.l   d3,(a4)+
      move.l   d4,(a5)+
      move.l   d7,(a6)+
      dbra     d5,main

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts
      END

