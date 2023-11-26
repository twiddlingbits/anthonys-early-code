

ie_Class	      equ	4  /* byte */
ie_Code        equ   6  /* word */
ie_Qualifier	equ	8  /* word */
ie_x           equ   $a /* word */
ie_y           equ   $c /* word */

IEQUALIFIER_LEFTBUTTON	 equ $4000

GE_WINDOW      equ   0
GE_X           equ   4
GE_Y           equ   6
GE_TYPE        equ   8  /* word */

IB_ACTIVEWINDOW   equ   $34
W_MOUSEX          equ   $e
W_MOUSEY          equ   $c

	xdef		_inhandler
	xref		_event_buffer
   xref     _write_ptr
   xref     _read_ptr
   xref     _IntuitionBase
   xref     _SysBase
   xref     _gadlib_sigbit
   xref     _tc_gadlib

	csect		text

_inhandler:

   move.l      a0,-(sp)	         	; save event list
   move.l      a2,-(sp)             ; save reg so we can use it
   move.w      #0,newevents         ; no new events yet
;
; first check if the previous event was a left mouse button press
;
   cmp.w       #0,pendingbutton
   beq         loop

   move.w      #0,pendingbutton     ; clear pending event
   move.w      #1,newevents         ; new event
   jsr         calc_event_ptr       ; put pointer to ge in a1
	move.w      #1,GE_TYPE(a1)       ; 1 = LBUTTONDOWN
   jsr         set_window
;
; now loop through all input events in this chain
;
loop:
   move.b      ie_Class(a0),d0      ; get class
   cmp.b       #2,d0                ; RAWMOUSE?
   bne         ignore               ; skip it

   move.w      ie_Code(a0),d0       ; 68=down, e8=up, ff=move
   cmp.w       #$68,d0
   bne.s       not_down
;
; here if user pressed left mouse button
;
   move.w      #1,pendingbutton     ; let intuition have it to select win
   jmp         ignore               

not_down:
   cmp.w       #$e8,d0
   bne         not_up
;
; here if user released left mouse button
;
   move.w      #1,newevents            ; new event
   jsr         calc_event_ptr          ; put pointer to ge in a1
	move.w      #2,GE_TYPE(a1)          ; 2 = LBUTTONUP
   jsr         set_window
   jmp         ignore

not_up:
   cmp.w       #$ff,d0
   bne         ignore
;
; here if mouse moved
;
   move.w      ie_Qualifier(a0),d0
   and.w       #IEQUALIFIER_LEFTBUTTON,d0
   beq         ignore                  ; only interested in dragging
	move.w      #1,newevents            ; new event
   jsr         calc_event_ptr          ; put pointer to ge in a1
   move.w      #3,GE_TYPE(a1)          ; 3 = DELTAMOVE
   jsr         set_window2             ; set win + x/y deltas

ignore:
   move.l      (a0),d0              ;ie_NextEvent
   move.l      d0,a0
   bne         loop

   cmp.w       #0,newevents
   beq         no_signal

   move.l      _SysBase,a2
   move.l      _tc_gadlib,a1
   move.l      _gadlib_sigbit,d1
   moveq       #1,d0
   lsl.l       d1,d0
   jsr         -324(a2)             ; Signal (Task, SignalSet);

no_signal:
   move.l      (sp)+,a2
   move.l      (sp)+,d0	      	   ; restore org event list
   rts

*************************************

set_window:

   movea.l     _IntuitionBase,a2
   move.l      IB_ACTIVEWINDOW(a2),d0     ; get active window from inutionbase
   move.l      d0,GE_WINDOW(a1)           ; save in circ. buffer
   move.l      d0,a2                   
   move.w      W_MOUSEX(a2),GE_X(a1)      ; GE->X = window->MouseX
   move.w      W_MOUSEY(a2),GE_Y(a1)      ; GE->Y = window->MouseY
   rts

*************************************

set_window2:

   movea.l     _IntuitionBase,a2
   move.l      IB_ACTIVEWINDOW(a2),d0     ; get active window from inutionbase
   move.l      d0,GE_WINDOW(a1)           ; save in circ. buffer
   move.w      ie_x(a0),GE_X(a1)          ; GE->X = ie->X
   move.w      ie_y(a0),GE_Y(a1)          ; GE->Y = ie->Y
   rts

*************************************

calc_event_ptr:

   move.w      _write_ptr,d0        ; index for destination
   lsl.w       #1,d0                ; X 10 (sizeof(struct GadEvent))
   move.w      d0,d1                ; X 10 = X (2+8)
   lsl.w       #2,d0
   add.w       d0,d1
   lea.l       _event_buffer,a1
   lea.l       0(a1,d1.w),a1        ; dest event_buffer
   add.w       #1,_write_ptr        ; inc pointer
   and.w       #511,_write_ptr      ; circ. buffer 512 in size
   rts

newevents:
   dc.w        0
pendingbutton:
   dc.w        0

	END
