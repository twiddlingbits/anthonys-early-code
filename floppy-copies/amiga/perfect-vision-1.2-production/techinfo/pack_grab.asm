*-----------------------------------------------------------------------
* pack_grab.asm  © 1988 SunRize Industries. All Rights Reserved.
*-----------------------------------------------------------------------
* Calling sequence:
*   pack_grab (buffer)
*
* Function:
*
*     Puts 64K from par: into buffer nyble packed
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

      XDEF  _pack_grab

      csect text

_pack_grab
      link     a0,#0
      movem.l  a1/a2/d1/d2,-(a7)

      movea.l  #port,a1
      move.b   #0,dir                * set port direction to read
      move.b   #0,dir2
      movea.l  buffer(a0),a2

*wait: move.b   (a1),d0
*      btst     #7,d0
*      bne      wait
*      move.b   d0,(a2)+                * save 1st byte

      move     #10000,d0                * don't sit in this loop forever
wait: btst     #0,port2
      dbeq     d0,wait
      addq     #1,d0
      bne      norm
      clr.l    d0                      * 0 on return means time out
      bra      done

norm:
      move     #31998,d0                * (320 X 200)/2 - 2

      move.b   (a1),d1                 * first byte is done outside loop
      lsl.b    #4,d1
      move.b   (a1),d2
      andi.b   #$f,d2
      or.b     d1,d2

loop: move.b   (a1),d1
      move.b   d2,(a2)+                * just processed two nybles
      lsl.b    #4,d1
      move.b   (a1),d2
      andi.b   #$f,d2
      or.b     d1,d2
      dbra     d0,loop
      move.b   d2,(a2)+                * finish of last byte

w1:   move.b   port,d0
      nop
      btst     #0,port2
      beq      w1

*      move.b   (a1),d0
*      nop
*      move.b   (a1),d0
*      nop
*      move.b   (a1),d0

      move.l   #1,d0                * 1 means normal grab
done:
      movem.l  (a7)+,a1/a2/d1/d2
      unlk     a0
      rts
      END

