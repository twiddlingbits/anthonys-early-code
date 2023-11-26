#ifdef AMIGA
#include "exec/types.h"
#include "exec/ports.h"
#include "exec/lists.h"
#include "libraries/dos.h"
#endif
/**
*
* This header file supplies information needed to interface with the
* particular operating system and C compiler being used.
*
**/

/**
*
* The following symbols specify which operating system is being used.
*
*	CPM		Any CP/M OS
*	CPM80		CP/M for Intel 8080 or Zilog Z80
*	CPM86		CP/M for Intel 8086
*	CPM68		CP/M for Motorola 68000
*	MSDOS		Microsoft's MSDOS
*
* Note: CPM will be set to 1 for any of the above.
*
*	UNIX		"Standard" UNIX
*	MIBS		General Automation's MIBS OS
*
*/

#if CPM80
#define CPM 1
#endif
#if CPM86
#define CPM 1
#endif
#if CPM68
#define CPM 1
#endif
#if MSDOS
#define CPM 1
#endif


/**
*
* The following definitions specify the particular C compiler being used.
*
*	LATTICE		Lattice C compiler
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
typedef unsigned char byte;
#endif


/**
*
* Miscellaneous definitions
*
*/
#define SECSIZ 128		/* disk sector size */
#if CPM
#define DMA (char *)0x80	/* disk buffer address */
#endif

/**
*
* The following structure is a File Control Block.  Operating systems
* with CPM-like characteristics use the FCB to store information about
* a file while it is open.
*
*/
#if CPM
struct FCB
	{
	char fcbdrv;		/* drive code */
	char fcbnam[8];		/* file name */
	char fcbext[3];		/* file name extension */
#if MSDOS
	short fcbcb;		/* current block number */
	short fcblrs;		/* logical record size */
	long fcblfs;		/* logical file size */
	short fcbdat;		/* create/change date */
	char fcbsys[10];	/* reserved */
	char fcbcr;		/* current record number */
	long fcbrec;		/* random record number */
#else
	char fcbexn;		/* extent number */
	char fcbs1;		/* reserved */
	char fcbs2;		/* reserved */
	char fcbrc;		/* record count */
	char fcbsys[16];	/* reserved */
	char fcbcr;		/* current record number */
	short fcbrec;		/* random record number */
	char fcbovf;		/* random record overflow */
#endif
	};

#define FCBSIZ sizeof(struct FCB)
#endif

/**
*
* The following symbols define the sizes of file names and node names.
*
*/
#if CPM
#define FNSIZE 13	/* maximum file node size */
#define FMSIZE 64	/* maximum file name size */
#define FESIZE 4	/* maximum file extension size */
#else
#if AMIGA
#define FNSIZE 32	/* maximum file node size */
#define FMSIZE 64	/* maximum file name size */
#define FESIZE 32	/* maximum file extension size */
#else
#define FNSIZE 16	/* maximum file node size */
#define FMSIZE 64	/* maximum file name size */
#define FESIZE 4	/* maximum file extension size */
#endif
#endif

/**
*
* The following structures define the 8086 registers that are passed to
* various low-level operating system service functions.
*
*/
#if I8086
struct XREG
	{
	short ax,bx,cx,dx,si,di;
	};

struct HREG
	{
	byte al,ah,bl,bh,cl,ch,dl,dh;
	};

union REGS
	{
	struct XREG x;
	struct HREG h;
	};

struct SREGS
	{
	short es,cs,ss,ds;
	};

struct XREGS
	{
	short ax,bx,cx,dx,si,di,ds,es;
	};

union REGSS
	{
	struct XREGS x;
	struct HREG h;
	};

#endif


/**
*
* The following codes are returned by the low-level operating system service
* calls.  They are usually placed into _OSERR by the OS interface functions.
*
*/
#if MSDOS
#define E_FUNC 1		/* invalid function code */
#define E_FNF 2			/* file not found */
#define E_PNF 3			/* path not found */
#define E_NMH 4	 		/* no more file handles */
#define E_ACC 5			/* access denied */
#define E_IFH 6			/* invalid file handle */
#define E_MCB 7			/* memory control block problem */
#define E_MEM 8			/* insufficient memory */
#define E_MBA 9			/* invalid memory block address */
#define E_ENV 10		/* invalid environment */
#define E_FMT 11		/* invalid format */
#define E_IAC 12		/* invalid access code */
#define E_DATA 13		/* invalid data */
#define E_DRV 15		/* invalid drive code */
#define E_RMV 16		/* remove denied */
#define E_DEV 17		/* invalid device */
#define E_NMF 18		/* no more files */
#endif

