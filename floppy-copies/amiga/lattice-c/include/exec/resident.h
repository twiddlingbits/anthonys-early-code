#ifndef	EXEC_RESIDENT_H
#define	EXEC_RESIDENT_H
/*********************************************************************
*
*   Commodore Amiga -- ROM Operating System Executive Include File
*
**********************************************************************
*
*   Source Control:
*
*	$Header: resident.h,v 1.0 85/08/28 15:13:28 carl Exp $
*
*	$Locker:  $
*
*********************************************************************/

#ifndef EXEC_NODES_H
#include "exec/nodes.h"
#endif !EXEC_NODES_H

struct Resident {
    UWORD rt_MatchWord;		/* word to match on (ILLEGAL) 	*/
    struct Resident *rt_MatchTag; /* pointer to the above	*/
    APTR  rt_EndSkip;		/* address to continue scan 	*/
    UBYTE rt_Flags;  		/* various tag flags 		*/
    UBYTE rt_Version;		/* release version number 	*/
    UBYTE rt_Type;   		/* type of module (NT_mumble) 	*/
    BYTE  rt_Pri;    		/* initialization priority */
    char  *rt_Name;   		/* pointer to node name 	*/
    char  *rt_IdString;		/* pointer to ident string 	*/
    APTR  rt_Init;   		/* pointer to init code 	*/
};

#define	RTC_MATCHWORD	0x4AFC

#define	RTF_AUTOINIT	(1<<7)
#define RTF_COLDSTART   (1<<0)

/* Compatibility: */
#define	RTM_WHEN 	3
#define	RTW_NEVER 	0
#define	RTW_COLDSTART 	1

#endif
