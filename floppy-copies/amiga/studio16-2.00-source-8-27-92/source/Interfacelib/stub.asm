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

	xdef	_FreeChannelMenu
_FreeChannelMenu:
	movea.l	_InterfaceBase,a6
	jmp	-84(a6)

	xdef	_S16Text
_S16Text:
	movea.l	_InterfaceBase,a6
	jmp	-90(a6)

	xdef	_MakeMenuesFit
_MakeMenuesFit:
	movea.l	_InterfaceBase,a6
	jmp	-96(a6)

	xdef	_ItemData
_ItemData:
	movea.l	_InterfaceBase,a6
	jmp	-102(a6)

	xdef	_CheckItem
_CheckItem:
	movea.l	_InterfaceBase,a6
	jmp	-108(a6)

	xdef	_CheckNum
_CheckNum:
	movea.l	_InterfaceBase,a6
	jmp	-114(a6)

	xdef	_CheckData
_CheckData:
	movea.l	_InterfaceBase,a6
	jmp	-120(a6)

	xdef	_MenuCheckSum
_MenuCheckSum:
	movea.l	_InterfaceBase,a6
	jmp	-126(a6)

	xdef	_ItemNum
_ItemNum:
	movea.l	_InterfaceBase,a6
	jmp	-132(a6)

	xdef	_BuildChannelMenu
_BuildChannelMenu:
	movea.l	_InterfaceBase,a6
	jmp	-138(a6)

	xdef	_FindCheckedItem
_FindCheckedItem:
	movea.l	_InterfaceBase,a6
	jmp	-144(a6)

	xdef	_BuildHandlerMenu
_BuildHandlerMenu:
	movea.l	_InterfaceBase,a6
	jmp	-150(a6)

	xdef	_FreeHandlerMenu
_FreeHandlerMenu:
	movea.l	_InterfaceBase,a6
	jmp	-156(a6)

	xdef	_show_error
_show_error:
	movea.l	_InterfaceBase,a6
	jmp	-162(a6)

	xdef	_newgetstr
_newgetstr:
	movea.l	_InterfaceBase,a6
	jmp	-168(a6)

	xdef	_BuildProjectMenu
_BuildProjectMenu:
	movea.l	_InterfaceBase,a6
	jmp	-174(a6)

	xdef	_BuildApplicationMenu
_BuildApplicationMenu:
	movea.l	_InterfaceBase,a6
	jmp	-180(a6)

	xdef	_FreeProjectMenu
_FreeProjectMenu:
	movea.l	_InterfaceBase,a6
	jmp	-186(a6)

	xdef	_FreeApplicationMenu
_FreeApplicationMenu:
	movea.l	_InterfaceBase,a6
	jmp	-192(a6)

	END
