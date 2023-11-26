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
      move.b   #4,data2          * RIGHT

      move.w   #$0200,dmaconw    * Turn of all dma
      move.w   period(a0),d0     * Get desired period
      lsr.w    #2,d0             * divide by 4
      move.w   d0,aud1per        * Tell hardware what speed we want
      move.w   #64,aud1vol       * set vol for monitor while recording

      move.w   #1,aud0per        * left channel is "free running"
      move.w   #64,aud0vol       * left channel

      move.l   maxsize(a0),d0    * d0 holds max samples
      movea.l  rp(a0),a1         * a1 now holds right buffer location
      movea.l  lp(a0),a2         * a2 points to left buffer

loop: 
      move.b   #2,data2          * pick LEFT channel
      lsl      #8,d1             * pause
      lsl      #8,d1             * pause
      moveq    #00,d1            * 4   Need a word to subtract
      move.b   data1,d1          * 16  Get byte (0-255) from par:
      subi.l   #128,d1           * 8   Turn into 2's complement
      move.b   d1,(a1)           * 8   Save In right buffer

* let user hear sound while digitizing

      lsl      #8,d1             * Make a word with two identical
      move.b   (a1)+,d1          * bytes. Let audio hardware play same
      move.w   d1,aud1dat        * byte twice.
      move.w   #$100,intreq      * Reset audio interupt flag

wat2: btst     #0,intreqr        * Wait for hardware to signal time
      beq      wat2              * For another word of data

* Now do left channel

      move.b   #4,data2          * pick RIGHT channel
      move.w   d1,aud1dat        * triger next interupt
      move.w   #$100,intreq      *
      nop                        * pause
*      lsl      #8,d1             * pause

      btst     #6,pa             * 12 left button cliked?
      beq      out               * 10/ 12 combine these into one db_?

      moveq    #00,d1            * 4   Need a word to subtract
      move.b   data1,d1          * 16  Get byte (0-255) from par:
      subi.l   #128,d1           * 8   Turn into 2's complement
      move.b   d1,(a2)           * 8   Save In left buffer

* let user hear sound while digitizing

      lsl      #8,d1             * Make a word with two identical
      move.b   (a2)+,d1          * bytes.
      move.w   d1,aud0dat        * Play left channel now
      move.w   #$100,intreq      * Reset audio interupt flag

wait: btst     #0,intreqr        * Wait for hardware to signal time
      beq      wait              * For another word of data

*      btst     #6,pa             * 12 left button cliked?
*      beq      out               * 10/ 12 combine these into one db_?
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

