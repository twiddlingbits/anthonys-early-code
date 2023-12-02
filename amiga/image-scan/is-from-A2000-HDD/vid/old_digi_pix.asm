* digipix.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   digi_pix(count)
*
*-----------------------------------------------------------------------
*
* Function:
*
* Waits for a horizontal pulse, count "count" number of clock pulses,
* then digitizes a pixel
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
buffer            equ 8    * put sound data here           l
*
*-----------------------------------------------------------------------
*
icr               equ   $bfed01
data1             equ   $bfe101
dir1              equ   $bfe301
data2             equ   $bfd000
dir2              equ   $bfd200
pa                equ   $bfe001
base              equ   $DFF000
dmaconw           equ   base+$96
intreq            equ   base+$9C
intreqr           equ   base+$1e
*
*-----------------------------------------------------------------------
* data on CIAB , Port A
* PA0 - Horizontal Pulse Detect (Read only, reading clears)
* PA1 - Pixel Grabed Detect     (Read only, reading clears)
*
* all data is on CIAA (BFER01) on port B
* d0 - High selects A/D for Output, Low Selects counters load (Write only)
* d1 = Horiz pulse detect
* d2 - d7 A/D data (Read) and Count Set (write)

      XDEF  _digi_pix

READ  equ   $01
WRITE equ   $FD

      csect text

_digi_pix:
      link     a0,#0
      movem.l  a1-a6/d1-d7,-(a7)

      moveq.l  #1,d1               * d1 counts columns
*      move.b   #$10,icr            * turn OFF par: interupts

      move.b   icr,d0


col_loop:
      movea.l  buffer(a0),a1
      adda.l   d1,a1
      adda.l   d1,a1                * we're in 320 mode!
      move.l   #99,d2               * takes too Horiz pulse

* wait for the vertical pulse

      move.b   icr,d0
      nop
      nop
vert: move.b   icr,d0
      btst     #4,d0
      beq      vert

* Count out pulese Horiz pulses that occur during vertical retrace

      move.l   #21,d3
hc:   move.b   data1,d0             * Wait for horizonatl pulse
      btst     #1,d0
      beq      hc
      dbra     d3,hc

row_loop:

      move.b   #WRITE,dir1          * all lines write, except horiz. pulse
      move.l   d1,d0
      lsl.b    #2,d0                * use bits 7 - 2
      move.b   d0,data1             * set up for counters to load
                                    * (they load count automaticly when
                                    * horizonal pulse comes)
      nop
      nop
wait: move.b   data1,d0             * Wait for horizonatl pulse
      btst     #1,d0
      beq      wait

      move.b   #READ,dir1           * switch to read so we can read A/D
      move.b   #1,d0
      move.b   d0,data1             * Enable A/D outputs
      nop
      nop

wai2: move.b   data2,d0             * Wait for "finished conversion"
      btst     #1,d0
      beq      wai2

      move.b   data1,0(a1)
      nop
      nop
      move.l   #3,d3
delay:
      move.b   data1,d0             *delay
      nop
      dbra  d3,delay

      adda.l   #1280,a1
      dbra     d2,row_loop
      addq.l   #1,d1
      cmp      #60,d1
      bne      col_loop

*      move.w   #$0200,dmaconw    * Turn of all dma
*out:  move.w   #$8200,dmaconw    * Turn back on dma

      movem.l  (a7)+,a1-a6/d1-d7
      unlk     a0
      rts                           *returns with digitized value in d0
      END

