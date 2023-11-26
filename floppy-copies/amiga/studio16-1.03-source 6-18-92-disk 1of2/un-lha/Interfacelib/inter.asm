	SECTION	LibCode,CODE

	XDEF	_SysBase,_DOSBase,_IntuitionBase,_GfxBase,_LibBase

numOfFuncs	equ	  13
negsize		equ	numOfFuncs*6

	XREF	_LVOAddLibrary,_LVOOldOpenLibrary,_LVOCloseLibrary
	XREF	_LVOAllocMem,_LVOFreeMem,_LVORemove

VERSION		equ	1
REVISION	equ	1

return0:
	moveq	#0,d0
	rts

RomTag:
	dc.w	$4AFC
	dc.l	RomTag,endRom
	dc.b	0,VERSION,9,0
	dc.l	Lib_Name,Lib_Id,libstartup
endRom

libstartup:
	move.l	a3,-(sp)
	lea	SegList,a3
	move.l	a0,(a3)+
	move.l	a6,(a3)+
	lea	12(a3),a1
	jsr	_LVOOldOpenLibrary(a6)
	move.l	d0,(a3)+
	lea	20(a3),a1
	jsr	_LVOOldOpenLibrary(a6)
	move.l	d0,(a3)+
	lea	34(a3),a1
	jsr	_LVOOldOpenLibrary(a6)
	move.l	d0,(a3)
	lea	-50(a3),a1
	movea.l	a1,a3
	jsr	_LVOAddLibrary(a6)
	XREF	_init_Interface
	move.l	a6,-(sp)
	jsr	_init_Interface
	movea.l	(sp)+,a6
	tst.b	d0
	bne.s	tru
	bsr.s	abe
	suba.l	a3,a3
tru	move.l	a3,d0
	movea.l	(sp)+,a3
	rts

myOpen:
	addq.w	#1,32(a6)
	Bclr.b	#3,14(a6)
	move.l	a6,d0
	rts

abe	lea	_SysBase,a3
	movea.l	(a3)+,a6
	movea.l	(a3)+,a1
	jsr	_LVOCloseLibrary(a6)
	movea.l	(a3)+,a1
	jsr	_LVOCloseLibrary(a6)
	movea.l	(a3),a1
	jsr	_LVOCloseLibrary(a6)

	suba.w	#50,a3
	movea.l	a3,a1
	jmp	_LVORemove(a6)
myClose:
	subq.w	#1,32(a6)
	bne.s	zro
	Bset.b	#3,14(a6)
	bra.s	exp
zro	moveq	#0,d0
	rts

myExpunge:
	Bset.b	#3,14(a6)
	move.w	32(a6),d1
	bne.s	zro
exp	move.l	a3,-(sp)
	XREF	_rem_Interface
	move.l	a6,-(sp)
	jsr	_rem_Interface
	movea.l	(sp)+,a6
	bsr.s	abe
	move.l	34(a3),d0
	movea.l	a3,a6
	movea.l	(sp)+,a3
	rts

LibFuncTable:
	XREF	_FileRequest
	dc.w	$4EF9
	dc.l	_FileRequest
	XREF	_FindFileName
	dc.w	$4EF9
	dc.l	_FindFileName
	XREF	_WindTitleChange
	dc.w	$4EF9
	dc.l	_WindTitleChange
	XREF	_RefreshBar
	dc.w	$4EF9
	dc.l	_RefreshBar
	XREF	_RefreshWind
	dc.w	$4EF9
	dc.l	_RefreshWind
	XREF	_HandleIntuitMess
	dc.w	$4EF9
	dc.l	_HandleIntuitMess
	XREF	_ColorGadList
	dc.w	$4EF9
	dc.l	_ColorGadList
	XREF	_CloseWind
	dc.w	$4EF9
	dc.l	_CloseWind
	XREF	_OpenNewWind
	dc.w	$4EF9
	dc.l	_OpenNewWind
	dc.w	$4EF9
	dc.l	return0
	dc.w	$4EF9
	dc.l	myExpunge
	dc.w	$4EF9
	dc.l	myClose
	dc.w	$4EF9
	dc.l	myOpen
_LibBase:
	dc.l	0,0
	dc.b	9,0
	dc.l	Lib_Name
	dc.b	6,0
	dc.w	negsize,54,VERSION,REVISION
	dc.l	Lib_Id,0
	dc.w	0
SegList	dc.l	0
_SysBase	dc.l	0
_DOSBase	dc.l	0
_IntuitionBase	dc.l	0
_GfxBase	dc.l	0

DOSName		dc.b	'dos.library',0
IntuitionName	dc.b	'intuition.library',0
GfxName		dc.b	'graphics.library',0

Lib_Name	dc.b	'Interface.library',0
Lib_Id		dc.b	'Todds window/menu/etc library � 1991 ',13,10,0

	END

	END
