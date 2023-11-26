#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <lattice/stdio.h>
#include "adventure.h"

struct IntuitionBase *IntuitionBase;
char command[80],word[80];
struct loc2 rm;
char *nounlist[MAXLIST];
char *verblist[MAXLIST];
char *adjlist[MAXLIST];
char *extlist[MAXLIST];
char *ext2list[MAXRMS];
char *ext3list[MAXLIST];
char *prepolist[MAXLIST];
char *typelist[MAXLIST];

main()
{
FILE *fp;
int i,ii,iq,k,rk,rk2,r,codekount,token,prev_rm,rnum,nnum;
int nrflag,orflag,andflag;
int screenw,screenh,numplanes,picx,picy;
int winx,winy,winw,winh;
char c,cn,terminator;
static char code[MAXCODE];
char thing[200];
int inmode,fragend,mainnum,varval[MAXLIST];
int backnum,objnum[2],objx[2][10],objy[2][10],objlist[100];
static int noun_pics[MAXLIST];
char *preplist[6];
char *listlist[5];
char roomdes[80],program[80];     /* filenames */
int noun,verb,prep,adj,prepobj,l,*ll;
int comcase,comlen,comif;
struct Image *object_images[MAXLIST];
struct winprint winp1,winp2;
struct Window *win1,*win2;
struct Screen *screen;
struct graphic graph1;
struct Image image,*tpoint;

