*-----------------------------------------------------------------------
* writepixels.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   writepixels (npix, planes, offset);
*
*-----------------------------------------------------------------------
plane5   equ   1     1 iff 5 bitplanes
plane6   equ   0     1 iff 6 bitplanes
*-----------------------------------------------------------------------
*
* Function:
*
*   Draws npix pixels from savearea into planes pointed to by planes at
*     bit offset offset.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
npix              equ 8    number of pixels to write  w
planes            equ 10   pointer to Planes array    *l
offset            equ 14   Bit offset into planes     l
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
*
*-----------------------------------------------------------------------

      XREF  _savearea

      csect text

   ifne plane6
      XDEF  _writepixels
_writepixels:
   endc
   ifne plane5
      XDEF  _writepixels32
_writepixels32:
   endc

      link     a6,#0
      movem.l  d2-d7,-(a7)

      move.l   planes(a6),a0
      move.w   npix(a6),d0
      subq.w   #1,d0

      lea      _savearea,a1

loop:
      move.b   (a1)+,d1

      lsr.b    #1,d1
      roxl.l   #1,d2
      lsr.b    #1,d1
      roxl.l   #1,d3
      lsr.b    #1,d1
      roxl.l   #1,d4
      lsr.b    #1,d1
      roxl.l   #1,d5
      lsr.b    #1,d1
      roxl.l   #1,d6
   ifne  plane6
      lsr.b    #1,d1
      roxl.l   #1,d7
   endc

      dbra     d0,loop

      move.l   offset(a6),d1
      lsr.l    #3,d1

      move.l   (a0)+,a1
      move.l   d2,0(a1,d1.l)
      move.l   (a0)+,a1
      move.l   d3,0(a1,d1.l)
      move.l   (a0)+,a1
      move.l   d4,0(a1,d1.l)
      move.l   (a0)+,a1
      move.l   d5,0(a1,d1.l)
      move.l   (a0)+,a1
      move.l   d6,0(a1,d1.l)
   ifne plane6
      move.l   (a0)+,a1
      move.l   d7,0(a1,d1.l)
   endc

done: movem.l  (a7)+,d2-d7
      unlk     a6
      rts
      END

