*-----------------------------------------------------------------------
* close.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   show.asm (r, g, b)
*
* Function:
*
*  find a color "close" to rgb
*
*
r                equ 10    * address of source             b
g                equ 14    * address of source             b
b                equ 18    * address of source             b
*
*
*-----------------------------------------------------------------------

      XDEF  _close
      XREF  _rcmap
      XREF  _gcmap
      XREF  _bcmap

      csect text

_close
      link     a0,#0
      movem.l  d1-d7,-(a7)

      clr.l    d0                         * color index
      move.w   #50,d5                     * best match so far in d5
*                                           best match index in d6
      move.w   r(a0),d1                   * get red to look for
      andi.w   #15,d1                     * only low nyble is data
      move.w   g(a0),d2                   * get grn to look for
      andi.w   #15,d2
      move.w   b(a0),d3                   * get blue to look for
      andi.w   #15,d3

loop:
      clr.w    d7                         * this index's "difference"
      clr.w    d4
      lea      _rcmap,a0
      move.b   0(a0,d0.w),d4           * get a red value
      sub.w    d1,d4
      bpl      pos1                       * take ABS of it
      neg.w    d4
pos1: move.w   d4,d7
      clr.w    d4
      lea      _gcmap,a0
      move.b   0(a0,d0.w),d4           * get a red value
      sub.w    d2,d4                      * gcmap[i] -  g
      bpl      pos2                       * take ABS of it
      neg.w    d4
pos2: add.w    d4,d7
      clr.w    d4
      lea      _bcmap,a0
      move.b   0(a0,d0.w),d4           * get a red value
      sub.w    d2,d4                      * gcmap[i] -  g
      bpl      pos3                       * take ABS of it
      neg.w    d4
pos3: add.w    d4,d7                      * dif now in d7

      cmp.w    d5,d7
      bge      worse
      move.w   d7,d5                      * new best color
      move.w   d0,d6                      * new index

worse:

      addq.w   #1,d0
      cmp.w    #32,d0
      bne      loop

      move.w   d6,d0                      * return index

      movem.l  (a7)+,d1-d7
      unlk     a0
      rts
      END

