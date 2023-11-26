/**
*
* This header file supplies information needed to interface with the
* particular operating system and C compiler being used.
*
**/

/**
*
* The following definitions specify the particular C compiler being used.
*
*	LATTICE		Lattice C compiler
*	BDS		BDS C compiler
*	BTL		Bell Labs C compiler or equivalent
*	MANX		MANX Aztec C compiler
*
*/
#define LATTICE 1



/**
*
* The following type definitions take care of the particularly nasty
* machine dependency caused by the unspecified handling of sign extension
* in the C language.  When converting "char" to "int" some compilers
* will extend the sign, while others will not.  Both are correct, and
* the unsuspecting programmer is the loser.  For situations where it
* matters, the new type "byte" is equivalent to "unsigned char".
*
*/
#if LATTICE
typedef char byte;
#endif

#if BDS
#define byte char
#endif

#if BTL
typedef unsigned char byte;
#endif

#if MANX
#define byte char
#endif

/**
*
* Miscellaneous definitions
*
*/
#define SECSIZ 128		/* disk sector size */

/**
*
* The following structure is a File Control Block.  Operating systems
* with CPM-like characteristics use the FCB to store information about
* a file while it is open.
*
*/
struct FCB
	{
	char fcbdrv;		/* drive code */
	char fcbnam[8];		/* file name */
	char fcbext[3];		/* file name extension */
	char fcbexn;		/* extent number */
	char fcbs1;		/* reserved */
	char fcbs2;		/* reserved */
	char fcbrc;		/* record count */
	char fcbsys[16];	/* reserved */
	char fcbcr;		/* current record number */
	short fcbrec;		/* random record number */
	char fcbovf;		/* random record overflow */
	};

#define FCBSIZ sizeof(struct FCB)

/**
*
* The following symbols define the sizes of file names and node names.
*
*/
#define FNSIZE 30	/* maximum file node name size */
#define FMSIZE 30	/* maximum file name size */


/**
*
* The following codes are used to open files in various modes.
*
*/
#if LATTICE
#define OPENR 0x8000		/* open for reading */
#define OPENW 0x8001		/* open for writing */
#define OPENU 0x8002		/* open for read/write */
#define OPENC 0x8001		/* create and open for writing */
#else 
#define OPENR 0
#define OPENW 1
#define OPENU 2
#endif

/**
*
* The following structure appears at the beginning (low address) of
* each free memory block.
*
*/
struct MELT
	{
	struct MELT *fwd;	/* points to next free block */
#if SPTR
	unsigned size;		/* number of MELTs in this block */
#else
	long size;		/* number of MELTs in this block */
#endif
	};
#define MELTSIZE sizeof(struct MELT)
