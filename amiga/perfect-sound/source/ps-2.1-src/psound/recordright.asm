;
; reads data from serial A/D chip (RIGHT channel).
; 'C' call format is:
; digitize (memptr, len)
; Written by Bobby Dean and Anthony Wood
; Last Update 1-11-87
;



pb_addr  equ   $BFE100       ; realy $BFE101 but we are using move.w

bit_data equ   0
bit_clk  equ   2
bit_cs   equ   3

memptr   equ   20              ; position on stack of allocated memory
maxlen   equ   24              ; position on stack of length
delay    equ   28              ; amount to pause between samples


      XDEF  _recordright

_recordright:

      movem.l  d2-d7/a4-a6,-(a7) ; restore the registers

      move.l   #pb_addr,a0
      bclr     #bit_cs,(a0)   ; select the chip
      move.w   (a0),d2        ; set up registers for clock set/reset
      move.w   d2,d3
      moveq    #bit_clk,d1
      bset     d1,d2
      bclr     d1,d3
      move.w   d2,a4
      move.w   d3,a5

      move.l   maxlen(a7),d4
      movea.l   memptr(a7),a6  ; a6 points to sound buffer

; wait for user to release the left mouse button

stilldown:

      btst #6,$BFE001         ; Is the mouse button clicked?
      beq  stilldown

readloop:

; get MSB

      move.w   (a0),d0        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 6
      move.w   (a0),d1        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 5
      move.w   (a0),d4        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 4
      move.w   (a0),d5        ; read data
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 3
      move.w   (a0),d6        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 2
      move.w   (a0),d7        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read bit 1
      move.w   (a0),d2        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; read LSB
      move.w   (a0),d3        ; read data                8
      move.w   a4,(a0)        ; set clock line           8
      move.w   a5,(a0)        ; clear clock line         8

; Now assemble bits into a byte                    total = 192

      lsr.b    #1,d1    ;bit 6                           8
      roxl.b   #1,d0                                    ;8

      lsr.b    #1,d4    ;bit 5
      roxl.b   #1,d0

      lsr.b    #1,d5    ;bit 4
      roxl.b   #1,d0

      lsr.b    #1,d6    ;bit 3
      roxl.b   #1,d0

      lsr.b    #1,d7    ;bit 2
      roxl.b   #1,d0

      lsr.b    #1,d2    ;bit 1
      roxl.b   #1,d0

      lsr.b    #1,d3    ;bit 0
      roxl.b   #1,d0                 ;total = 112


      subi.b  #128,d0         ; make 2's complement          16
      move.b  d0,(a6)+        ; save this byte to memory     12
*      move.l  delay(a7),d0    ; pause between samples
* pause:
*      dbf d0,pause

      btst #6,$BFE001         ; Is the mouse button clicked? 16
      beq  pressmouse                                     ;  8

      dbf d4,readloop                                     ;  10

pressmouse:                 ; total= 192 + 112 + 46 == 350

      bset     #bit_cs,(a0)      ; deselect the chip
      move.l   d4,d0             ; return "bytes left in buffer"
      movem.l  (a7)+,d2-d7/a4-a6 ; restore the registers
      rts

      end

