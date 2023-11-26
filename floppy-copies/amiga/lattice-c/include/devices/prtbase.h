/********************************************************************/
/*               Commodore-Amiga, Inc.                              */
/*               prtbase.h                                          */
/********************************************************************/
/*********************************************************************
 *
 * printer device data definition
 *
 ********************************************************************/

#ifndef  DEVICES_PRTBASE_H
#define  DEVICES_PRTBASE_H


#ifndef  EXEC_NODES_H
#include "exec/nodes.h"
#endif
#ifndef  EXEC_LISTS_H
#include "exec/lists.h"
#endif
#ifndef  EXEC_PORTS_H
#include "exec/ports.h"
#endif
#ifndef  EXEC_LIBRARIES_H
#include "exec/libraries.h"
#endif
#ifndef  EXEC_TASKS_H
#include "exec/tasks.h"
#endif

#ifndef  DEVICES_PARALLEL_H
#include "devices/parallel.h"
#endif
#ifndef  DEVICES_SERIAL_H
#include "devices/serial.h"
#endif
#ifndef  DEVICES_TIMER_H
#include "devices/timer.h"
#endif
#ifndef  LIBRARIES_DOSEXTENS_I
#include "libraries/dosextens.h"
#endif
#ifndef  INTUITION_INTUITION_H
#include "intuition/intuition.h"
#endif


struct DeviceData {
    struct Library dd_Device; /* standard library node */
    APTR dd_Segment;          /* A0 when initialized */
    APTR dd_ExecBase;         /* A6 for exec */
    APTR dd_CmdVectors;       /* command table for device commands */
    APTR dd_CmdBytes;         /* bytes describing which command queue */
    UWORD   dd_NumCommands;   /* the number of commands supported */
};


#define  P_STKSIZE   0x800

struct   PrinterData {
   struct   DeviceData pd_Device;
   struct   MsgPort pd_Unit;  /* the one and only unit */
   BPTR     pd_PrinterSegment;   /* the printer specific segment */
   UWORD    pd_PrinterType;   /* the segment printer type */
   struct   PrinterSegment *pd_SegmentData; /* the segment data structure */
   UBYTE    *pd_PrintBuf;     /* the raster print buffer */
   int      (*pd_PWrite)();   /* the write function */
   int      (*pd_PBothReady)();  /* write function's done */
   union {                    /* port I/O request 0 */
   struct IOExtPar pd_p0;
   struct IOExtSer pd_s0;
    } pd_ior0;
#define  pd_PIOR0 pd_ior0.pd_p0
#define  pd_SIOR0 pd_ior0.pd_s0
    union {                   /*   and 1 for double buffering */
   struct IOExtPar pd_p1;
   struct IOExtSer pd_s1;
    } pd_ior1;
#define  pd_PIOR1 pd_ior1.pd_p1
#define  pd_SIOR1 pd_ior1.pd_s1
    struct timerequest pd_TIOR;  /* timer I/O request */
    struct MsgPort pd_IORPort;   /*   and message reply port */
    struct Task pd_TC;           /* write task */
    UBYTE pd_Stk[P_STKSIZE];     /*   and stack space */
    UBYTE pd_Flags;              /* device flags */
    UBYTE pd_pad;
    struct Preferences pd_Preferences; /* the latest preferences */
    UBYTE   pd_PWaitEnabled;     /* wait function switch */
};

#define  PPCB_GFX 0
#define  PPCF_GFX 0x01
#define  PPCB_COLOR  1
#define  PPCF_COLOR  0x02

#define  PPC_BWALPHA 0 /* black&white alphanumerics */
#define  PPC_BWGFX   1 /* black&white graphics */
#define  PPC_COLORGFX   3 /* color graphics */

#define	PCC_BW		1	/* only black&white */
#define	PCC_YMC		2	/* only yellow/magenta/cyan */
#define	PCC_YMC_BW	3	/* yellow/magenta/cyan or black&white */
#define	PCC_YMCB	4	/* yellow/magenta/cyan/black */

#define	PCC_4COLOR	0x4	/* a flag for YMCB and BGRW */
#define	PCC_ADDITIVE	0x8	/* not yellow/magenta/cyan/black, */
				/*   but blue/green/red/white */
#define	PCC_WB		0x9	/* only black&white, 0 == BLACK */
#define	PCC_BGR		0xa	/* blue/green/red */
#define	PCC_BGR_WB	0xb	/* blue/green/red or black&white */
#define	PCC_BGRW	0xc	/* blue/green/red/white */

struct PrinterExtendedData {
    char    *ped_PrinterName;    /* printer name, null terminated */
    VOID    (*ped_Init)();       /* called after LoadSeg */
    VOID    (*ped_Expunge)();    /* called before UnLoadSeg */
    int     (*ped_Open)();       /* called at OpenDevice */
    VOID    (*ped_Close)();      /* called at CloseDevice */
    UBYTE   ped_PrinterClass;    /* printer class */
    UBYTE   ped_ColorClass;      /* color class */
    UBYTE   ped_MaxColumns;      /* number of print columns available */
    UBYTE   ped_NumCharSets;     /* number of character sets */
    UWORD   ped_NumRows;         /* number of raster rows in a raster dump */
    ULONG   ped_MaxXDots;        /* number of dots maximum in a raster dump */
    ULONG   ped_MaxYDots;        /* number of dots maximum in a raster dump */
    UWORD   ped_XDotsInch;       /* horizontal dot density */
    UWORD   ped_YDotsInch;       /* vertical dot density */
    char    ***ped_Commands;     /* printer text command table */
    VOID    (*ped_DoSpecial)();  /* special command handler */
    VOID    (*ped_Render)();     /* raster render function */
    LONG    ped_TimeoutSecs;     /* good write timeout */
    /* the following only exists if the segment version is 33 or greater */
    char    **ped_8BitChars;     /* conversion strings for the extended font */
};

struct PrinterSegment {
    ULONG   ps_NextSegment;      /* (actually a BPTR) */
    ULONG   ps_runAlert;         /* MOVEQ #0,D0 : RTS */
    UWORD   ps_Version;          /* segment version */
    UWORD   ps_Revision;         /* segment revision */
    struct  PrinterExtendedData ps_PED;   /* printer extended data */
};
#endif
