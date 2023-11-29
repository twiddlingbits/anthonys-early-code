*-----------------------------------------------------------------------
* cycle.asm (displayed in interlaced)
*-----------------------------------------------------------------------
* Calling sequence:
*   cycle.asm (p0, p1, p2, p3)
*
* Function:
*
*     fast render of 6 bit data into 4 bitplanes
*
*
p0               equ 8     * dest display bitplane pointers
p1               equ 12
p2               equ 16
p3               equ 20
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _lace_cycle

      csect text

_lace_cycle
      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)

      movea.l  #port,a1
      move.b   #0,dir                * set port direction to read
      move.b   #0,dir2               * set port direction to read
      movea.l  p0(a0),a2
      movea.l  p1(a0),a3
      movea.l  p2(a0),a4
      movea.l  p3(a0),a5

* wait: move.b   port,d0               * high means digitizer is busy
*      btst     #7,d0
*      bne      wait

*      moveq    #31,d6
      move     #199,d5             * number of lines to update - 1
*      moveq    #9,d7               * long words per line - 1
*      bra      xx                  * process 1st data byte

wait  btst     #0,port2            * wait till busy goes low
      bne      wait

main:
      moveq    #9,d7               * long words per line - 1
lines:
      moveq    #31,d6
loop: 
      move.b   (a1),d0

*      btst     #7,d0      for auto sync
*      bne      wait

xx:   lsr      #1,d0                * put low order but into X
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

* wat2: move.b   port,d0               * high means digitizer is busy
*      btst     #7,d0
*      bne      wat2

wat   btst     #0,port2            * wait till busy goes low
      bne      wat

      move     #199,d5             * number of lines to update - 1

main2:
      moveq    #9,d7               * long words per line - 1
lines2:
      moveq    #31,d6
loop2: 
      move.b   (a1),d0

      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d1
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d2
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d3
      lsr      #1,d0                * put low order but into X
      roxl.l   #1,d4
      dbra     d6,loop2

      move.l   d1,(a2)+
      move.l   d2,(a3)+
      move.l   d3,(a4)+
      move.l   d4,(a5)+
      dbra     d7,lines2

      add.l    #40,a2               * skip interlaced field
      add.l    #40,a3
      add.l    #40,a4
      add.l    #40,a5
      dbra     d5,main2

      move.b   (a1),d0
      nop
      move.b   (a1),d0
      nop
      move.b   (a1),d0

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts
      END

