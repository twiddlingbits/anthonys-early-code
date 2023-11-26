;
; reads data from serial A/D chip (LEFT channel).
; 'C' call format is:
; digitize (memptr, len)
; Written by Bobby Dean and Anthony Wood
; Last Update 1-11-87
;



pb_addr  equ   $BFE101

bit_data equ   0
bit_clk  equ   2
bit_cs   equ   3

memptr   equ   20              ; position on stack of allocated memory
len      equ   24              ; position on stack of length

readbit_l  MACRO                ;  *** Read a single bit ***
      move.b   (a0),d1        ; read data                8
      lsr.b    #1,d1          ; get data bit in X        8
      roxl.b   #1,d0          ; rotate it into d0        8
      move.b   d2,(a0)        ; set clock line           8
      move.b   d3,(a0)        ; clear clock line         8
      ENDM                                            ; 40


      XDEF  _recordleft

_recordleft:

      movem.l  d2-d4/a6,-(a7)

      move.l   #pb_addr,a0
      bclr     #bit_clk,(a0)
*      bclr     #bit_cs,(a0)   ; select the chip
      move.b   (a0),d2        ; set up registers for clock set/reset
      move.b   d2,d3
      moveq    #bit_clk,d1
      bset     d1,d2
      bclr     d1,d3

      move.l   len(a7),d4
      movea.l   memptr(a7),a6  ; a6 points to sound buffer

readloop:
      bclr  #bit_cs,(a0)
      nop
      nop
      nop
      nop
      readbit_l
      readbit_l
      readbit_l
      readbit_l
      readbit_l
      readbit_l
      readbit_l
      readbit_l
      bset    #bit_cs,(a0)
      subi.b  #128,d0         ; make 2's complement          16
      move.b  d0,(a6)+        ; save this byte to memory     12
      move.w  #12,d0                               ;         16
delay:
      dbf d0,delay
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      dbf d4,readloop                              ;         10

      bset     #bit_cs,(a0)   ; deselect the chip
      move.l memptr(a7),d0
      movem.l  (a7)+,d2-d4/a6    ; restore the registers
      rts

      end

