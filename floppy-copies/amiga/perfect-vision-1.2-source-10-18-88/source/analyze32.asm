*-----------------------------------------------------------------------
* disp_line.asm
* disp_noreg.asm
*-----------------------------------------------------------------------
*
* Five different modules are created from this.  See the XDEF section for
* filenames, etc.  That's all combinations of (analyze,draw) and
* (hamreg,hamnoreg,noham) except for analyze+hamnoreg which is not needed.
*
* analyze & draw are mutually exclusive
* hamreg, hamnoreg, and noham are also mutex
* usebyte, usenopack, usepack are also mutex
* uselut can go with any of the other three use's (one of the three must
*                                                  be set)
*
analyze     equ   1     1 iff analyze phase  (not needed with hamnoreg)
draw        equ   0     1 iff drawing phase
*
hamreg      equ   0     1 iff use ham mode with registers      (disp_line)
hamnoreg    equ   0     1 iff use ham mode without registers   (disp_noreg)
noham       equ   1     1 iff 32 color (non-ham) mode          (disp_line32)
*
uselut      equ   1     1 iff samples should go through the lut
*                             (available with any of the modes below)
usebyte     equ   0     1 iff samples are byte wide
usenopack   equ   0     1 iff samples are one per byte, top 4 bits
usepack     equ   1     1 iff samples are packed two per byte (lo first)
*
ham         equ   hamreg+hamnoreg  1 iff either hamreg or hamnoreg are set
*
*-----------------------------------------------------------------------
* Calling sequence:
*   disp_analyze(rbuf, gbuf, bbuf);
*   disp_analyze32(rbuf, gbuf, bbuf);
*   disp_32(rbuf, gbuf, bbuf, planes, addr);
*   disp_line(rbuf, gbuf, bbuf, planes, addr);
*   disp_noreg(rbug, gbuf, bbuf, planes, addr);
*
*-----------------------------------------------------------------------
*
* Function:
*
*   Display a line using delta values with color registers for disp_noreg,
*   and without color registers for disp_line.  Line goes in Planes *planes,
*   address offset 'addr'.  Addr is updated to point past the line.  Lines
*   (rbuf, etc.) are byte fmt.
*   Also analyzes the lines prior to displaying them by accumulating the
*   delta values.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
rbuf              equ 8    red buffer                 l
gbuf              equ 12   green buffer               l
bbuf              equ 16   blue buffer                l
planes            equ 20   pointer to RastPort        l
addr              equ 24   address offset into rp     l
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
*
*-----------------------------------------------------------------------
*
* Constants:
*
counter  equ   -2                      word
red      equ   -3                      byte
green    equ   -4                      byte
blue     equ   -5                      byte
maxcolor equ   -6
maxframe equ   -6
*
*-----------------------------------------------------------------------

   ifne ham
      XREF  _writepixels                              extern routine
   endc
   ifne noham
      XREF  _writepixels32
   endc
      XREF  _colormapr,_colormapg,_colormapb          byte *
      XREF  _colordifftbl,_colorregtbl                byte *
      XREF  _lutred,_lutgreen,_lutblue                byte *
      XREF  _nsamps                                   int
      XREF  _savearea                                 byte *
      XREF  _colordelta                               int *

      csect text

   ifne  analyze
     ifne ham
      XDEF  _disp_analyze
_disp_analyze
     endc
     ifne noham
      XDEF  _disp_analyze32
_disp_analyze32
     endc
   endc
   ifne draw
     ifne  hamreg
      XDEF  _disp_line
_disp_line
     endc
     ifne  hamnoreg
      XDEF  _disp_noreg
_disp_noreg
     endc
     ifne  noham
      XDEF  _disp_32
_disp_32
     endc
   endc


      link     a6,#maxframe
      movem.l  d0-d7/a4-a5,-(a7)

   ifne  draw
      lea      _savearea,a4
      move.l   a4,a5
      add.l    #32,a5
   endc

   ifne analyze
      lea      _colordelta,a1
   endc

      moveq    #0,d4                i  (counter)

   ifne  ham
      clr.b    red(a6)
      clr.b    green(a6)
      clr.b    blue(a6)
   endc

samploop:
      move.w   d4,counter(a6)

   ifne uselut
      moveq    #0,d5
      moveq    #0,d6
      moveq    #0,d7
   endc

   ifne usebyte
      move.l   rbuf(a6),a0
      move.b   0(a0,d4.w),d5
      move.l   gbuf(a6),a0
      move.b   0(a0,d4.w),d6
      move.l   bbuf(a6),a0
      move.b   0(a0,d4.w),d7
   endc

   ifne usenopack
      move.l   rbuf(a6),a0
      move.b   0(a0,d4.w),d5
      lsr.b    #4,d5
      move.l   gbuf(a6),a0
      move.b   0(a0,d4.w),d6
      lsr.b    #4,d6
      move.l   bbuf(a6),a0
      move.b   0(a0,d4.w),d7
      lsr.b    #4,d7
   endc

   ifne usepack
      lsr.w    #1,d4
      bcc      topnibble        use lower half of byte on odd count

      move.l   rbuf(a6),a0      use lower half
      move.b   0(a0,d4.w),d5
      and.b    #$0F,d5
      move.l   gbuf(a6),a0
      move.b   0(a0,d4.w),d6
      and.b    #$0F,d6
      move.l   bbuf(a6),a0
      move.b   0(a0,d4.w),d7
      and.b    #$0F,d7
      bra      unpackdone

