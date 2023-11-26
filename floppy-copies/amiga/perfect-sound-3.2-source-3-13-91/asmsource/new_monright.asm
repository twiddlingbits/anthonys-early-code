* new_right.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   new_monright()
*
*-----------------------------------------------------------------------
*
* Function:
*
* Monitor from the right channel of a perfect sound digitizer 3.0.
* ticks is the count used by CIA chip to time sampling rate. The 8520
* (CIA) chip can be programed to produce a square wave on bit 6.  This
* square wave us used to clock the A/D in Perfect Sound.  This eliminates
* sample jitter common with other samplers.
*
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
doplot            equ   -12   * word
pending           equ   -14   * word
reserve           equ   -14   * amount of space to reserve
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

      XDEF  _new_monright
      XREF  _set_auto_gain

      XREF  _plane

      csect text

_new_monright:
      link     a0,#reserve
      movem.l  d2-d7/a2-a6,-(a7)
      move.l   a4,sav4(a0)
      move.l   a5,sav5(a0)

*
* build table of premultipled (16 --> -16) X 40 + _plane
* This table is indexed by taking a signed word that contains a sample
* between -128 and 127, and doing this:
*      lsr.w    #1,d2             * shift right 3 then X 4 because .l
*      and.w    #%01111100,d2
*      lea.l    mult_tab,a1
*      movea.l  0(a1,d2.w),a1     * a1 = _plane+(samp/8)*40
*
      lea      mult_tab,a1
      moveq.l  #0,d0
      move.l   #31,d2
buildtab:
      move.w   d0,d1
      lsl.b    #3,d1       * extend sign of 5 bit data to byte size data
      asr.b    #3,d1
      ext.w    d1          * extend sign of byte to word
      muls     #40,d1
      movea.l  _plane,a2         * plane=rp->BitMap->Planes[0]+40*REC_Y
      add.w    d1,a2
      move.l   a2,(a1)+
      addq.w   #1,d0
      dbra     d2,buildtab
*
*
      move.b   #$c0,dir1         * all read except PB6 & PB 7
      move.b   #$ff,data1
      move.b   #$c4,dir2         * pa2 write  (chanel sellect) ($c0 def)
      move.b   #4,data2          * right

      move.b   #35,talo          * 25 = approx. 14,000 samp/sec
      move.b   #0,tahi
      move.b   #7,cra            * for toggle use 7, 11 for pulse
      move.w   #1,aud1per        * Tell hardware speed we want
      move.w   #64,aud1vol       * set vol for monitor while recording

      lea      old,a2            * put in register to speed things up
      lea      intreq,a3         * ""
      move.w   #$100,d3          * "": $100=right, $80=left
      lea      temp(a0),a4       * a4 now points to a scratch word
      move.w   #0,(a4)           * clear
      move.w   #3,d4             * for fast ANDing below
      move.w   #128,d5           * for fast subracting below
      lea      data1,a5
      lea      data2,a6
      move.w   #6,d6             * for fast btst
      move.w   #0,doplot(a0)     * 0 means do real time graph
      move.w   #0,pending(a0)    * 0 means no graph changes pending
      move.w   #40,d7            * d7 is xpos in graph

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
      move.w   d2,aud1dat        * 8 byte twice.  a2 was aud1dat
      move.w   d3,(a3)           * 8 Reset audio interupt flag a3=intreq
                                 *    d3 = $100

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
      cmp.b    #183,d1           * g, toggle graph on/off
      beq      tog
      cmp.b    #190,d1           * a, auto gain set (was 191)
      beq      auto_gain
      bra      no_plot
tog:
      not.w   pending(a0)        * signal a pending graph state change
      cmp.w    #0,doplot(a0)     * 0 in doplot means plotting
      beq      no_plot           * turn off graph at end of sweep
      not.w    doplot(a0)        * turn on graph
      not.w    pending(a0)       * turn off pending graph turn off
      move.b   #35,talo          * slow down when graphing (35 = 10,222)
      bra      no_plot
