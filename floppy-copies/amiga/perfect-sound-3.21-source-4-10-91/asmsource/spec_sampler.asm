* spec_sampler.asm
*-----------------------------------------------------------------------
* Calling sequence:
*  spec_sampler
*
*-----------------------------------------------------------------------
*
* Function:
*
* Determines the fastest the Perfect Sound can go
*
*
*
*-----------------------------------------------------------------------
*
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

      XDEF  _spec_sampler

      csect text

_spec_sampler
      link     a0,#0
      movem.l  d2-d6/a2-a6,-(a7)

      move.b   #$c0,dir1         * all read except PB6 & PB 7
      move.b   #$ff,data1        * bit 6 (RD\) high

      move.l   #0,d0             * clear d0
      move.b   #0,tahi           * load $00ff into timer
      move.b   #$ff,talo         *
      move.b   icr,d1            * clear any interupt flags
      move.b   #9,cra            * start timer & oneshot mode
   nop
      move.b   #$00,data1        * pb6 low, start conversion
   nop
      move.b   #$00,data1        * pb6 low, start conversion
   nop
      move.b   #$00,data1        * pb6 low, start conversion
   nop
      move.b   #$00,data1        * pb6 low, start conversion
   nop
      move.b   #$00,data1        * pb6 low, start conversion

*loop1 subq.l   #1,d0
*
*      btst.b   #4,icr            * FLG - busy going low? (new sample start)
*      beq.s    loop1

      move.b   talo,d0           * get count

      move.b   #$ff,data1        * pb6 high
      move.b   #0,cra            * turn off pulses

      movem.l  (a7)+,d2-d6/a2-a6
      unlk     a0
      rts
      END

