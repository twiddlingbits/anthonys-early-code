#ifndef	EXEC_LISTS_H
#define	EXEC_LISTS_H
/*********************************************************************
*
*   Commodore Amiga -- ROM Operating System Executive Include File
*
**********************************************************************
*
*   Source Control:
*
*	$Header: lists.h,v 33.1 86/03/07 17:30:49 neil Exp $
*
*	$Locker:  $
*
*********************************************************************/

#ifndef EXEC_NODES_H
#include "exec/nodes.h"
#endif !EXEC_NODES_H


/* normal, full featured list */
struct List { 
    struct  Node *lh_Head;
    struct  Node *lh_Tail;
    struct  Node *lh_TailPred;
    UBYTE   lh_Type;
    UBYTE   l_pad;
};

/* minimum list -- no type checking possible */
struct MinList {
    struct MinNode *mlh_Head;
    struct MinNode *mlh_Tail;
    struct MinNode *mlh_TailPred;
};

#endif
