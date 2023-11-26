#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "/include/psound.h"
#include "stdio.h"

#define GERMAN 0

extern struct studioBase *studioBase;

/*******************/
/** askuser.c   ****/
/*******************/

void askuser(line1,line2)
char *line1,*line2;
{
telluser2(line1,line2,TRUE);
}

/*******************/
/** telluser.c  ****/
/*******************/

void telluser(line1,line2)
char *line1,*line2;
{
telluser2(line1,line2,FALSE);
}

/********************/
/** telluser2.c  ****/
/********************/

telluser2(line1,line2,showcan)
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

struct Screen *this_screen;
int i;
int len;
extern struct IntuitionBase *IntuitionBase;

/* printf("telluser:\n%s\n%s\n",line1,line2); */

this_screen = studioBase->screen;
if (this_screen==0)
   this_screen = IntuitionBase->ActiveScreen;

len=max(strlen(line1),strlen(line2));
len= (len+8)<<3;
IText1.IText=line1;
IText2.IText=line2;
if (showcan)
   i=AutoRequest(this_screen->FirstWindow,&IText1,&pos,&neg,0L,0L,len,70L);
else
   i=AutoRequest(this_screen->FirstWindow,&IText1,0L,&pos,0L,0L,len,70L);
return(i);
}

