
/*** libraries/filehandler.h **********************************************
*
*  device and file handler specific code for AmigaDOS
*
* Copyright (C) 1986,  Commodore-Amiga, Inc., All rights reserved.
*
* $Header: filehandler.h,v 33.3 86/05/21 19:14:02 neil Exp $
*
* $Locker:  $
*
****************************************************************************/

#ifndef LIBRARIES_FILEHANDLER_H
#define	LIBRARIES_FILEHANDLER_H	1

#ifndef	  EXEC_TYPES_H
#include "exec/types.h"
#endif	 !EXEC_TYPES_H

#ifndef	  EXEC_PORTS_H
#include "exec/ports.h"
#endif	 !EXEC_PORTS_H

#ifndef	  LIBRARIES_DOS_H
#include "libraries/dos.h"
#endif	 !LIBRARIES_DOS_H


/* The disk "environment" is a longword array that describes the
** disk geometry.  It is variable sized, with the length at the beginning.
** Here are the constants for a standard geometry.
*/

/* these are the offsets into the array */
#define	DE_TABLESIZE	0	/* standard value is 11 */
#define	DE_SIZEBLOCK	1	/* in longwords: standard value is 128 */
#define DE_SECORG	2	/* not used; must be 0 */
#define DE_NUMHEADS	3	/* # of heads (surfaces). drive specific */
#define DE_SECSPERBLK	4	/* not used; must be 1 */
#define DE_BLKSPERTRACK	5	/* blocks per track. drive specific */
#define DE_RESERVEDBLKS	6	/* unavailable blocks at start.  usually 2 */
#define DE_PREFAC	7	/* not used; must be 0 */
#define DE_INTERLEAVE	8	/* usually 0 */
#define	DE_LOWCYL	9	/* starting cylinder. typically 0 */
#define DE_UPPERCYL	10	/* max cylinder.  drive specific */
#define DE_NUMBUFFERS	11	/* starting # of buffers.  typically 5 */
#define DE_MEMBUFTYPE	12	/* type of mem to allocate for buffers.
				** default is 3, hard disks want 0 */

/* The file system startup message is linked into a device node's startup
** field.  It contains a pointer to the above environment, plus the
** information needed to do an exec OpenDevice().
*/
struct FileSysStartupMsg {
    ULONG	fssm_Unit;	/* exec unit number for this device */
    BSTR	fssm_Device;	/* null terminated bstring to the device name */
    BPTR	fssm_Environ;	/* ptr to environment table (see above) */
    ULONG	fssm_Flags;	/* flags for OpenDevice() */
};


/* The include file "libraries/dosextens.h" has a DeviceList structure.
** The "device list" can have one of three different things linked onto
** it.  Dosextens defines the structure for a volume.  DLT_DIRECTORY
** is for an assigned directory.  The following structure is for
** a dos "device" (DLT_DEVICE).
*/
struct DeviceNode {
    BPTR	dn_Next;	/* singly linked list */
    ULONG	dn_Type;	/* always 0 for dos "devices" */
    struct MsgPort *dn_Task;	/* standard dos "task" field.  If this is
				 * null when the node is accesses, a task
				 * will be started up
				 */
    BPTR	dn_Lock;	/* not used for devices -- leave null */
    BSTR	dn_Handler;	/* filename to loadseg (if seglist is null) */
    ULONG	dn_StackSize;	/* stacksize to use when starting task */
    LONG	dn_Priority;	/* task priority when starting task */
    BPTR	dn_Startup;	/* startup msg: FileSysStartupMsg for disks */
    BPTR	dn_SegList;	/* code to run to start new task (if necessary).
				 * if null then dn_Handler will be loaded.
				 */
    BPTR	dn_GlobalVec;	/* BCPL global vector to use when starting
				 * a task.  -1 means that dn_SegList is not
				 * for a bcpl program, so the dos won't
				 * try and construct one.  0 tell the
				 * dos that you obey BCPL linkage rules,
				 * and that it should construct a global
				 * vector for you.
				 */
    BSTR	dn_Name;	/* the node name, e.g. '\3','D','F','3' */
};


    




#endif !LIBRARIES_FILEHANDLER_H
