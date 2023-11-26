* new_right.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   new_right(buffer, maxsize, ticks)
*
*-----------------------------------------------------------------------
*
* Function:
*
* Digitizes from the right channel of a perfect sound digitizer 3.0.
* Stops when maxsize is reached or left mouse button is clicked.
* ticks is the count used by CIA chip to time sampling rate.
* Call Disable()/Enable() around this routine.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
buffer            equ 8    * put sound data here           l
maxsize           equ 12   * Max bytes to digitize         l
ticks             equ 18   * delay between each sample     w
*
* reserved space on the stack
*
temp              equ   -2    * word
reserve           equ   -2    * amount of space to reserve
*-----------------------------------------------------------------------
*
talo              equ   $bfe401
tahi              equ   $bfe501
cra               equ   $bfee01
data1             equ   $bfe101
dir1              equ   $bfe301
icr               equ   $bfed01
data2             equ   $bfd000
dir2              equ   $bfd200
pa                equ   $bfe001
base              equ   $DFF000
dmaconw           equ   base+$96
intreq            equ   base+$9C
intreqr           equ   base+$1e
aud0lch           equ   base+$A0
aud0len           equ   base+$A4
aud0vol           equ   base+$A8
aud0per           equ   base+$A6
aud0dat           equ   base+$AA
aud0lc            equ   aud0lch
* channel 1 chip addresses
aud1lch           equ   base+$B0
aud1len           equ   base+$B4
aud1vol           equ   base+$B8
aud1per           equ   base+$B6
aud1dat           equ   base+$BA
aud1lc            equ   aud1lch
*
*-----------------------------------------------------------------------

      XDEF  _new_right

      csect text

_new_right:
      link     a0,#reserve
      movem.l  d2-d6/a2-a6,-(a7)

      move.b   #$c0,dir1         * all read except PB6 & PB 7
      move.b   #$ff,data1
      move.b   #4,dir2           * pa2 write  (chanel sellect)
      move.b   #4,data2          * RIGHT

      move.w   ticks(a0),d0
      move.b   d0,talo
      lsr.w    #8,d0
      move.b   d0,tahi
      move.b   #7,cra            * for toggle use 7, 11 for pulse
      move.w   #1,aud1per        * Tell hardware speed we want
      move.w   #64,aud1vol       * set vol for monitor while recording

      move.l   maxsize(a0),d0    * d0 holds max samples
      movea.l  buffer(a0),a1     * a1 now holds buffer location

      lea      aud1dat,a2        * put in register to speed things up
      lea      intreq,a3         * ""
      move.w   #$100,d3          * ""
      lea      temp(a0),a4       * a4 now points to a scratch word
      move.w   #0,(a4)           * clear
      move.w   #3,d4             * for fast ANDing below
      move.w   #128,d5           * for fast subracting below
      lea      data1,a5
      lea      data2,a6
      move.w   #6,d6             * for fast btst

*loop: btst     #4,icr            * FLG - busy going low? (new sample start)
*      beq      loop              * up to 80

*
* first sync to clock, and start 1st sampe digitizing
*
s1:   btst     d6,(a5)           * wait for clk to go low
      bne      s1
s2:   btst     d6,(a5)           * wait for clk to go high
      beq      s2

*
* start digitizing, but check and make sure we arn't going two fast
* for this code.  If clk rate is two fast, jump to faster code
*

loop: btst     d6,(a5)           * is clk already low?
      beq      try3              * branches if "ticks" is smaller than 19
lop:  move.b   (a5),d1           * 40 get data
      btst     d6,d1             * 6  d6 = #6
      bne      lop               * 10  wait for clk to go low
      move.b   (a6),d2           * 40 get low two bits
      lsl.b    #2,d1             * 10 move bits 7-2 to their correct position
      and.w    d4,d2             * 4 keep only lower two bits d4 = 3
      or.b     d1,d2             * 4 mix bits to create correct byte in d2
      sub.w    d5,d2             * 4 Turn into 2's complement  d5=128
      move.b   d2,(a1)           * 8 Save In buffer for later

lp2:  btst     #6,(a5)           * 40
      beq      lp2               * 10 wait for clk to return high

* let user hear sound while digitizing

      move.b   d2,(a4)           * 8 put byte into MSByte of a word
      move.w   (a4),d2           * 8 shifted low byte to high byte
      move.b   (a1)+,d2          * 8 Let audio hardware play same
      move.w   d2,(a2)           * 8 byte twice.  a2 = aud1dat
      move.w   d3,(a3)           * 8 Reset audio interupt flag a3=intreq
                                 *    d3 = $100
      btst     #6,pa             * 40 left mouse button down?
      beq      out               * 8
      subq.l   #1,d0             * 8
      bne      loop              * 10
      bra      out

*
* The following code is used when above loop is two slow.
* speed is increased by not waiting for the digitizing pulse
* to go high then low.
*

try2: btst     d6,(a5)           * is clk already low?
      beq      try5              * branches if "ticks" is smaller than 19
try3: move.b   (a5),d1           * 40 get data
      btst     d6,d1             * 6  d6 = #6
      bne      try3              * 10
      move.b   (a6),d2           * 40 get low two bits
      lsl.b    #2,d1             * 10 move bits 7-2 to their correct position
      and.w    d4,d2             * 4 keep only lower two bits d4 = 3
      or.b     d1,d2             * 4 mix bits to create correct byte in d2
      sub.w    d5,d2             * 4 Turn into 2's complement  d5=128
      move.b   d2,(a1)           * 8 Save In buffer for later

* lp2:  btst     #6,(a5)           * 40
*      beq      lp2               * 10 wait for clk to return high

* let user hear sound while digitizing

      move.b   d2,(a4)           * 8 put byte into MSByte of a word
      move.w   (a4),d2           * 8 shifted low byte to high byte
      move.b   (a1)+,d2          * 8 Let audio hardware play same
      move.w   d2,(a2)           * 8 byte twice.  a2 = aud1dat
      move.w   d3,(a3)           * 8 Reset audio interupt flag a3=intreq
                                 *    d3 = $100
      btst     #6,pa             * 40 left mouse button down?
      beq      out               * 8
      subq.l   #1,d0             * 8
      bne      try2              * 10
      bra      out

*
* The following code is used when above loop is two slow.
* speed is increased by not playing the sound as we digitize.
* this code generaly executes when sampling above 30,000 samples/second
*

try4: btst     d6,(a5)           * is clk already low?
      beq      out               * branches if "ticks" is smaller than 19
try5: move.b   (a5),d1           * 40 get data
      btst     d6,d1             * 6  d6 = #6
      bne      try5              * 10
      move.b   (a6),d2           * 40 get low two bits
      lsl.b    #2,d1             * 10 move bits 7-2 to their correct position
      and.w    d4,d2             * 4 keep only lower two bits d4 = 3
      or.b     d1,d2             * 4 mix bits to create correct byte in d2
      sub.w    d5,d2             * 4 Turn into 2's complement  d5=128
      move.b   d2,(a1)+           * 8 Save In buffer for later

      btst     #6,pa             * 40 left mouse button down?
      beq      out               * 8
      subq.l   #1,d0             * 8
      bne      try4              * 10

out:  suba.l   buffer(a0),a1     * Calculate Sample length
      move.l   a1,d0             * Return len in d0
      move.b   #0,cra            * turn of pulses

      movem.l  (a7)+,d2-d6/a2-a6
      unlk     a0
      rts
      END

