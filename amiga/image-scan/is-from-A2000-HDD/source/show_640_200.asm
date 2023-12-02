*-----------------------------------------------------------------------
* show_raw_640_200.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   show_frame (buffer, p0, p1, p2, p3, translation, height)
*
* Function:
*
*     fast render of 4 bit data into 4 bitplanes
*     assumes one pixel per byte
*
buffer           equ 8     * address of source             l
p0               equ 12    * address of source             l
p1               equ 16    * address of source             l
p2               equ 20    * address of source             l
p3               equ 24    * address of source             l
trans            equ 28    * address of translation table  l
height           equ 32    * # of lines - 1
*
*-----------------------------------------------------------------------

      XDEF  _show_raw_640_200

      csect text

_show_raw_640_200:

      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)

      move.l   height(a0),d5             * number of lines - 1
      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5
      movea.l  trans(a0),a6

      moveq    #0,d0

newrow:

      moveq    #19,d7

lword:
      moveq    #31,d6
loop:
      move.b   (a1)+,d0              * get byte
      move.b   0(a6,d0),d0

      lsl.b    #5,d0                * put high order bit into X
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

      dbra     d7,lword
      add.l    #640,a1
      dbra     d5,newrow

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts
      END

