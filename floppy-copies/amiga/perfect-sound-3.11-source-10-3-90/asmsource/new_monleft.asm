* new_left.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   new_monleft()
*
*-----------------------------------------------------------------------
*
* Function:
*
* Monitor from the left channel of a perfect sound digitizer 3.0.
* ticks is the count used by CIA chip to time sampling rate.
* Call Disable()/Enable() around this routine.
*
*-----------------------------------------------------------------------
*
*
* reserved space on the stack
*
temp              equ   -2    * word
sav4              equ   -6    * long
sav5              equ   -10   * long
flags             equ   -12   * word
reserve           equ   -12    * amount of space to reserve
*-----------------------------------------------------------------------
*
talo              equ   $bfe401
tahi              equ   $bfe501
cra               equ   $bfee01
data1             equ   $bfe101
dir1              equ   $bfe301
icr               equ   $bfed01
sdr               equ   $bfec01
data2             equ   $bfd000
dir2              equ   $bfd200
pa                equ   $bfe001
base              equ   $DFF000
dmaconw           equ   base+$96
intreq            equ   base+$9C
intreqr           equ   base+$1e
aud0lch           equ   base+$A0          * left
aud0len           equ   base+$A4
aud0vol           equ   base+$A8
aud0per           equ   base+$A6
aud0dat           equ   base+$AA
aud0lc            equ   aud0lch
* channel 1 chip addresses
aud1lch           equ   base+$B0          * right
aud1len           equ   base+$B4
aud1vol           equ   base+$B8
aud1per           equ   base+$B6
aud1dat           equ   base+$BA
aud1lc            equ   aud1lch
*
*-----------------------------------------------------------------------

      XDEF  _new_monleft
      XREF  _rec_plot
      XREF  _set_auto_gain

      csect text

_new_monleft:
      link     a0,#reserve
      movem.l  d2-d6/a2-a6,-(a7)
      move.l   a4,sav4(a0)
      move.l   a5,sav5(a0)

      move.b   #$c0,dir1         * all read except PB6 & PB 7
      move.b   #$ff,data1
      move.b   #4,dir2           * pa2 write  (chanel sellect)
      move.b   #0,data2          * LEFT

      move.l   #25,d0
      move.b   d0,talo
      lsr.w    #8,d0
      move.b   d0,tahi
      move.b   #7,cra            * for toggle use 7, 11 for pulse
      move.w   #1,aud0per        * Tell hardware speed we want
      move.w   #64,aud0vol       * set vol for monitor while recording

      lea      aud0dat,a2        * put in register to speed things up
      lea      intreq,a3         * ""
      move.w   #$80,d3          * ""
      lea      temp(a0),a4       * a4 now points to a scratch word
      move.w   #0,(a4)           * clear
      move.w   #3,d4             * for fast ANDing below
      move.w   #128,d5           * for fast subracting below
      lea      data1,a5
      lea      data2,a6
      move.w   #6,d6             * for fast btst
      move.w   #0,flags(a0)      * 0 means do real time graph

*
* start digitizing
*

loop: move.b   (a5),d1           * 40 get data
      btst     d6,d1             * 6  d6 = #6
      bne      loop              * 10  wait for clk to go low
      move.b   (a6),d2           * 40 get low two bits
      lsl.b    #2,d1             * 10 move bits 7-2 to their correct position
      and.w    d4,d2             * 4 keep only lower two bits d4 = 3
      or.b     d1,d2             * 4 mix bits to create correct byte in d2
      sub.w    d5,d2             * 4 Turn into 2's complement  d5=128
      move.w   d2,d1             * copy into d1 for use below

lp2:  btst     #6,(a5)           * 40
      beq      lp2               * 10 wait for clk to return high

* let user hear sound while digitizing

      move.b   d2,(a4)           * 8 put byte into MSByte of a word
      move.w   (a4),d2           * 8 shifted low byte to high byte
      move.b   d1,d2             * 8 Let audio hardware play same
      move.w   d2,(a2)           * 8 byte twice.  a2 = aud0dat
      move.w   d3,(a3)           * 8 Reset audio interupt flag a3=intreq
                                 *    d3 = $80
*
* check and see if user pressed up or down arrow keys
*
      btst     #3,icr
      beq      nokey
      nop
      move.b   sdr,d1            * get key pressed
      nop
      ori.b     #$40,cra         * SP = output
      nop
      move.w   #300,d2           * was 250
pze:  nop                        * wait at least 75us
      dbra     d2,pze
      andi.b   #$bf,cra          * SP = input
      nop
      cmp.b    #$67,d1           * rotated rawkeys
      beq      uparrow
      cmp.b    #$65,d1           * rotated rawkeys
      beq      downarrow
      cmp.b    #183,d1            * g, toggle graph on/off
      beq      tog
      cmp.b    #190,d1            * a release, auto gain set
      beq      auto_gain
      bra      nokey
tog:
      not.w    flags(a0)
      bra      nokey
downarrow:
      bclr     #7,data1
      nop
      bset     #7,data1
      bra      nokey
uparrow:
      move.b   #4,data2          * right
      nop
      bclr     #7,data1
      nop
      bset     #7,data1
      nop
      move.b   #0,data2          * LEFT
      bra      nokey
auto_gain:
      move.b   #0,cra
      movem.l  a0/d3/a3/a4/a5,-(a7)       * save my values
      move.l   sav4(a0),a4       * get os values
      move.l   sav5(a0),a5
      jsr      _set_auto_gain    * call C routine
      movem.l  (a7)+,a0/d3/a3/a4/a5       * restore my values
      move.b   #0,data2          * LEFT
      move.b   #7,cra

nokey:
      *
      * every 100 samples, update graph
      *
      cmp.w    #0,flags(a0)     * No graphing (!0=plot)
      bne      no_plot
      dbra     d0,no_plot
      move.l   #0,d2
      move.b   (a4),d2           * get current digitizer value
      movem.l  a4/a5,-(a7)       * save my values
      move.l   sav4(a0),a4       * get os values
      move.l   sav5(a0),a5
      move.l   a0,-(a7)          * save
      move.l   d2,-(a7)          * pass parameter on stack
      jsr      _rec_plot         * call C routine to plot it
      add.l    #4,a7             * fix stack
      move.l   (a7)+,a0          * save
      movem.l  (a7)+,a4/a5       * restore my values
      move.w   #100,d0           * count 100 points till next update
no_plot:
      btst     #6,pa             * 40 left mouse button down?
      bne      loop              * 8

      move.l   #0,d0             * Return len in d0
      move.b   d0,cra            * turn off pulses

      movem.l  (a7)+,d2-d6/a2-a6
      unlk     a0
      rts
      END

