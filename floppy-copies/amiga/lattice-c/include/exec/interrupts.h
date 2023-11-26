#ifndef	EXEC_INTERRUPTS_H
#define	EXEC_INTERRUPTS_H
/*********************************************************************
*
*   Commodore Amiga -- ROM Operating System Executive Include File
*
**********************************************************************
*
*   Source Control:
*
*	$Header: interrupts.h,v 33.1 86/04/08 21:55:43 neil Exp $
*
*	$Locker:  $
*
*********************************************************************/

#ifndef EXEC_NODES_H
#include "exec/nodes.h"
#endif !EXEC_NODES_H

#ifndef EXEC_LISTS_H
#include "exec/lists.h"
#endif !EXEC_LISTS_H


struct Interrupt {
    struct  Node is_Node; 
    APTR    is_Data;		    /* server data segment  */
    VOID    (*is_Code)();	    /* server code entry    */
};


struct IntVector {		/* For EXEC use ONLY! */
    APTR    iv_Data;
    VOID    (*iv_Code)();
    struct  Node *iv_Node;
};


struct SoftIntList {		/* For EXEC use ONLY! */
    struct List sh_List;
    UWORD  sh_Pad;
};

#define SIH_PRIMASK (0xf0)

/* this is a fake INT definition, used only for AddIntServer and the like */
#define INTB_NMI	15
#define INTF_NMI	(1<<15)

#endif
