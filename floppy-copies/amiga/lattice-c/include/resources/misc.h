
#ifndef RESOURCES_MISC_I
#define RESOURCES_MISC_I

/*****************************************************************
*                                                                *
* Copyright (C) 1985, Commodore Amiga Inc.  All rights reserved. *
*                                                                *
*****************************************************************/


/****************************************************************
*
* misc.i -- external declarations for misc system resources
*
* SOURCE CONTROL
* ------ -------
* $Header: misc.h,v 27.3 85/07/12 16:28:29 neil Exp $
*
* $Locker:  $
*
****************************************************************/

#ifndef EXEC_TYPES_H
#include "exec/types.h" 
#endif	!EXEC_TYPES_H

#ifndef EXEC_LIBRARIES_H
#include "exec/libraries.h" 
#endif	!EXEC_LIBRARIES_H


/********************************************************************
*
* Resource structures
*
********************************************************************/

#define	MR_SERIALPORT	0
#define	MR_SERIALBITS	1
#define	MR_PARALLELPORT	2
#define	MR_PARALLELBITS	3

#define NUMMRTYPES	4

struct MiscResource {
    struct Library mr_Library;
    ULONG mr_AllocArray[NUMMRTYPES];
};

#define	MR_ALLOCMISCRESOURCE	(LIB_BASE)
#define MR_FREEMISCRESOURCE	(LIB_BASE + LIB_VECSIZE)


#define MISCNAME "misc.resource"

#endif	!RESOURCES_MISC_H
