/**
*
* The following symbols are used for the "open" and "creat" functions.
* They are generally UNIX-compatible, except for O_APPEND under MSDOS,
* which has been moved in order to accomodate the file sharing flags
* defined in MSDOS Version 3.
*
* Also, O_TEMP, O_UNIQ, and O_RAW are Lattice extensions.
*
**/
#define O_RDONLY 0	/* Read-only value (right byte of mode word) */
#define O_WRONLY 1	/* Write-only value */
#define O_RDWR 2	/* Read-write value */

#if MSDOS
#define O_NDELAY 0	/* Non-blocking I/O flag (N/A) */
#else
#define O_NDELAY 4	/* Non-blocking I/O flag */
#endif

#define O_APPEND 8	/* Append mode flag */

#if MSDOS
#define O_SDRW 0x10	/* Share mode: deny read/write */
#define O_SDW 0x20	/* Share mode: deny write */
#define O_SDR 0x30	/* Share mode: deny read */
#define O_SDN 0x40	/* Share mode: deny none */
#define O_PVT 0x80	/* Private file (not passed to child) */
#endif

#define O_CREAT 0x0100	/* File creation flag */
#define O_TRUNC 0x0200	/* File truncation flag */
#define O_EXCL 0x0400	/* Exclusive access flag */

#ifndef AMIGA
#define O_UNIQ 0x2000	/* Unique file flag (Lattice feature) */
#define O_TEMP 0x4000	/* Temporary file flag (Lattice feature) */
#endif

#define O_RAW 0x8000	/* Raw I/O flag (Lattice feature) */

/**
*
* The following flags are used to establish the protection mode.  Note
* that under MSDOS only write-protection is meaningful.
*
*
*/
#ifdef AMIGA
#define S_IREAD  8
#define S_IWRITE  4
#define S_IEXECUTE 2
#define S_IDELETE  1
#else
#define S_IWRITE 0200
#define S_IREAD 0400
#endif
/**
*
* The following symbols are used for the "fcntl" function.
*
*/
#define F_DUPFD 0	/* Duplicate file descriptor */
#define F_GETFD 1	/* Get file descriptor flags */
#define F_SETFD 2	/* Set file descriptor flags */
#define F_GETFL 3	/* Get file flags */
#define F_SETFL 4	/* Set file flags */

/**
*
* External definitions
*
**/
#ifndef NARGS
extern int dup(int);
extern int dup2(int, int);
extern int open(char *, int, );
extern int creat(char *, int);
extern int unlink(char *);
extern int remove(char *);
extern int rename(char *, char *);
extern unsigned read(int, char *, unsigned);
extern unsigned write(int, char *, unsigned);
extern long lseek(int, long, int);
extern long tell(int);
extern int close(int);
extern int iomode(int, int);
#else
extern int dup();
extern int dup2();
extern int open();
extern int creat();
extern int unlink();
extern int remove();
extern int rename();
extern unsigned read();
extern unsigned write();
extern long lseek();
extern long tell();
extern int close();
extern int iomode();
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
