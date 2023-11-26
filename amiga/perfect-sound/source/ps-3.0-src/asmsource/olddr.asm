*-----------------------------------------------------------------------
* digi_right.asm
*-----------------------------------------------------------------------
* Calling sequence:
*   digi_right(buffer, maxsize, delay)
*
*-----------------------------------------------------------------------
*
* Function:
*
* Digitizes from the right channel of a perfect sound digitizer.
* intill either maxsize is reached or left mouse button is clicked.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
buffer            equ 8    * put sound data here           l
maxsize           equ 12   * Max bytes to digitize      l
delay             equ 18   * delay between each sample     w
*
*-----------------------------------------------------------------------
*
data1             equ   $bfe101
dir1              equ   $bfe301
data2             equ   $bfd000
dir2              equ   $bfd200
pa                equ   $bfe001
*
*-----------------------------------------------------------------------
maxframe          equ  0


      XDEF  _digi_right

      csect text

_digi_right:
      link     a0,#maxframe
      move.l   a2,-(a7)
      move.l   a3,-(a7)

      move.b   #0,dir1           * all lines read
      move.b   #0,data1
      move.b   #6,dir2           * pa1 and pa2 write
      move.b   #4,data2          * RIGHT

      move.l   maxsize(a0),d0    * d0 holds max samples
      movea.l  buffer(a0),a1     * a1 now holds buffer location
      lea      pa,a2             * a2 points to mouse control byte

loop: moveq    #00,d1            * 4
      move.b   data1,d1          * 16
      subi.l   #128,d1           * 8
      move.b   d1,(a1)+          * 8

      move.w   delay(a0),d1      * 12
wait: dbf      d1,wait           * 10 / 14 if no branch
      btst     #6,(a2)           * 12 left button cliked?
      beq      out               * 10/ 12 combine these into one db_?
      subq.l   #1,d0             * 8
      bne      loop              * 10/ 8
*                                  total = 104 + 10*delay
out:  suba.l   buffer(a0),a1
      move.l   a1,d0

      move.l  (a7)+,a3
      move.l  (a7)+,a2
      unlk     a0
      rts
      END

