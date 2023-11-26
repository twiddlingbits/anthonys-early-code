*
* listen.asm - get a byte from the digitizer and send it
*              to audio chanel 0
*
         XDEF     _listen

_listen:
*         move.l   (a1),a5     put address of is_Data in a5
         move.w   #0,d0       clear register
         move.b   $BFE101,d0  get audio sample
         subi     #128,d0
         move.w   d0,d1       copy to d1
         lsl.w    #8,d0       move lower byte to high word byte
         or.w     d1,d0       make a word with upper and lower bytes equal
         move.w   #$80,$9c(a0) clear interup flag
         move.w   d0,$aa(a0)  move to aud0dat
*         move.w   d0,(a5)
         rts


