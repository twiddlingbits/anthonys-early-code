#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "stdio.h"

/********************/
/** telluser2.c  ****/
/********************/

overwrite(window)
struct Window *window;

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
   "Append",
   NULL   /* next IntuiText structure */
};


static struct IntuiText neg = {
   WHITE,BLACK,JAM1,   /* front and back text pens and drawmode */
   6,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Overwrite",
   NULL   /* next IntuiText structure */
};

int i;
int len;
char *line1,*line2;

neg.FrontPen=BLACK;
pos.FrontPen=BLACK;
IText1.FrontPen=BLACK;
IText2.FrontPen=BLACK;

line1 = "File Already Exits";
line2 = "";

len=max(strlen(line1),strlen(line2));
len= (len+8)<<3;
IText1.IText=line1;
IText2.IText=line2;
i=AutoRequest(window,&IText1,&pos,&neg,0L,0L,len,70L);
return(i);
}

