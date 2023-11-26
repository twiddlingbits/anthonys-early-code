*-----------------------------------------------------------------------
* new_grab.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   new_grab (buffer)
*
* Function:
*
*     Puts 64K from par: into buffer
*
*
buffer           equ 8    * address of source             l
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _new_grab

      csect text

_new_grab
      link     a0,#0
      movem.l  a1/a2,-(a7)

      movea.l  #port,a1
      move.b   #0,dir                * set port direction to read
      move.b   #0,dir2
      movea.l  buffer(a0),a2

*wait: move.b   (a1),d0
*      btst     #7,d0
*      bne      wait
*      move.b   d0,(a2)+                * save 1st byte

wait: btst    #0,port2
      bne      wait

      move     #7999,d0                * (320 X 200)/8 - 1
*      bra      xx

loop: move.b   (a1),(a2)+
      nop
xx:   move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      nop
      move.b   (a1),(a2)+
      dbra     d0,loop

      move.b   (a1),d0
      nop
      move.b   (a1),d0
      nop
      move.b   (a1),d0

      movem.l  (a7)+,a1/a2
      unlk     a0
      rts
      END

