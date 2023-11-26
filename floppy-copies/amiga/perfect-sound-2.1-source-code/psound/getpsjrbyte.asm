;**************************************************************
; data = getpsjrbyte();
; UBYTE data;
;
; Read a value from the D/A converter and return it in D0
;
; Written by: Bobby Deen
;             Nov. 23, 1986
;**************************************************************

pb_addr  equ   $BFE101

bit_data equ   0
bit_clk  equ   2
bit_cs   equ   3

readbit  MACRO                ;  *** Read a single bit ***
      move.b   (a0),d1        ; read data
      lsr.b    #1,d1          ; get data bit in X
      roxl.b   #1,d0          ; rotate it into d0
      move.b   d2,(a0)        ; set clock line
      move.b   d3,(a0)        ; clear clock line
      ENDM


      XDEF  _getpsjrbyte

_getpsjrbyte:
      movem.w  d2/d3,-(a7)

      move.l   #pb_addr,a0
      bclr     #bit_cs,(a0)   ; select the chip
      move.b   (a0),d2        ; set up registers for clock set/reset
      move.b   d2,d3
      moveq    #bit_clk,d1
      bset     d1,d2
      bclr     d1,d3

readloop:
      readbit
      readbit
      readbit
      readbit
      readbit
      readbit
      readbit
      readbit

      bset     #bit_cs,(a0)   ; deselect the chip
      movem.w  (a7)+,d2/d3    ; restore the registers
      rts

      end

