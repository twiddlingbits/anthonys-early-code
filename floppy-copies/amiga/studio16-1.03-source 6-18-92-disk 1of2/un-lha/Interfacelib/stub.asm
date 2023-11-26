;:ts=8

	xref	_InterfaceBase
	xdef	_OpenNewWind
_OpenNewWind:
	movea.l	_InterfaceBase,a6
	jmp	-30(a6)

	xdef	_CloseWind
_CloseWind:
	movea.l	_InterfaceBase,a6
	jmp	-36(a6)

	xdef	_ColorGadList
_ColorGadList:
	movea.l	_InterfaceBase,a6
	jmp	-42(a6)

	xdef	_HandleIntuitMess
_HandleIntuitMess:
	movea.l	_InterfaceBase,a6
	jmp	-48(a6)

	xdef	_RefreshWind
_RefreshWind:
	movea.l	_InterfaceBase,a6
	jmp	-54(a6)

	xdef	_RefreshBar
_RefreshBar:
	movea.l	_InterfaceBase,a6
	jmp	-60(a6)

	xdef	_WindTitleChange
_WindTitleChange:
	movea.l	_InterfaceBase,a6
	jmp	-66(a6)

	xdef	_FindFileName
_FindFileName:
	movea.l	_InterfaceBase,a6
	jmp	-72(a6)

	xdef	_FileRequest
_FileRequest:
	movea.l	_InterfaceBase,a6
	jmp	-78(a6)

	END
