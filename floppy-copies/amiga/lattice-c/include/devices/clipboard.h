#ifndef     DEVICES_CLIPBOARD_H
#define     DEVICES_CLIPBOARD_H
/********************************************************************/
/*               Commodore-Amiga, Inc.                              */
/*               clipboard.h                                        */
/********************************************************************/
/*********************************************************************
 *
 * clipboard device command definitions
 *
 ********************************************************************/

#ifndef     EXEC_NODES_H
#include "exec/nodes.h"
#endif
#ifndef     EXEC_LISTS_H
#include "exec/lists.h"
#endif
#ifndef     EXEC_PORTS_H
#include "exec/ports.h"
#endif

#define  CBD_POST             (CMD_NONSTD+0)
#define  CBD_CURRENTREADID    (CMD_NONSTD+1)
#define  CBD_CURRENTWRITEID   (CMD_NONSTD+2)

#define  CBERR_OBSOLETEID     1


struct ClipboardUnitPartial {
    struct  Node cu_Node;     /* list of units */
    ULONG   cu_UnitNum;       /* unit number for this unit */
    /* the remaining unit data is private to the device */
};


struct IOClipReq {
    struct  Message io_Message;
    struct  Device  *io_Device; /* device node pointer  */
    struct  Unit    *io_Unit; /* unit (driver private)*/
    UWORD   io_Command;       /* device command */
    UBYTE   io_Flags;         /* including QUICK and SATISFY */
    BYTE    io_Error;         /* error or warning num */
    ULONG   io_Actual;        /* number of bytes transferred */
    ULONG   io_Length;        /* number of bytes requested */
    STRPTR  io_Data;          /* either clip stream or post port */
    ULONG   io_Offset;        /* offset in clip stream */
    LONG    io_ClipID;        /* ordinal clip identifier */
};

#define  PRIMARY_CLIP   0     /* primary clip unit */

struct SatisfyMsg {
    struct  Message sm_Msg;      /* the length will be 6 */
    UWORD   sm_Unit;          /* which clip unit this is */
    LONG    sm_ClipID;        /* the clip identifier of the post */
};

#endif