/**
*
* This structure contains disk size information returned by the getdfs
* function.
*/
#ifdef MSDOS
struct DISKINFO
	{
	unsigned short free;	/* number of free clusters */
	unsigned short cpd;	/* clusters per drive */
	unsigned short spc;	/* sectors per cluster */
	unsigned short bps;	/* bytes per sector */
	};

#endif

#ifdef AMIGA
#define DISKINFO InfoData
#endif

/**
*
* The following structure is used by the dfind and dnext functions to
* hold file information.
*
*/
#ifdef MSDOS
struct FILEINFO
	{
	char resv[21];	  /* reserved */	
	char attr;	  /* actual file attribute */
	long time;	  /* file time  and date */
	long size;	  /* file size in bytes */
	char name[13];	  /* file name */
	};
#endif

#ifdef AMIGA
#define FILEINFO FileInfoBlock
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

#ifdef AMIGA
/**
*
* The following structure is used to keep track of currently allocated
* system memory
*
*/
struct MELT2
	{
	struct MELT2 *fwd;	/* points to next block */
	struct MELT2 *bwd;	/* points to previous block */
	unsigned size;		/* size of this block */
	};
#define MELT2SIZE sizeof(struct MELT2)
#endif

#ifdef MSDOS
/**
*
* The following structure is a device header.  It is copied to _OSCED
* when a critical error occurs.
*
*/
struct DEV
	{
	long nextdev;	/* long pointer to next device */
	short attr;	/* device attributes */
	short sfunc;	/* short pointer to strategy function */
	short ifunc;	/* short pointer to interrupt function */
	char name[8];	/* device name */
	};

/**
*
* The following structure contains country-dependent information returned
* by the getcdi function.
*
*/
struct CDI2		/* DOS Version 2 format */
	{
	short fdate;	/* date/time format */
			/* 0 => USA (h:m:s m/d/y) */
			/* 1 => Europe (h:m:s d/m/y) */
			/* 2 => Japan (h:m:s d:m:y) */
	char curr[2];	/* currency symbol and null */
	char sthou[2];	/* thousands separator and null */
	char sdec[2];	/* decimal separator and null */
	char resv[24];	/* reserved */
	};

struct CDI3		/* DOS Version 3 format */
	{
	short fdate;	/* date format */
			/* 0 => USA (m d y) */
			/* 1 => Europe (d m y) */
			/* 2 => Japan (d m y) */
	char curr[5];	/* currency symbol, null-terminated */
	char sthou[2];	/* thousands separator and null */
	char sdec[2];	/* decimal separator and null */
	char sdate[2];	/* date separator and null */
	char stime[2];	/* time separator and null */
	char fcurr;	/* currency format */
			/* Bit 0 => 0 if symbol precedes value */
			/*       => 1 if symbol follows value */
			/* Bit 1 => number of spaces between value */
			/*          and symbol */
	char dcurr;	/* number of decimals in currency */
	char ftime;	/* time format */
			/* Bit 0 => 0 if 12-hour clock */
			/*       => 1 if 24-hour clock */
	long pcase;	/* far pointer to case map function */
	char sdata[2];	/* data list separator and null */
	short resv[5];	/* reserved */
	};

union CDI
	{
	struct CDI2 v2;
	struct CDI3 v3;
	};

#endif

#ifdef AMIGA
/**
*
* The following structures are used with the AmigaDOS fork() and wait()
* functions
*
*/

struct ProcID {				/* packet returned from fork()  */
	struct ProcID *nextID;		/* link to next packet		*/
	struct Process *process;	/* process ID of child		*/
	int UserPortFlag;
	struct MsgPort *parent;		/* termination msg destination	*/
	struct MsgPort *child;		/* child process' task msg port	*/
	long seglist;			/* child process' segment list	*/
	};

struct FORKENV {
	long priority;			/* new process priority		*/
	long stack;			/* stack size for new process	*/
	long std_in;			/* stdin for new process	*/
	long std_out;			/* stdout for new process	*/
	long console;			/* console window for new process */
	struct MsgPort *msgport;	/* msg port to receive termination */
	};				/* message from child		*/

struct TermMsg {			/* termination message from child */
	struct Message msg;
	long class;			/* class == 0			*/
	short type;			/* message type == 0		*/
	struct Process *process;	/* process ID of sending task	*/
	long ret;			/* return value			*/
	};

