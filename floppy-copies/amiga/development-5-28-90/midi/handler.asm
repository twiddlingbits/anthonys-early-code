*
* _midi_handler
*
* This replacement of the serial.device handler will correctly capture
* incoming midi data. See associated routines:
*
* midi_init()              Installs _midi_handler
* midi_end()               restores serial.device handler
* midi_check()             0 if no data pending, else not 0
* count = midi_grab(data)  transfers count bytes into data
*

SERDATR  equ   $018
INTREQ   equ   $09c
INTF_RBF equ   $800

CLR_RFB  equ   $0800
SET_RFB  equ   $8800

         csect text

         XDEF _midi_handler
         XREF _current_index
         XREF _midi_buffer
         XREF _num_bytes      * word

_midi_handler:
         move.w   SERDATR(a0),d0       ;get word of data
         move.w   _current_index,d1
         lea      _midi_buffer,a5
         move.b   d0,0(a5,d1.w)
         addq.w   #1,d1
         andi.l   #511,d1              ;wrap buffer ptr at 512 bytes
         move.w   d1,_current_index    ;store ptr for next interupt
         addq.w   #1,_num_bytes        ;count how many bytes gotten
         move.w   #INTF_RBF,INTREQ(a0) ;clear the interupt
         rts
         END

