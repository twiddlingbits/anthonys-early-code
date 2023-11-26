#ifndef	EXEC_DEVICES_H
#define	EXEC_DEVICES_H
/*********************************************************************
*
*   Commodore Amiga -- ROM Operating System Executive Include File
*
**********************************************************************
*
*   Source Control:
*
*	$Header: devices.h,v 1.0 85/08/28 15:06:50 carl Exp $
*
*	$Locker:  $
*
*********************************************************************/

#ifndef EXEC_LIBRARIES_H
#include "exec/libraries.h"
#endif !EXEC_LIBRARIES_H

#ifndef EXEC_PORTS_H
#include "exec/ports.h"
#endif !EXEC_PORTS_H


/****** Device ******************************************************/

struct Device { 
    struct  Library dd_Library;
};


/****** Unit ********************************************************/

struct Unit {
    struct  MsgPort *unit_MsgPort;	/* queue for unprocessed messages */
    UBYTE   unit_flags;
    UBYTE   unit_pad;
    UWORD   unit_OpenCnt;		/* number of active opens */
};


#define UNITF_ACTIVE	(1<<0)
#define UNITF_INTASK	(1<<1)

#endif
