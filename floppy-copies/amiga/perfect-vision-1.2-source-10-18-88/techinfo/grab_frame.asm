*-----------------------------------------------------------------------
* grab_frame.asm  © 1988 SunRize Industries.  All Rights Reserved.
*-----------------------------------------------------------------------
* Calling sequence:
*   grab_frame (buffer)
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

      XDEF  _grab_frame

      csect text

_grab_frame
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

      move     #10000,d0                * don't sit in this loop forever
wait: btst     #0,port2
      dbeq     d0,wait
      addq     #1,d0
      bne      norm
      clr.l    d0                      * 0 on return means time out
      bra      done

norm:
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

w1:   move.b   (a1),d0
      btst     #0,port2
      beq      w1

*      move.b   (a1),d0
*      nop
*      move.b   (a1),d0
*      nop
*      move.b   (a1),d0

      move.l   #1,d0                * 1 means normal grab
done:
      movem.l  (a7)+,a1/a2
      unlk     a0
      rts
      END

