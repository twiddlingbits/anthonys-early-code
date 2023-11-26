
/*** expansion.h **********************************************************
*
*  external definitions for expansion.resource
*
* Copyright (C) 1985,  Commodore-Amiga, Inc., All rights reserved.
*
* $Header: expansion.h,v 33.2 86/04/25 12:15:40 neil Exp $
*
* $Locker:  $
*
* $Log:	expansion.h,v $
* Revision 33.2  86/04/25  12:15:40  neil
* Added MakeDosNode/AddDosNode; changed Get/SetCurrentBinding
* 
* Revision 33.1  86/04/01  12:17:24  neil
* First version
* 
*
****************************************************************************/

#ifndef LIBRARIES_EXPANSION_H
#define	LIBRARIES_EXPANSION_H	1

#define EXPANSIONNAME	"expansion.library"

/* flags for the AddDosNode() call */
#define ADNB_STARTPROC	0

#define ADNF_STARTPROC	(1<<0)


/* correct types for C programs */

struct ConfigDev *AllocConfigDev();
CPTR AllocExpansionMem();
struct ConfigDev *FindConfigDev();
struct DeviceNode *MakeDosNode()

#endif !LIBRARIES_EXPANSION_H
