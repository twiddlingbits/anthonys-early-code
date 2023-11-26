#ifndef GRAPHICS_GFXBASE_H
#define GRAPHICS_GFXBASE_H

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#ifndef EXEC_INTERRUPTS_H
#include <exec/interrupts.h>
#endif

/****** gfxbase.h ***************************************************/
/*                                                                  */
/*              Confidential Information: Amiga Computer, Inc.      */
/*              Copyright (c) Amiga Computer, Inc.                  */
/*                                                                  */
/*                  Modification History                            */
/*  date    :   author :    Comments                                */
/*  -------     ------      --------------------------------------- */
/*  10-20-84    Kodiak      added this header file & TextFonts      */
/*                                                                  */
/********************************************************************/
struct GfxBase
{
    struct Library  LibNode;
    struct View *ActiView;
    struct copinit *copinit;    /* ptr to copper start up list */
    long    *cia;       /* for 8520 resource use */
    long    *blitter;   /* for future blitter resource use */
    UWORD   *LOFlist;
    UWORD   *SHFlist;
    struct bltnode *blthd,*blttl;
    struct bltnode *bsblthd,*bsblttl;
    struct Interrupt vbsrv,timsrv,bltsrv;
    struct List     TextFonts;
    struct  TextFont *DefaultFont;
    UWORD  Modes;              /* copy of current first bplcon0 */
    BYTE VBlank;
    BYTE Debug;
    SHORT BeamSync;
    SHORT system_bplcon0;   /* this is initialized to 0 */
			/* it is ored into each bplcon0 for display */
    UBYTE   SpriteReserved;
    UBYTE   bytereserved;
    /* candidates for removal */
    USHORT   Flags;
    SHORT   BlitLock;
	short	BlitNest;

	struct	List	BlitWaitQ;
	struct	Task	*BlitOwner;
    struct  List    TOF_WaitQ;
	UWORD	DisplayFlags;	/* NTSC PAL GENLOC etc*/
							/* Display flags are determined at power on */
	struct SimpleSprites **SimpleSprites;
	UWORD	MaxDisplayRow;
	UWORD	MaxDisplayColumn;
	UWORD	NormalDisplayRows;
	UWORD	NormalDisplayColumns;
	UWORD	NormalDPMX;		/* Dots per meter on display */
	UWORD	NormalDPMY;		/* Dots per meter on display */
	struct	SignalSemaphore *LastChanceMemory;
	UWORD	*LCMptr;
	UWORD	MicrosPerLine;	/* 256 time usec/line */
    ULONG   reserved[2];    /* for future use */
};

#define NTSC	1
#define GENLOC	2
#define PAL		4

#define BLITMSG_FAULT   4
#endif
