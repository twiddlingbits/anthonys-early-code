/****************************************************************
*                                                               *
* Copyright 1985, Commodore Amiga Inc.  All rights reserved.    *
* No part of this program may be reproduced, transmitted,       *
* transcribed, stored in retrieval system, or translated into   *
* any language or computer language, in any form or by any      *
* means, electronic, mechanical, magnetic, optical, chemical,   *
* manual or otherwise, without the prior written permission of  *
* Commodore-Amiga Incorporated, 983 University Ave. Building #D,*
* Los Gatos, California, 95030                                  *
*                                                               *
*****************************************************************


*****************************************************************
*
* parallel.h -- external declarations for Serial Port Driver
*
* SOURCE CONTROL
* ------ -------
* $Header: parallel.h,v 25.0 85/03/27 19:14:15 tomp Exp $
*
* $Locker:  $
*
*****************************************************************/

#ifndef DEVICES_PARALLEL_H
#define DEVICES_PARALLEL_H

#ifndef   EXEC_IO_H
#include "exec/io.h"
#endif   !EXEC_IO_H 

 struct  IOPArray {
        ULONG PTermArray0;
        ULONG PTermArray1;
};

/******************************************************************/
/* CAUTION !!  IF YOU ACCESS the parallel.device, you MUST (!!!!) use
   an IOExtPar-sized structure or you may overlay innocent memory !! */
/******************************************************************/
 
 struct   IOExtPar {
        struct   IOStdReq IOPar;

/*     STRUCT   MsgNode
*   0   APTR     Succ
*   4   APTR     Pred
*   8   UBYTE    Type
*   9   UBYTE    Pri
*   A   APTR     Name
*   E   APTR     ReplyPort
*  12   UWORD    MNLength
*     STRUCT   IOExt
*  14   APTR     io_Device
*  18   APTR     io_Unit
*  1C   UWORD    io_Command
*  1E   UBYTE    io_Flags
*  1F   UBYTE    io_Error
*     STRUCT   IOStdExt
*  20   ULONG    io_Actual
*  24   ULONG    io_Length
*  28   APTR     io_Data
*  2C   ULONG    io_Offset
*  30 */
        ULONG   io_PExtFlags;    /* (not used) flag extension area */
        UBYTE   io_Status;       /* status of parallel port and registers */
        UBYTE   io_ParFlags;     /* see PARFLAGS bit definitions below */
        struct  IOPArray io_PTermArray; /* termination character array */
};

#define	PARB_SHARED    5           /* ParFlags non-exclusive access bit */
#define	PARF_SHARED    (1<<5)      /*    "     non-exclusive access mask */
#define PARB_RAD_BOOGIE 3          /*    "     (not yet implemented) */
#define PARF_RAD_BOOGIE (1<<3)     /*    "     (not yet implemented) */
#define PARB_EOFMODE   1           /*    "     EOF mode enabled bit */
#define PARF_EOFMODE   (1<<1)      /*    "     EOF mode enabled mask */
#define IOPARB_QUEUED  6           /* IO_FLAGS rqst-queued bit */
#define IOPARF_QUEUED  (1<<6)      /*    "     rqst-queued mask */
#define	IOPARB_ABORT   5           /*    "     rqst-aborted bit */
#define	IOPARF_ABORT   (1<<5)      /*    "     rqst-aborted mask */
#define	IOPARB_ACTIVE  4           /*    "     rqst-qued-or-current bit */
#define	IOPARF_ACTIVE  (1<<4)      /*    "     rqst-qued-or-current mask */
#define	IOPTB_RWDIR    3           /* IO_STATUS read=0,write=1 bit */
#define	IOPTF_RWDIR    (1<<3)      /*    "     read=0,write=1 mask */
#define	IOPTB_PBUSY    2           /*    "     printer in busy toggle bit */
#define	IOPTF_PBUSY    (1<<2)      /*    "     printer in busy toggle mask */
#define	IOPTB_PAPEROUT 1           /*    "     paper out bit */
#define	IOPTF_PAPEROUT (1<<1)      /*    "     paper out mask */
#define	IOPTB_PSEL     0           /*    "     printer selected bit */
#define	IOPTF_PSEL     (1<<0)      /*    "     printer selected mask */

#define PARALLELNAME            "parallel.device"

#define PDCMD_QUERY             (CMD_NONSTD)
#define PDCMD_SETPARAMS         (CMD_NONSTD+1)

#define ParErr_DevBusy	       	1
#define ParErr_BufTooBig     	2
#define ParErr_InvParam      	3
#define ParErr_LineErr       	4
#define ParErr_NotOpen          5
#define ParErr_PortReset        6
#define ParErr_InitErr         	7

#endif !DEVICES_PARALLEL_H
