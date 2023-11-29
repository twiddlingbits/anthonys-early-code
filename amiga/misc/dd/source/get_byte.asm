*-----------------------------------------------------------------------
* get_byte
*-----------------------------------------------------------------------
* Calling sequence:
*   get_byte()
*
* Function:
*
*  returns a byte from the 2X disk drive
*
* buffer           equ 8    * address of source             l
*
port             equ   $bfe101
dir              equ   $bfe301
port2            equ   $BFD000
dir2             equ   $BFD200
*
*-----------------------------------------------------------------------

      XDEF  _get_byte
      XREF  _send_byte

      csect text

_get_byte
      link     a0,#0
      movem.l  a1/d1/d2,-(a7)

      movea.l  #port,a1
      move.b   #0,dir            * set port direction to read

retry:

      btst     #1,(a1)           * is clk bit set?
      beq      set

      jsr      clkeq             * bit 7
      jsr      clkne             * bit 6
      jsr      clkeq             * bit 5
      jsr      clkne             * bit 4
      jsr      clkeq             * bit 3
      jsr      clkne             * bit 2
      jsr      clkeq             * bit 1
      jsr      clkne             * bit 0
      jmp      done

set:  jsr      clkne
      jsr      clkeq
      jsr      clkne
      jsr      clkeq
      jsr      clkne
      jsr      clkeq
      jsr      clkne
      jsr      clkeq

done:
      moveq    #15,d1               * pause to let drive switch modes
pau:  dbra     d1,pau
      MOVE.L   D0,-(A7)             * echo char to drive
      move.w   d0,d1                * save
      JSR       _send_byte(PC)
      ADDQ.W    #4,A7
      move.w   d1,d0
      movem.l  (a7)+,a1/d1/d2
      unlk     a0
      rts

clkeq:
      move     #5,d2             * time out after approx 50 mico seconds
clkeq2:
      move.b   (a1),d1
      btst     #1,d1             * wait for clk to change
      beq      dat               * go if we got valid data
      dbra     d2,clkeq2
      addq.l   #4,sp             * throw away return address
      jmp      retry             * timed out

clkne:
      move     #5,d2             * time out after approx 50 micro seconds
clkne2:
      move.b   (a1),d1
      btst     #1,d1             * wait for clk to change
      bne      dat               * go if we got valid data
      dbra     d2,clkne2
      addq.l   #4,sp             * throw away return address
      jmp      retry             * timed out

dat:  roxr.b   #1,d1
      roxl.b   #1,d0                * shift bit into d0
      rts
      END
