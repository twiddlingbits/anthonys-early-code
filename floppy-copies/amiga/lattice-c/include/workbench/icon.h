
#ifndef LIBRARIES_ICON_H
#define LIBRARIES_ICON_H

/*****************************************************************
*                                                                *
* Copyright (C) 1985, Commodore Amiga Inc.  All rights reserved. *
*                                                                *
*****************************************************************/


/****************************************************************
*
* icon.h -- external declarations for workbench support library
*
* SOURCE CONTROL
* ------ -------
* $Header: icon.h,v 33.2 86/03/15 16:28:18 neil Exp $
*
* $Locker:  $
*
****************************************************************/


/********************************************************************
*
* library structures
*
********************************************************************/


#define	ICONNAME	"icon.library"

/********************************************************************
*
* function types
*
********************************************************************/

struct WBObject *GetWBObject(), *AllocWBObject();
struct DiskObject *GetDiskObject();
LONG PutWBObject(), PutIcon(), GetIcon(), MatchToolValue();
VOID FreeFreeList(), FreeWBObject(), AddFreeList();
char *FindToolType();


#endif !LIBRARIES_ICON_H