downarrow:
      jsr      rest              * do some nop's
      move.b   #0,data2          *left
      jsr      rest
      bclr.b   #7,data1
      jsr      rest
      bset.b   #7,data1
      jsr rest
      move.b   #4,data2          *right
      jsr      rest      
      bra      no_plot
uparrow:                         * right should already be selected
      jsr      rest
      bclr.b   #7,data1
      jsr      rest
      bset.b   #7,data1
      jsr      rest
      bra      no_plot
auto_gain:
      move.b   #0,cra
      movem.l  a0/d3/a3/a4/a5,-(a7)       * save my values
      move.l   sav4(a0),a4       * get os values
      move.l   sav5(a0),a5
      jsr      _set_auto_gain    * call C routine
      movem.l  (a7)+,a0/d3/a3/a4/a5       * restore my values
      move.b   #4,data2          * RIGHT
      move.b   #7,cra
      bra      no_plot
nokey:
      cmp.w    #0,doplot(a0)     * No graphing (0=plot)
      bne      no_plot
      move.b   0(a2,d7.w),d2     * get sample from last pass thru graph
      jsr      unplot_pt         * erase plot of previous sample
      move.b   d1,0(a2,d7.w)     * store latest sample in circular buffer
      jsr      plot_pt           * plot current sample
no_plot:
      btst     #6,pa             * 40 left mouse button down?
      bne      loop              * 8

      move.l   #0,d0             * Return len in d0
      move.b   d0,cra            * turn off pulses

      move.w   #0,aud1vol        * turn volume totally off
      movem.l  (a7)+,d2-d7/a2-a6
      unlk     a0
      rts
*
* given a height in d1.w, signed, plot it at xpos
*

plot_pt:
      lsr.w    #1,d1             * shift right 3 then X 4 cause .l 
      and.w    #%01111100,d1
      lea.l    mult_tab,a1
      movea.l  0(a1,d1.w),a1     * a1 = _plane+(samp/8)*40

      move.w   d7,d0             * d7 contains xpos
      lsr.w    #3,d0             * convert pixels to bytes
      add.w    d0,a1
      move.w   d7,d1
      not.w    d1                * d1 = 7 - d1
      andi.w   #7,d1             * keep bit number only
      bset.b   d1,(a1)           * finally, set the bit!
      addq.w   #1,d7             * inc xpos
      cmp.w    #211,d7           * window is 40 to 210 pixels, inclusive
      bne      nowrap
      move.w   #40,d7
*
* at the end of every sweep of the graph, check the pending flag to
* see if the user wants the graph on or off. By only toggling the graph
* on or off at the end of a line, we avoid discontinuities in the graph.
*
      cmp.w    #0,pending(a0)    * 0 means no stop/start pending
      beq      nowrap
      not.w    pending(a0)       * clear pending toggle
      not.w    doplot(a0)        * and turn off graphing & speed samp rate
      move.b   #25,talo          * 25 = approx. 14,000 samp/sec
nowrap:
      rts

*
* given a height in d2.w, signed, erase it at xpos
*
unplot_pt:
      lsr.w    #1,d2             * shift right 3 then X 4 cause .l
      and.w    #%01111100,d2
      lea.l    mult_tab,a1
      movea.l  0(a1,d2.w),a1     * a1 = _plane+(samp/8)*40
      move.w   d7,d0
      lsr.w    #3,d0             * convert pixels to bytes
      add.w    d0,a1
      move.w   d7,d2
      not.w    d2                * d2 = 7 - d2
      andi.w   #7,d2             * keep bit number only
      bclr.b   d2,(a1)           * finally, clear the bit!
      rts

;
; I don't know why this is needed, but on '030 machines
; (and sometimes 68000's) you can't access the cia's
; consecutively

rest:
      nop
      nop
      nop
      nop
      nop
      nop
      rts

mult_tab:
      ds.l     32
old   ds.b     211               * Store prev samps here

      END

