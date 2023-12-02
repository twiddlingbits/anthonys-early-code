/** WARNING: you must compile this module with -ad and no -b option **/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "scan.h"

/*****************************/
/*** change pointer **********/
/*****************************/

newpointer(screen,mode)

SHORT mode;
struct Screen *screen;

{
struct Window *w;

static unsigned short busy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x380, 0x380,
   0x1FE0, 0x1FE0,
   0x3FFC, 0x3FFC,
   0x3FFE, 0x31FE,
   0x3FFE, 0x3DFE,
   0x7FFE, 0x7B1E,
   0x7FFE, 0x77DE,
   0x7FFC, 0x71BC,
   0x7FFC, 0x7F7C,
   0x3FFC, 0x3F1C,
   0x7FF8, 0x7FF8,
   0x7F00, 0x7F00,
   0x3BC0, 0x3BC0,
   0x1C0, 0x1C0,
   0x80, 0x80,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

static unsigned short zero[] = {
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
   };

switch (mode) {

   case NORMAL:
      for (w=screen->FirstWindow;w!=0;w=w->NextWindow) {
         ClearPointer(w);
         }
      return(0);

   case BUSY:

      setall(screen,busy, 16, 16, -2, -1);
      break;

   case ZERO:

      setall(screen,zero, 16, 16, -2, -1);
      break;

   }  /** end of switch **/
}

/************************************/
/** change pointers for all windows */
/************************************/

setall(s,data,a,b,c,d)

struct Screen *s;
char *data;
int a,b,c,d;
{
struct Window *w;

for (w=s->FirstWindow; w!=0; w=w->NextWindow) {
   SetPointer(w,data,a,b,c,d-1);
   }
}



