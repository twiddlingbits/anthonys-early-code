/**
*
* Level 3 memory allocation services
*
**/
#ifndef NARGS
extern char *malloc(unsigned);
extern char *calloc(unsigned,unsigned);
extern char *realloc(char*, unsigned);
extern int free(char *);
#else
extern char *malloc();
extern char *calloc();
extern char *realloc();
extern int free();
#endif

/**
*
* Level 2 memory allocation services
*
**/
#ifndef NARGS
extern int allmem(void);
extern char *getmem(unsigned);
extern char *getml(long);
extern int rlsmem(char *, unsigned);
extern int rlsml(char *, long);
extern int bldmem(int);
extern long sizmem(void);
extern long chkml(void);
extern void rstmem(void);
#else
extern char *getmem();
extern char *getml();
extern int rlsmem();
extern int rlsml();
extern int bldmem();
extern long sizmem();
extern long chkml();
extern void rstmem();
#endif

/**
*
* Level 1 memory allocation services
*
**/
#ifndef NARGS
extern char *sbrk(unsigned);
extern char *lsbrk(long);
extern void rbrk(void);
#else
extern char *sbrk();
extern char *lsbrk();
extern void rbrk();
#endif


/**
*
* Miscellaneous I/O services
*
*/
#ifndef NARGS
extern int access(char *, int);
extern int chdir(char *);
extern int chmod(char *, int);
extern int dup(int);
extern int dup2(int, int);
extern char *getcwd(char *, int);
extern int getext(char *, char *);
extern int isatty(int);
extern int mkdir(char *);
extern int perror(char *);
extern int rmdir(char *);
extern char *tmpnam(char *);
#else
extern int access();
extern int chdir();
extern int chmod();
extern int dup();
extern int dup2();
extern int getext();
extern char *getcwd();
extern int isatty();
extern int mkdir();
extern int perror();
extern int rmdir();
extern char *tmpnam();
#endif

/**
*
* Memory block services
*
**/
#ifndef NARGS
extern char *memccpy(char *, char *, int, unsigned);
extern char *memchr(char *, int, unsigned);
extern int memcmp(char *, char *, unsigned);
extern char *memcpy(char *, char *, unsigned);
extern char *memset(char *, int, unsigned);

extern void movmem(char *, char *, unsigned);
extern void repmem(char *, char *, int, int);
extern void setmem(char *, unsigned, int);
extern void swmem(char *, char *, unsigned);
#else
extern char *memccpy();
extern char *memchr();
extern int memcmp();
extern char *memcpy();
extern char *memset();

extern void movmem();
extern void repmem();
extern void setmem();
extern void swmem();
#endif

/**
*
* Sort functions
*
*/
#ifndef NARGS
extern void dqsort(double *, int);
extern void fqsort(float *, int);
extern void lqsort(long *, int);
extern void qsort(char *, int, int, int (*)());
extern void sqsort(short *, int);
extern void tqsort(char **, int);
#else
extern void dqsort();
extern void fqsort();
extern void lqsort();
extern void qsort();
extern void sqsort();
extern void tqsort();
#endif

/**
*
* fork/exec functions
*
*/
#ifndef NARGS
extern int execl(char *, char *,);
extern int execv(char *, char **);
extern int execle(char *, char *,);
extern int execve(char *, char **, char **);
extern int execlp(char *, char *,);
extern int execvp(char *, char **);
extern int execlpe(char *, char *,);
extern int execvpe(char *, char **, char **);
#ifndef AMIGA
extern int forkl(char *, char *,);
extern int forkv(char *, char **);
extern int forkle(char *, char *,);
extern int forkve(char *, char **, char **);
extern int forklp(char *, char *,);
extern int forkvp(char *, char **);
extern int forklpe(char *, char *,);
extern int forkvpe(char *, char **, char **);

extern int wait(void);
extern int system(char *);
#endif
#endif

/**
*
* Miscellaneous functions
*
*/
#ifndef NARGS
extern void abort(void);
extern char *argopt(int, char**, char *, int *, char *);
extern int atoi(char *);
extern long atol(char *);
extern char *ecvt(double, int, int *, int *);
extern void exit(int);
extern void _exit(int);
extern char *fcvt(double, int, int *, int *);
extern unsigned FP_OFF(long);
extern unsigned FP_SEG(long);
extern char *getenv(char *);
extern char *gcvt(double, int, char *);
extern int getfnl(char *, char *, unsigned, int);
extern int getpid(void);
extern int iabs(int);
extern int isauto(char *);
extern int isdata(char *, unsigned);
extern int isdptr(char *);
extern int isheap(char *);
extern int ispptr(int(*)());
extern int isstatic(char *);
extern long labs(long);
extern void makedv(char *, unsigned *, unsigned *);
extern void makepv(int(*)(), unsigned *, unsigned *);
extern void movedata(unsigned, unsigned, unsigned, unsigned, unsigned);
extern int onexit(int(*)());
extern int putenv(char *);
extern int rmvenv(char *);
#else
extern void abort();
extern char *argopt();
extern int atoi();
extern long atol();
extern char *ecvt();
extern void exit();
extern void _exit();
extern char *fcvt();
extern unsigned FP_OFF();
extern unsigned FP_SEG();
extern char *gcvt();
extern char *getenv();
extern int getfnl();
extern int getpid();
extern int iabs();
extern int isauto();
extern int isdata();
extern int isdptr();
extern int isheap();
extern int ispptr();
extern int isstatic();
extern long labs();
extern void makedv();
extern void makepv();
extern void movedata();
extern int onexit();
#endif
/*
* define NULL if it's not already defined
*
*/
#ifndef NULL
#define NULL 0L
#endif
