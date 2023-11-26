/**
*
* The following structure is a UNIX file block that retains information about
* a file being accessed via the level 1 I/O functions.
*/
struct UFB
{
int ufbflg;		/* flags */
int ufbfh;		/* file handle */
};
#define NUFBS 40	/* number of UFBs defined */

/*
*
* UFB.ufbflg definitions
*
*/

#if MSDOS1
#define UFB_OP 0x80	/* file is open */
#define UFB_RA 0x40	/* reading is allowed */
#define UFB_WA 0x20	/* writing is allowed */
#define UFB_NT 0x10	/* access file with no translation */
#define UFB_AP 8	/* append mode flag */	
#else
#define UFB_RA 1	/* reading is allowed */
#define UFB_WA 2	/* writing is allowed */
#endif
#define UFB_NC 4	/* no close */

/*
*
* UFB.ufbtyp definitions
*
*/

#if MSDOS1
#define D_DISK 0
#define D_CON 1
#define D_PRN 2
#define D_AUX 3
#define D_NULL 4
#endif

/**
*
* External definitions
*
*/

#ifndef NARGS
extern struct UFB *chkufb(int);
#else
extern struct UFB *chkufb();
#endif

/**
*
* Define NULL if it's not already defined
*
*/

#ifndef NULL
#if SPTR
#define NULL 0			/* null pointer value */
#else
#define NULL 0L
#endif
#endif
