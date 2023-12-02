#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "stdio.h"

#define GERMAN 0
#define WHITE 0
#define BLACK 15

/*******************/
/** telluser.c  ****/
/*******************/

void telluser(screen,line1,line2)
struct Screen *screen;
char *line1,*line2;
{
telluser2(screen,line1,line2,TRUE);
}

/********************/
/** telluser2.c  ****/
/********************/

telluser2(screen,line1,line2,showcan)
struct Screen *screen;
char *line1,*line2;
int showcan;

{
static struct IntuiText IText2 = {
   WHITE,BLACK,JAM1,   /* front and back text pens and drawmode */
   18,21,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   NULL,   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   WHITE,BLACK,JAM1,   /* front and back text pens and drawmode */
   18,10,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   NULL,   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

static struct IntuiText pos = {
   WHITE,BLACK,JAM1,   /* front and back text pens and drawmode */
   6,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
#if GERMAN
   "JA",   /* pointer to text */
#else
   "OK",
#endif
   NULL   /* next IntuiText structure */
};


static struct IntuiText neg = {
   WHITE,BLACK,JAM1,   /* front and back text pens and drawmode */
   6,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
#if GERMAN
   " NEIN ",   /* pointer to text */
#else
   "Cancel",
#endif
   NULL   /* next IntuiText structure */
};

struct Screen *oldscreen;
int i;
int len;
extern struct IntuitionBase *IntuitionBase;

oldscreen = screen;
if (screen==0)
   screen = IntuitionBase->ActiveScreen;

len=max(strlen(line1),strlen(line2));
len= (len+8)<<3;
IText1.IText=line1;
IText2.IText=line2;
if (oldscreen)
   SetRGB4(&screen->ViewPort, 1, 13, 13, 13);    /** make gray black temp. **/
if (showcan)
   i=AutoRequest(screen->FirstWindow,&IText1,&pos,&neg,0L,0L,len,70L);
else
   i=AutoRequest(screen->FirstWindow,&IText1,0L,&pos,0L,0L,len,70L);
if (oldscreen)
   SetRGB4(&screen->ViewPort, 1, 1, 1, 1);

return(i);
}

