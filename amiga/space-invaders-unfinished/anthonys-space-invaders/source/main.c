#include "exec/types.h"
#include "intuition/intuition.h"
#include "si.h"


void main()
{
struct Window *w,*setup();
struct IntuiMessage *message,*GetMsg();
ULONG class;
USHORT code;
short x,y;
short i,k;
short base_motion=STILL;
short si_x = X_START;
short si_y = 15;
short si_w = NUMCOL*BOXWIDTH;
short si_h = NUMROW*BOXHEIGHT;
short alive[NUMCOL][NUMROW];   /** lists all vaders still around **/
short speed = 3;               /** speed vaders move at **/
short gun_x = 30;
USHORT move_aliens=1;



for (i=0; i < NUMCOL; i++)     /** initlize all vaders to alive **/
   for (k=0; k < NUMROW; k++)
      alive[i][k]=TRUE;

if ((w=setup())==0)
   exit(0);

showvaders(w,si_x,si_y);        /** Display a screen of space invaders **/
drawgun(w, gun_x, 185);

while (TRUE) {
   while((message=GetMsg(w->UserPort))==0) {
      Wait(1<<w->UserPort->mp_SigBit);
      }

   class=message->Class;
   code=message->Code;
   ReplyMsg(message);

   if (class == RAWKEY) {
      if (code == ESC)
         break;
      else if (code == GO_RIGHT)
         base_motion = MOVING_RIGHT;
      else if (code == GO_LEFT)
         base_motion = MOVING_LEFT;
      else
         base_motion = STILL;
      }

   else {
      if (base_motion != STILL) {
         cleargun(w, gun_x, 185);
         if (base_motion == MOVING_RIGHT)
            gun_x += 10;
         if (base_motion == MOVING_LEFT)
            gun_x -= 10;
         if (gun_x < 20)
            gun_x = 20;
         if (gun_x+43 > USE_WIDTH)
            gun_x = USE_WIDTH - 43;
         drawgun(w, gun_x, 185);
         }

      if (move_vaders(w, &si_x, &si_y, si_w, si_h, speed)) {
         printf("Game over!!\n\n");
         break;
         }
      }
   }
closedown(w);
}

#if 0
x=createmsl(w);
FreeSprite(x);
#endif

move_vaders(w,si_x, si_y, si_w, si_h, speed)

struct Window *w;
short *si_x, *si_y, si_w, si_h, speed;

{
static short dir_r = TRUE;

if (dir_r) {
   if (*si_x + si_w + speed > USE_WIDTH) {
      dir_r = FALSE;
      WaitTOF();
      moveall(w,*si_x,*si_y,si_h,si_w,0,BOXHEIGHT);
      *si_y += BOXHEIGHT;
      }
   else {
      moveall(w,*si_x,*si_y,si_h,si_w,speed,0);
      *si_x += speed;
      }
   }

else {
   if (*si_x - speed <= X_START) {
      dir_r = TRUE;
      WaitTOF();
      moveall(w,*si_x,*si_y,si_h,si_w,0,BOXHEIGHT);
      *si_y += BOXHEIGHT;
      }
   else {
      moveall(w,*si_x,*si_y,si_h,si_w,-speed,0);
      *si_x -= speed;
      }
   }

if (*si_y + si_h + BOXHEIGHT >= 199)
   return(TRUE);

return(FALSE);
}


/******************************************************************/
/*** Takes a word and returns TRUE if time for event to happen,
     or FALSE if not time yet. LSBin word is no. of ticks to
     count, and MSB is current count **/
/******************************************************************/

timefor(x)

USHORT *x;
{
UBYTE msb,lsb;

msb = (*x)>>8;
lsb = (*x)&0xFF;
msb++;
if (msb == lsb) {
   *x = lsb;
   return(TRUE);
   }
*x = (msb<<8) | lsb;
return(FALSE);
}