topnibble:
      move.l   rbuf(a6),a0      use upper half
      move.b   0(a0,d4.w),d5
      lsr.b    #4,d5
      move.l   gbuf(a6),a0
      move.b   0(a0,d4.w),d6
      lsr.b    #4,d6
      move.l   bbuf(a6),a0
      move.b   0(a0,d4.w),d7
      lsr.b    #4,d7
unpackdone:
   endc

   ifne uselut
      add.l    #_lutred,d5
      move.l   d5,a0
      move.b   (a0),d5              rpix
      add.l    #_lutgreen,d6
      move.l   d6,a0
      move.b   (a0),d6              gpix
      add.l    #_lutblue,d7
      move.l   d7,a0
      move.b   (a0),d7              bpix
   endc

*-----------------------------------------------------------------------
* Done getting the pixel values, now use them
*-----------------------------------------------------------------------

 ifne ham
      move.b   #$20,maxcolor(a6)

   ifne  hamreg
      moveq    #0,d1                calculate the diff
   endc
      move.b   red(a6),d2
      sub.b    d5,d2                red-rpix
      bpl.s    s1
      neg.b    d2
s1:
   ifne  hamreg
      add.b    d2,d1
   endc
      move.b   green(a6),d3
      sub.b    d6,d3                green-gpix
      bpl      s2
      neg.b    d3
s2:
   ifne  hamreg
      add.b    d3,d1
   endc
      cmp.b    d2,d3                find max diff
      ble      s20
      move.b   d3,d2
      move.b   #$30,maxcolor(a6)
s20:  move.b   blue(a6),d3
      sub.b    d7,d3                blue-bpix
      bpl      s3
      neg.b    d3
s3:
   ifne  hamreg
      add.b    d3,d1
   endc
      cmp.b    d2,d3
      ble      s30
      move.b   d3,d2
      move.b   #$10,maxcolor(a6)
s30:
   ifne  hamreg
      sub.b    d2,d1          sub max diff since it'll be set through HAM
   endc
 endc

   ifne hamreg+noham
      clr.w    d0
      move.b   d5,d0                rpix
      lsl.b    #4,d0
      or.b     d6,d0                gpix
      lsl.w    #4,d0
      or.b     d7,d0                bpix
   endc

   ifne  analyze
      lsl.w    #2,d0                * 4 for long index
     ifne ham
      add.l    d1,0(a1,d0.w)        accumulate delta value
     endc
     ifne noham
      addq.l   #1,0(a1,d0.w)        increment frequency value
     endc
   endc

 ifne draw
   ifne noham
      lea      _colorregtbl,a0
      move.b   0(a0,d0.w),d0        save color register
      and.w    #$001F,d0
      bra      plot
   endc
   ifne hamreg
      lea      _colordifftbl,a0
      cmp.b    0(a0,d0.w),d1
      ble      usenoreg             test > old
      lea      _colorregtbl,a0
      move.b   0(a0,d0.w),d0        save color register
      and.w    #$000F,d0

usereg:
      lea      _colormapr,a0        set current colors to creg value
      move.b   0(a0,d0.w),red(a6)
      lea      _colormapg,a0
      move.b   0(a0,d0.w),green(a6)
      lea      _colormapb,a0
      move.b   0(a0,d0.w),blue(a6)
      bra      plot

   endc

usenoreg:
      move.b   maxcolor(a6),d0
      cmp.b    #$20,d0
      bne      n1
      move.b   d5,red(a6)
      or.b     d5,d0
      bra      n3
n1:   cmp.b    #$30,d0
      bne      n2
      move.b   d6,green(a6)
      or.b     d6,d0
      bra      n3
n2:   move.b   d7,blue(a6)
      or.b     d7,d0
n3:

plot:
      and.l    #$00FF,d0
      move.b   d0,(a4)+
      cmp.l    a4,a5                   done with 32 pixels?
      bne      nextsamp                nope

      move.l   addr(a6),a0             yep, write them all out to screen
      move.l   (a0),-(a7)
      add.l    #32,(a0)
      move.l   planes(a6),-(a7)
      move.w   #32,-(a7)               pixel count
   ifne ham
      jsr      _writepixels
   endc
   ifne noham
      jsr      _writepixels32
   endc
      lea      $a(a7),a7
      lea      _savearea,a4
 endc

nextsamp:
      move.w   counter(a6),d4
      addq.w   #1,d4
      cmp.w    _nsamps,d4
      blt      samploop

done:
      movem.l  (a7)+,d0-d7/a4-a5
      unlk     a6

      rts
      END