#ifndef NARGS
extern int forkl(char *, char *,);
extern int forkv(char *, char **, struct FORKENV *, struct ProcID *);
extern int wait(struct ProcID *);
extern struct ProcID *waitm(struct ProcID **);
#else
extern int forkl();
extern int forkv();
extern int wait();
extern struct ProcID *waitm();
#endif
#endif

/**
*
* Level 0 I/O services
*
**/
#ifndef NARGS
extern void chgdta(char *);
extern int chgfa(char *, int);
extern int chgft(int, long);
extern int dclose(int);
extern int dcreat(char *, int);
extern int dcreatx(char *, int);
extern int dfind(struct FILEINFO *, char *, int);
extern int dnext(struct FILEINFO *);
extern int dopen(char *, int);
extern unsigned dread(int, char *, unsigned);
extern long dseek(int, long, int);
extern int dunique(char *, int);
extern unsigned dwrite(int, char *, unsigned);
extern int getcd(int,char *);
extern int getch(void);
extern int getche(void);
extern int getdfs(int, struct DISKINFO *);
extern char *getdta(void);
extern int getfa(char *);
extern int getfc(int, int *);
#ifdef AMIGA
extern long getft(char *);
#else
extern long getft(int);
#endif
extern int getvfy(void);
extern int kbhit(void);
extern int putch(int);
extern int rlock(int, long, long);
extern void rstdta(void);
extern void rstvfy(void);
extern int runlk(int, long, long);
extern void setvfy(void);
extern int ungetch(int);
#else
extern void chgdta();
extern int chgfa();
extern int chgft();
extern int dclose();
extern int dcreat();
extern int dcreatx();
extern int dfind();
extern int dnext();
extern int dopen();
extern unsigned dread();
extern long dseek();
extern int dunique();
extern unsigned dwrite();
extern int getcd();
extern int getch();
extern int getche();
extern int getdfs();
extern char *getdta();
extern int getfa();
extern int getfc();
extern long getft();
extern int getvfy();
extern int kbhit();
extern int putch();
extern int rlock();
extern void rstdta();
extern void rstvfy();
extern int runlk();
extern void setvfy();
extern int ungetch();
#endif

/**
*
* Miscellaneous external definitions
*
*/
#ifndef NARGS
extern int chgclk(unsigned char *);
extern int chgdsk(int);
extern char *envpack(char **, char **);
extern int envunpk(char *);
extern long ftpack(char *);
extern void ftunpk(long, char *);
extern int getbrk(void);
extern int getcdi(int, struct CDI3 *);
extern void getclk(unsigned char *);
extern int getdsk(void);
extern int getpf(char *, char *);
extern int getpfe(char *, char *);
extern unsigned inp(unsigned);
extern int int86(int, union REGS *, union REGS *);
extern int int86s(int, union REGSS *, union REGSS *);
extern int int86x(int, union REGS *, union REGS *, struct SREGS *);
extern int intdos(union REGS *, union REGS *);
extern int intdoss(union REGSS *, union REGSS *);
extern int intdosx(union REGS *, union REGS *, struct SREGS *);
extern int isibmat(void);
extern int isibmjr(void);
extern int isibmpc(void);
extern int isnet(void);
extern int isnetdc(int);
extern int isnetfh(int);
extern int isneton(void);
extern int onbreak(int(*)());
extern void onerror(int);
extern void outp(unsigned, unsigned);
extern void peek(unsigned, unsigned, char *, unsigned);
extern void poke(unsigned, unsigned, char *, unsigned);
extern int poserr(char *);
extern void rstbrk(void);
extern void rstdsk(void);
extern int setcdi(int);
extern void setbrk(void);
#else
extern int chgclk();
extern int chgdsk();
extern char *envpack();
extern int envunpk();
extern long ftpack();
extern void ftunpk();
extern int getbrk();
extern int getcdi();
extern void getclk();
extern int getdsk();
extern int getpf();
extern int getpfe();
extern unsigned inp();
extern int int86();
extern int int86s();
extern int int86x();
extern int intdos();
extern int intdoss();
extern int intdosx();
extern int isibmat();
extern int isibmjr();
extern int isibmpc();
extern int isnet();
extern int isnetdc();
extern int isnetfh();
extern int isneton();
extern int onbreak();
extern void onerror();
extern void outp();
extern void peek();
extern void poke();
extern int poserr();
extern void rstbrk();
extern void rstdsk();
extern int setcdi();
extern void setbrk();
#endif
/*
* Define NULL if it's not already defined
*
*/
#ifndef NULL
#define NULL 0L
#endif
