*-----------------------------------------------------------------------
* grab_frame.asm
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
*
*-----------------------------------------------------------------------

      XDEF  _grab_frame

      csect text

_grab_frame:
      link     a0,#0
      movem.l  a1/a2,-(a7)

      movea.l  #port,a1
      move.w   #63999,d0                * 320 X 200 - 1
      move.b   #0,dir                   * set port direction to read
      movea.l  buffer(a0),a2
loop: move.b   (a1),(a2)+
      dbra     d0,loop

      movem.l  (a7)+,a1/a2
      unlk     a0
      rts
      END

