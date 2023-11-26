*-----------------------------------------------------------------------
* pack_screen_320_400.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   pack_screen (buffer, p0, p1, p2, p3)
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

      XDEF  _pack_screen_320_400

      csect text

_pack_screen_320_400
      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)

      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5
      move.l   #399,d5             * number of lines to update

newrow:

      moveq    #9,d7                * number of longwords per line

newlw:
      moveq    #31,d6

      move.l   (a2)+,d1
      move.l   (a3)+,d2
      move.l   (a4)+,d3
      move.l   (a5)+,d4

loop: 

      lsl.l    #1,d4
      roxl.b   #1,d0

      lsl.l    #1,d3
      roxl.b   #1,d0

      lsl.l    #1,d2
      roxl.b   #1,d0

      lsl.l    #1,d1
      roxl.b   #1,d0

      lsl.b    #4,d0

      move.b   d0,(a1)+               * save byte
      move.b   d0,(a1)+               * save byte  (low res done twice)

      dbra     d6,loop
      dbra     d7,newlw
      dbra     d5,newrow

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts
      END

