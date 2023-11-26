*-----------------------------------------------------------------------
* lpack_screen.asm
*-----------------------------------------------------------------------
* Calling sequence:
*
*   lpack_screen (buffer, p0, p1, p2, p3)
*
* Function:
*
*     fast render of 4 bit interlaced data into 4 bitplanes
*
*
buffer           equ 8     * address of source             l
p0               equ 12    * address of source             l
p1               equ 16    * address of source             l
p2               equ 20    * address of source             l
p3               equ 24    * address of source             l
*
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _lpack_screen

      csect text

_lpack_screen

      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)


      movea.l  buffer(a0),a1
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5

      move     #199,d5             * number of lines to update - 1

main:
      moveq    #9,d7               * long words per line - 1
lines:
      moveq    #15,d6

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

      lsl.l    #1,d4
      roxl.b   #1,d0

      lsl.l    #1,d3
      roxl.b   #1,d0

      lsl.l    #1,d2
      roxl.b   #1,d0

      lsl.l    #1,d1
      roxl.b   #1,d0

      move.b   d0,(a1)+

      dbra     d6,loop
      dbra     d7,lines

      add.l    #40,a2               * skip interlaced field
      add.l    #40,a3
      add.l    #40,a4
      add.l    #40,a5
      dbra     d5,main


*
* now read in the interlaced field and interleve it into the bitplanes
*

      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5

      add.l    #40,a2               * skip interlaced field
      add.l    #40,a3
      add.l    #40,a4
      add.l    #40,a5

      move     #199,d5             * number of lines to update - 1

main2:
      moveq    #9,d7               * long words per line - 1

lines2:
      moveq    #15,d6

      move.l   (a2)+,d1
      move.l   (a3)+,d2
      move.l   (a4)+,d3
      move.l   (a5)+,d4

loop2: 
      lsl.l    #1,d4
      roxl.b   #1,d0

      lsl.l    #1,d3
      roxl.b   #1,d0

      lsl.l    #1,d2
      roxl.b   #1,d0

      lsl.l    #1,d1
      roxl.b   #1,d0

      lsl.l    #1,d4
      roxl.b   #1,d0

      lsl.l    #1,d3
      roxl.b   #1,d0

      lsl.l    #1,d2
      roxl.b   #1,d0

      lsl.l    #1,d1
      roxl.b   #1,d0

      move.b   d0,(a1)+

      dbra     d6,loop2
      dbra     d7,lines2

      add.l    #40,a2               * skip interlaced field
      add.l    #40,a3
      add.l    #40,a4
      add.l    #40,a5
      dbra     d5,main2

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts
      END

