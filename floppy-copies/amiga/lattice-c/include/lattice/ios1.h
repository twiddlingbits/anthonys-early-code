/**
*
* The following structure is a UNIX file block that retains information about
* a file being accessed via the level 1 I/O functions.
*/
struct UFB
{
char ufbflg;		/* flags */
char ufbtyp;
int ufbfh;		/* file handle */
};
#define NUFBS 20	/* number of UFBs defined */

/*
*
* UFB.ufbflg definitions
*
*/
#define UFB_OP 0x80	/* file is open */
#define UFB_RA 0x40	/* reading is allowed */
#define UFB_WA 0x20	/* writing is allowed */
#define UFB_NT 0x10	/* access file with no translation */
#define UFB_AP 8	/* append mode flag */	
#define UFB_NC 4	/* no-close flag */

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
