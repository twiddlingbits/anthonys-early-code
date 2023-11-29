*-----------------------------------------------------------------------
* slow_cycle.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   cycle.asm (p0, p1, p2, p3)
*
* Function:
*
*     fast render of 6 bit data into 4 bitplanes
*
*
p0               equ 8    * address of source             l
p1               equ 12    * address of source             l
p2               equ 16    * address of source             l
p3               equ 20    * address of source             l
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _slow_cycle

      csect text

_slow_cycle
      link     a0,#0
      movem.l  a1-a6/d1-d6,-(a7)

      movea.l  #port,a1
      move.b   #0,dir                * set port direction to read
      move.b   #0,dir2               * set port direction to read
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5

wait: move.b   port2,d0
      btst     #1,d0
      bne      wait


       move     #1999,d5             * number of bytes to update - 1

main:
      moveq    #31,d6
loop: 
      move.b   port2,d0
      btst     #1,d0
      bne      wait

      move.b   (a1),d0
      lsr      #3,d0                * put low order but into X
      roxl.l   #1,d1
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d2
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d3
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d4
      dbra     d6,loop

      move.l   d1,(a2)+
      move.l   d2,(a3)+
      move.l   d3,(a4)+
      move.l   d4,(a5)+
      dbra     d5,main

*      move.l   #64000,d0
*xx:   nop
*      nop
*      nop
*      dbra     d0,xx                 * wait 6 ms

      movem.l  (a7)+,a1-a6/d1-d6
      unlk     a0
      rts
      END

