
#define MAXRMS 50      /* number of rooms to reserve space for */
#define MAXLIST 100
#define ITEMSPACE 200
#define MAXCODE 5000   /* maximum length of program code */
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
#define LOADS   63
#define PLAYS   64
#define STOPS   65
#define CLEARS  66
#define WAIT    67
#define DG      70
#define DS      71
#define ACTS    72
#define ENDS    73
#define INJECT  74
#define CRMG    75
#define CRMS    78
#define COBJG   79
#define MAIN       76
#define BACKGROUND 77
#define GRAPHIC 80
#define TEXT    81
#define SPEAK   90
#define SLAVE   91
#define UNSLAVE 92
#define OPEN    93
#define CLOSEF  94
#define READ    95
#define WRITE   96
#define STOREG  97
#define LOADG   98
#define SDONE   100
#define NDONE   101
#define REDRAW  102


struct loc2 {
   int desc[MAXRMS];
   int pic[MAXRMS];
   int n[MAXRMS],s[MAXRMS],e[MAXRMS],w[MAXRMS];
   int u[MAXRMS],d[MAXRMS];
   int item[MAXRMS][MAXLIST];
};

struct winprint {
   struct Window *WIN;
   int X,Y,W,H;
   int x,y;
   int stopnum;
};

struct graphic {
   UBYTE *bitmap;
   SHORT w,h;
   int planenum;
};

#define PERM 1
#define ACTIVATED 2
#define REPEAT 4
#define ON 8
#define SIMPLE 16
#define SUPER 32

   struct sequence {
   char speed,number,count;
   char framenum,backnum,room;
   int flags;
   char cell,repeat;
   int sec[10],x[10],y[10];
   char *names[10];
   struct Image *graphics[10];
   struct BitMap background;
};


