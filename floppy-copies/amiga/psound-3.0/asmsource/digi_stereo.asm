* digi_stereo.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   digi_stereo(lp, rp, maxsize, period)
*
*-----------------------------------------------------------------------
*
* Function:
*
* Digitizes from both channels of a perfect sound digitizer (stereo).
* Stops when maxsize is reached or left mouse button is clicked.
* Period is the amiga hardware period that describes the speed to
* Digitize at. This routine uses the audio hardware's interupt
* mechanisim to time the reading of Perfect Sound. You must call
* Disable()/Enable() around this routine.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
lp                equ 8    * put sound data here (left)    l
rp                equ 12   * right channel buffer          l
maxsize           equ 16   * Max bytes to digitize         l
period            equ 22   * delay between each sample     w
*
*-----------------------------------------------------------------------
*
data1             equ   $bfe101
dir1              equ   $bfe301
data2             equ   $bfd000
dir2              equ   $bfd200
pa                equ   $bfe001
base              equ   $DEF000
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

      XDEF  _digi_stereo

      csect text

_digi_stereo:
      link     a0,#0
      move.l   a2,-(a7)

      move.b   #0,dir1           * all lines read
      move.b   #0,data1
      move.b   #6,dir2           * pa1 and pa2 write
      move.b   #2,data2          * left

      move.w   #$0200,dmaconw    * Turn of all dma

      move.w   period(a0),d0     * Get desired period
      lsr.w    #1,d0             * divide by 2
      move.w   d0,aud1per        * Tell hardware speed we want
      move.w   #0,aud1vol        * set vol for monitor while recording
      move.w   #0,aud1dat        * Start timer
      move.w   #$100,intreq      * Reset audio interupt flag

      move.l   maxsize(a0),d0    * d0 holds max samples
      movea.l  rp(a0),a1         * a1 now holds right buffer location
      movea.l  lp(a0),a2         * a2 points to left buffer

loop: 
*      move.w   period(a0),d0     * Get pause factor
*      sub.w    #3,d0             * We arelready did some delay'in
* xx:   dbra     d0,xx             * Let right channel signal settle

      moveq    #00,d1            * 4   Need a word to subtract
      move.b   data1,d1          * Get left chan data, start Right
      subi.w   #128,d1           * 8   Turn into 2's complement
      move.b   d1,(a2)+          * 8   Save In right buffer
      lsr      #8,d1
      lsr      #8,d1
      lsr      #8,d1
      lsr      #8,d1

      move.b   #2,data2          * pick LEFT channel

wait: btst     #0,intreqr        * Wait for hardware to signal time
      beq      wait              * For another word of data

      move.w   #0,aud1dat        * Start timer (that we wait for above)
      move.w   #$100,intreq      * Reset audio interupt flag

*      move.w   period(a0),d0     * Get pause factor
*      move.w   #2,d0             * Get pause factor
* yy:   dbra     d0,yy             * Let left channel signal settle

      moveq    #00,d1            * 4   Need a word to subtract
      move.b   data1,d1          * Get right chan, start left
      subi.w   #128,d1           * 8   Turn into 2's complement
      move.b   d1,(a1)+          * 8   Save In right buffer

      lsr      #8,d1
      lsr      #8,d1
      lsr      #8,d1
      lsr      #8,d1

      move.b   #4,data2          * pick RIGHT channel

      btst     #6,pa             * 12 left button cliked?
      beq      out               * 10/ 12 combine these into one db_?

      subq.l   #1,d0             * 8
      bne      loop              * 10/ 8
*                                  total = 104 + 10*delay
out:  move.w   #$8200,dmaconw    * Turn back on dma
      suba.l   rp(a0),a1         * Calculate Sample length
      move.l   a1,d0             * Return len in d0

      move.l   (a7)+,a2
      unlk     a0
      rts
      END

