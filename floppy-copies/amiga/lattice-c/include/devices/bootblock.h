#ifndef	DEVICES_BOOTBLOCK_H
#define	DEVICES_BOOTBLOCK_H
/************************************************************************
*									*
*	Copyright (C) 1985, Commodore Amiga Inc.  All rights reserved.	*
*									*
************************************************************************/


/************************************************************************
*
* bootblock.h
*
* Source Control
* ------ -------
* 
* $Header: bootblock.h,v 1.5 86/04/21 15:03:58 barry Exp $
*
* $Locker:  $
*
************************************************************************/

/******* BootBlock definition: */

struct BootBlock {
	UBYTE	bb_id[4];		/* 4 character identifier */
	LONG	bb_chksum;	/* boot block checksum (balance) */
	LONG	bb_dosblock;	/* reserved for DOS patch */
};

#define		BOOTSECTS	2	/* 1K bootstrap */

#define BBID_DOS	{ 'D', 'O', 'S', '\0' }
#define BBID_KICK	{ 'K', 'I', 'C', 'K' }

#define BBNAME_DOS	(('D'<<24)|('O'<<16)|('S'<<8))
#define BBNAME_KICK	(('K'<<24)|('I'<<16)|('C'<<8)|('K'))

#endif
