;**********************************************************************
;*                                                                    *
;*          MODIFY COPPER ROUTINES for OVERSCAN Video Display         *
;*                                                                    *
;*                                                                    *
;*WARNING!!!!   WARNING!!!!   WARNING!!!!    WARNING!!!!    WARNING!!!*
;*                                                                    *
;*                  K L U D G E C O D E   A H E A D                   *
;*                                                                    *
;*                                                                    *
;*  Provided courtesy of Randy Linden, Visionary Design Technologies  *
;*                                                                    *
;**********************************************************************
/** bit planes must be 44 bytes wide! **/

   XDEF     _ModifyCopper
   XREF     _GfxBase


LastPlane   equ      $00ee       ; $EE for 4 planes,
                                 ; $F2 for 5 planes,
                                 ; $F6 for 6 planes.

PlanesOn   equ      $01004200    ; $01004200 for 4 planes,
                                 ; $01005200 for 5 planes,
                                 ; $01006200 for 6 planes.


;
;   * * * * * * *       MODIFY COPPER FOR OVERSCAN       * * * * * * *
;
;   Publics required:   GraphicsBase
;
;   Registers passed:   NONE
;
;   Registers killed:   A0, D0
;
_ModifyCopper
   move.l   GraphicsBase,a0               ; Pointer to Graphics.library
   move.l   $32(a0),a0                  ; Pointer to current gb_LOFlist
   move.l   #$1601fffe,(a0)+            ; Start display at line $16
ModifyCopper100
   move.w   (a0)+,d0                  ; Get the next instruction
   cmp.w   #$008e,d0                  ; DIWSTRT?
   bne.s   ModifyCopper200
   move.w   #$0571,(a0)+
   bra.s   ModifyCopper850
ModifyCopper200
   cmp.w   #$0090,d0                  ; DIWSTOP?
   bne.s   ModifyCopper300
   move.w   #$40d1,(a0)+
   bra.s   ModifyCopper850
ModifyCopper300
   cmp.w   #$0092,d0                  ; DDFSTRT?
   bne.s   ModifyCopper400
   move.w   #$0030,(a0)+
   bra.s   ModifyCopper850
ModifyCopper400
   cmp.w   #$0094,d0                  ; DDFSTOP?
   bne.s   ModifyCopper500
   move.w   #$00d8,(a0)+
   bra.s   ModifyCopper850
ModifyCopper500
   cmp.w   #$2c01,d0                  ; Turn ON Display?
   beq.s   ModifyCopper550
   cmp.w   #LastPlane,d0               ; Last BitPlane setting?
   bne.s   ModifyCopper700
ModifyCopper550
   move.w   (a0)+,d0
   move.l   #$1801fffe,(a0)+            ; Start display at $18
   move.l   #PlanesOn,(a0)+               ; Turn on bitplanes
   move.l   #$fffffffe,(a0)+            ; END of list!
   bra.s   ModifyCopper900
ModifyCopper700
   cmp.w   #$0108,d0                  ; MODULO?
   beq.s   ModifyCopper750
   cmp.w   #$010a,d0                  ; MODULO?
   bne.s   ModifyCopper800
ModifyCopper750
   move.w   #$0000,(a0)+
   bra.s   ModifyCopper850
ModifyCopper800
   move.w   (a0)+,d0                  ; Skip the parameter value
ModifyCopper850
   bra.s   ModifyCopper100
ModifyCopper900
   rts                             ; Finished.
