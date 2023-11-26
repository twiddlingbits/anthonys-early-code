*-----------------------------------------------------------------------
* get_byte
*-----------------------------------------------------------------------
* Calling sequence:
*   send_byte(val)
*
* Function:
*
*  sends a byte to the 2X disk drive
*
buffer           equ 10    * address of source             w
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _send_byte

      csect text

_send_byte:
      link     a0,#0
      movem.l  a1/d1/d2/d3,-(a7)

      movea.l  #port,a1
      move.b   #$FF,dir          * set port direction to write

      moveq.l  #7,d1             * send 8 bits
      move.w   buffer(a0),d0     * put byte to send into d0
      moveq    #0,d2             * d2 contains clk, high on entry to sub
loop: roxl.b   #1,d0             * put data bit into X flag
      ror.b    #1,d2             * move clk bit down to LSBit
      roxl.b   #1,d2             * move clk bit up and shift in data bit
      move.b   d2,(a1)           * send it to the port
      eori.b   #$ff,d2           * flip clock bit
      move.l   #40,d3
lp:   dbra     d3,lp             * wait for drive to process data bit
      dbra     d1,loop           * do all 8 bits

      move.b   #$ff,(a1)         * always leave all highs in port

      movem.l  (a7)+,a1/d1/d2/d3
      unlk     a0
      rts
      END

