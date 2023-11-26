*-----------------------------------------------------------------------
* kern.asm
*-----------------------------------------------------------------------
*
* Calling sequence:
*  apply_kern(prevch, ch, &state, &w);
*
*-----------------------------------------------------------------------
*
* Function:
*
*  Adds the kern value for character pair (prevch, ch) to *w.  *state
*     contains the font info and therefore the kern table.
*
*-----------------------------------------------------------------------
*
* Register Usage:
*
*   d0   = character pair, and w
*   d1   = loop counter
*
*   a0   = pointer into kern table
*   a1   = general structure pointer
*
*  Note: None of d0-d1/a0-a1 need to be saved.
*
*-----------------------------------------------------------------------
*
* Calling Parameters:
*
prevch            equ 11   ; previous character             b
ch                equ 15   ; current character              b
state             equ 16   ; text_state structure           l
wptr              equ 20   ; pointer to width               l (*w)
*
*-----------------------------------------------------------------------
*
* Temporary Variables on Stack:
*
maxframe          equ 0    ; stack frame size (no vars)
*
*-----------------------------------------------------------------------
*
      INCLUDE  laser.i
      INCLUDE  text.i
*
*-----------------------------------------------------------------------

      XDEF  _apply_kern

      csect text

_apply_kern:
      link     a6,#maxframe

      move.b   prevch(a6),d0
      lsl.w    #8,d0
      move.b   ch(a6),d0                  ; prevch<<8 | ch

      move.l   state(a6),a1               ; state pointer

      cmp.w    #$2E20,d0                  ; end of sentence?  '. '
      bne      notperiod                  ; nope

      move.w   st_sent_space(a1),d1       ; yes, get extra space
      move.l   wptr(a6),a0                ; get pointer to w
      add.w    d1,(a0)                    ; add extra space

notperiod:
      move.l   st_lfont(a1),a1            ; state->lfont
      move.l   a1,d1                      ; test for NULL
      beq.s    return                     ; not lfont
      move.l   lf_kern(a1),a1             ; state->lfont->kern
      move.l   a1,d1                      ; test for NULL
      beq.s    return                     ; no kern table

      move.l   kern_num(a1),d1
      beq.s    return                     ; just in case
      subq     #1,d1                      ; adjust for db count

      move.l   kern_pairs(a1),a0          ; addr of pairs

loop  cmp.w    (a0)+,d0
      dbeq     d1,loop
*****      dbcc     d1,loop               ; loop until d0 >= (a0) (dbhs)

      bne.s    return                     ; no match, or counter ran out

      sub.l    kern_pairs(a1),a0          ; subtract base of table
      add.l    kern_value(a1),a0          ; add base of parallel table
      move.w   -(a0),d0                   ; get kern value - predec
*                                         ; compensates for postinc above
      move.l   wptr(a6),a1                ; get pointer to w
      add.w    d0,(a1)                    ; add kern value

return:
      unlk     a6
      rts

      END

