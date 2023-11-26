********************************************************************
*               Commodore-Amiga, Inc.                              *
*               prtgfx.i                                           *
********************************************************************

	IFND	DEVICES_PRTGFX_I
DEVICES_PRTGFX_I	EQU		1

PCMYELLOW			EQU		0	; byte index for blue/yellow
PCMMAGENTA			EQU		1	; byte index for green/magenta
PCMCYAN				EQU		2	; byte index for red/cyan
PCMBLACK			EQU		3	; byte index for white/black

	STRUCTURE	colorEntry,0
		LABEL	colorLong
		LABEL	colorSByte
		STRUCT	colorByte,4
		LABEL	ce_SIZEOF
		
	STRUCTURE	PrtInfo,0
		APTR	pi_render	; ptr to printer specific render function
		APTR	pi_rp		; source rastport
		APTR	pi_temprp	; temp 1 line high rastport
		APTR	pi_RowBuf	; array of color codes
		APTR	pi_HamBuf	; array of HAM color codes
		APTR	pi_ColorMap	; rastport colormap in YMCB values
		APTR	pi_ColorInt	; color intensities for entire row
		APTR	pi_HamInt	; HAM color intensities for entire row
		APTR	pi_Dest1Int		; color intensities for dest1 row
		APTR	pi_Dest2Int		; color intensities for dest2 row
		APTR	pi_ScaleX		; array of scale values for X
		APTR	pi_ScaleXAlt	; alt array of scale values for X
		APTR	pi_dmatrix	; ptr to dither matrix
		APTR	pi_TopBuf		; array of color codes for line above us
		APTR	pi_BotBuf		; array of color codes for line below us

		UWORD	pi_RowBufSize	; size of RowBuf array
		UWORD	pi_HamBufSize	; size of HamBuf array
		UWORD	pi_ColorMapSize	; size of ColorMap array
		UWORD	pi_ColorIntSize	; size of Color Intensities array
		UWORD	pi_HamIntSize	; size of Ham Intensities array
		UWORD	pi_Dest1IntSize	; size of Dest1Int array
		UWORD	pi_Dest2IntSize	; size of Dest2Int array
		UWORD	pi_ScaleXSize	; size of ScaleX array
		UWORD	pi_ScaleXAltSize	; size of ScaleXAlt array

		UWORD	pi_PrefsFlags	; copy of Preferences 'PrintFlags'
		ULONG	pi_special	; copy of io_Special bits
		UWORD	pi_xstart	; source x origin to print from
		UWORD	pi_ystart	; source y origin to print from
		UWORD	pi_width	; source width
		UWORD	pi_height	; source height
		UWORD	pi_pc		; destination width
		UWORD	pi_pr		; destination height
		UWORD	pi_ymult	; y multiple (for y scaling)
		UWORD	pi_ymod		; y modulas (for y scaling)
		UWORD	pi_ety		; y error term (for y scaling)
		UWORD	pi_xpos		; offset to start printing picture on paper
		UWORD	pi_threshold	; copy of threshold value (from prefs)
		UWORD	pi_tempwidth	; temp var for x scaling
		UWORD	pi_flags	; internal flags (see PRT_ EQU's above)
		LABEL	prtinfo_SIZEOF

   ENDC
