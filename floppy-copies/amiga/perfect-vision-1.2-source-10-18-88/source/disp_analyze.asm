     
*-----------------------------------------------------------------------
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
hamreg      equ   1     1 iff use ham mode with registers      (disp_line)
hamnoreg    equ   0     1 iff use ham mode without registers   (disp_noreg)
noham       equ   0     1 iff 32 color (non-ham) mode          (disp_line32)
*
*
usebiglut   equ   1     1 iff samples go through full color (4096) lut
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
*
     
      csect text
     
      XDEF  _disp_analyze
_disp_analyze
     
      INCLUDE disp_line.inc
     
      END
     
