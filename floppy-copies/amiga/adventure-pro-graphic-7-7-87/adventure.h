
#define MAXRMS 50      /* number of rooms to reserve space for */
#define MAXLIST 100
#define ITEMSPACE 200
#define MAXCODE 2000   /* maximum length of program code */
#define ITEMSINROOM 20

/* define tokens */

#define  FIN -1
#define VERB 1
#define NOUN 2
#define ADJ 3
#define PREP 4       /* numbers less than 10 reserved for
                        tokens followed by a list */
#define IF 255       /* not an actual token */
#define IFVAREQ 10   /*if variable equals */
#define IFVARGT 11   /* numbers 10-19 reserved for IF... */
#define IFVARLT 12
#define IFRMCON 13    /*if room contains */
#define IFINCON 14    /*if inventory contains */
#define TYPE    20
#define DONE    21
#define ENDIF   22
#define ASSIGN  30        /* eg., var=10 */
#define NEND    23
#define ADJEND  24
#define POEND   25
#define VEND    26
#define RMPLUS  40
#define RMMINUS 41
#define INPLUS  42
#define INMINUS 43
#define CONNECT 45
#define NUMTYPE   46
#define VARIABLE  50
#define NUMBER    51
#define END     52
#define TOROOM  53
#define INSTEAD 54
#define NOUNREP 55
#define OR      60
#define AND     61
#define DESTROY 62
#define LOADG   70
#define LOADS   71
#define LOADM   72
#define USEG    73
#define USES    74
#define USEM    75
#define MAIN       76
#define BACKGROUND 77
#define GRAPHIC 80
#define TEXT    81



struct location {
   char desc[MAXRMS][500];
   int n[MAXRMS],s[MAXRMS],e[MAXRMS],w[MAXRMS];
   int u[MAXRMS],d[MAXRMS];
   int item[MAXRMS][MAXLIST];
};

struct loc2 {
   char *desc[MAXRMS];
   int n[MAXRMS],s[MAXRMS],e[MAXRMS],w[MAXRMS];
   int u[MAXRMS],d[MAXRMS];
   int item[MAXRMS][MAXLIST];
};

struct winprint {
   struct Window *WIN;
   int x,y;
   int stopnum;
};

struct graphic {
   UBYTE *bitmap;
   SHORT w,h;
   int planenum;
};


