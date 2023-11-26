

/*****************************************************************
*                                                                *
* Copyright (C) 1985, Commodore Amiga Inc.  All rights reserved. *
*                                                                *
*****************************************************************/


/****************************************************************
*
* SOURCE CONTROL
* ------ -------
* $Header: timer.h,v 27.1 85/06/24 13:32:37 neil Exp $
*
* $Locker:  $
*
****************************************************************/

#ifndef DEVICES_TIMER_H
#define DEVICES_TIMER_H

#ifndef EXEC_IO_H
#include "exec/io.h"
#endif EXEC_IO_H

/* unit defintions */
#define UNIT_MICROHZ	0
#define UNIT_VBLANK	1

#define TIMERNAME	"timer.device"

struct timeval {
    ULONG tv_secs;
    ULONG tv_micro;
};

struct timerequest {
    struct IORequest tr_node;
    struct timeval tr_time;
};

/* IO_COMMAND to use for adding a timer */
#define TR_ADDREQUEST	CMD_NONSTD
#define TR_GETSYSTIME	(CMD_NONSTD+1)
#define TR_SETSYSTIME	(CMD_NONSTD+2)

#endif DEVICES_TIMER_H
