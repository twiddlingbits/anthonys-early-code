*-----------------------------------------------------------------------
* show_frame.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   show_frame (buffer, p0, p1, p2, p3)
*
* Function:
*
*     fast render of 4 bit data into 4 bitplanes
*     assumes nyble packed data
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

      XDEF  _show_frame

      csect text

_show_frame
      link     a0,#0
      movem.l  a1-a6/d1-d6,-(a7)

      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5

      move     #1999,d5             * number of bytes to update - 1

main:
      moveq    #15,d6
loop: 
      move.b   (a1)+,d0              * Get byte

      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d4
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d3
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d2
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d1
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d4
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d3
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d2
      lsl.b    #1,d0                * put high order bit into X
      roxl.l   #1,d1
      dbra     d6,loop

      move.l   d1,(a2)+
      move.l   d2,(a3)+
      move.l   d3,(a4)+
      move.l   d4,(a5)+
      dbra     d5,main

      movem.l  (a7)+,a1-a6/d1-d6
      unlk     a0
      rts
      END

