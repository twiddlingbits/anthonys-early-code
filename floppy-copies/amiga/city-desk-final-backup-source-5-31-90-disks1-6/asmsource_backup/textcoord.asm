*-----------------------------------------------------------------------
* textcoord.asm
*-----------------------------------------------------------------------
*
* Calling sequence:
*  opt_tptoc(&state);
*
*-----------------------------------------------------------------------
*
* Function:
*
*  Premultiply mag and cpertextpix to optimize the conversions.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*   d0, d1, d2, d3   = temps
*   a0   = pointer to text_state
*
*  Note: None of d0-d1/a0-a1 need to be saved.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
state             equ 8    * text_state structure           l
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
maxframe1         equ 0    * stack frame size (no vars)
*
*-----------------------------------------------------------------------
*
      INCLUDE   text.i
*
*-----------------------------------------------------------------------

*      csect    tc,,,2,2

      XREF  _cpertextpixx
      XREF  _cpertextpixy

      csect text

      XDEF  _opt_tptoc

_opt_tptoc:
      link     a5,#maxframe1
      move.l   d2,-(a7)
      move.l   d3,-(a7)

      move.l   state(a5),a0

      move.l   _cpertextpixy,d0     * 4.16         do Y first
      lsr.l    #4,d0                * 4.12
      move.l   st_magy(a0),d1       * 8.16
      lsr.l    #4,d1                * 8.12

      move.l   d0,d2                * 32X32 multiply
      move.l   d1,d3
      swap     d2
      swap     d3
      mulu     d1,d2
      mulu     d0,d3
      mulu     d1,d0
      add.w    d3,d2
      swap     d2
      clr.w    d2
      add.l    d2,d0                * 12.24

      lsr.l    #8,d0                * 12.16
      move.l   d0,opt_y

      move.l   _cpertextpixx,d0     * 4.16         now do X
      lsr.l    #4,d0                * 4.12
      move.l   st_magx(a0),d1       * 8.16
      lsr.l    #4,d1                * 8.12

      move.l   d0,d2                * 32X32 multiply
      move.l   d1,d3
      swap     d2
      swap     d3
      mulu     d1,d2
      mulu     d0,d3
      mulu     d1,d0
      add.w    d3,d2
      swap     d2
      clr.w    d2
      add.l    d2,d0                * 12.24

      lsr.l    #8,d0                * 12.16
      move.l   d0,opt_x

      move.l   (a7)+,d3
      move.l   (a7)+,d2
      unlk     a5
      rts
*
*-----------------------------------------------------------------------
*
* Calling sequence:
*  COORD tpmagtocx (p, mag);
*  COORD stpmagtocx(p, mag);     (same as tpmagtocx for now)
*  COORD tpmagtocy (p, mag);
*
*-----------------------------------------------------------------------
*
* Function:
*
*  Convert integer text pixels p into COORD in x or y, magnified.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*   d0,d1,d2   = temporaries
*
*   a0   = used for saving d2 only
*
*  Note: None of d0-d1/a0-a1 need to be saved.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
p        equ   10    * text pixels                 w
mag      equ   12    * magnify factor (not used)   l
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
maxframe2         equ 0    * stack frame size (no vars)
*
*-----------------------------------------------------------------------
*

      XDEF  _tpmagtocx
      XDEF  _stpmagtocx
      XDEF  _tpmagtocy

_tpmagtocx:
_stpmagtocx:
      link     a6,#maxframe2
      move.l   d2,a0                * save d2 in a0 (a0 need not be saved)

      move.w   p(a6),d0
      bpl      x1
      neg.w    d0
x1:   move.l   opt_x,d1

      move.l   d1,d2                * 16X32 multiply
      swap     d2
      mulu     d0,d2
      mulu     d1,d0
      swap     d2
      clr.w    d2
      add.l    d2,d0

      clr.w    d0
      swap     d0                   * p*opt >> 16
      tst.w    p(a6)
      bpl      x2
      neg.w    d0

x2:   move.l   a0,d2
      unlk     a6
      rts

*-----------------------------------------------------------------------

_tpmagtocy:
      link     a6,#maxframe2
      move.l   d2,a0                * save d2 in a0 (a0 need not be saved)

      move.w   p(a6),d0
      bpl      y1
      neg.w    d0
y1:   move.l   opt_y,d1

      move.l   d1,d2                * 16X32 multiply
      swap     d2
      mulu     d0,d2
      mulu     d1,d0
      swap     d2
      clr.w    d2
      add.l    d2,d0

      clr.w    d0
      swap     d0                   * p*opt >> 16
      tst.w    p(a6)
      bpl      y2
      neg.w    d0

y2:   move.l   a0,d2
      unlk     a6
      rts

*-----------------------------------------------------------------------
*
* Data area
*
*-----------------------------------------------------------------------
*   csect data,1,,0,4

opt_y ds.l     2
opt_x     equ      opt_y+4        * Stupid Assembler bug
      END

