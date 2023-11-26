*-----------------------------------------------------------------------
* fast_gray.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   fast_gray4(pat, i, src, dest, width, planes, plen)
*
*
*-----------------------------------------------------------------------
*
* Function:
*
*  breaks up color bitplanes into one plane of dot patterns
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*
*
*
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
pat               equ 8    * address of pattern array
i                 equ 12   * translates bitvalue to pat index
src               equ 16   * source plane 0
dest              equ 20   * dest bitmap
width             equ 24   * src width in bits
planes            equ 28   * number of planes in source
plen              equ 32   * page length
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
* destline          equ -2    * destination line #         (var)       w
*
*-----------------------------------------------------------------------
*
* Constants:
*
* round       equ   $8000          * round fixed pt number to integer
*
*-----------------------------------------------------------------------

      XDEF  _fast_gray

      csect text

_fast_gray:

      link     a0,#0
      movem.l  d2-d7/a2-a5,-(a7)
      movea.l  src(a0),a1              * put source address into a1
      movea.l  dest(a0),a2             * put dest address into a2
      movea.l  i(a0),a3                * put translation index into a3
      move.w   plen(a0),d2             * bit plane len in d2
      moveq.w  #0,d0                   * d0 is the pixel position

loop: moveq.b  #0,d1                   * d1 is the color reg number
      moveq.w  #0,d3
      move.b   d0,d3                   * pix pos in d3
      eori.b   #7,d3                   * d3 = 7-(d3&7)

      move.w   d0,d4                   * d4 will be byte offset from source
      lsr      #3,d4

      movem.l  (a7)+,d2-d7/a2-a5
      unlk     a0
      rts
      END

