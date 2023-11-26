/* text editor definitions */

#define MEMEXTRA 5000

struct textwin {
int x,y,w,h;
int maxx,maxy;
struct Window *win;
int stx,sty;
int letw,leth;
};

struct screenline {
char letters[100];
SHORT pos[100];
char att[100];
SHORT font[100];
int baseline,h,d,l;
int start;
};

struct linedata {
int start[50];
int att[50],*font[50];
struct screenline *next;
};


