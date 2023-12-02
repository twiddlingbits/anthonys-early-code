#include "exec/types.h"
#include "exec/exec.h"
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
short speed = 2;               /** speed vaders move at **/
short gun_x = 30;
USHORT move_aliens=8;
short base_fired = FALSE;



for (i=0; i < NUMCOL; i++)     /** initlize all vaders to alive **/
   for (k=0; k < NUMROW; k++)
      alive[i][k]=TRUE;

if ((w=setup())==0)
   exit(0);

showvaders(w,si_x,si_y);        /** Display a screen of space invaders **/
drawgun(w, gun_x, 185);

while (TRUE) {
WaitTOF();

   if ((message=GetMsg(w->UserPort))!=0) {
      class=message->Class;
      code=message->Code;
      ReplyMsg(message);
      }

   else {
      class=0;
      code=0;
   }

   if (class == RAWKEY) {
      if (code == ESC)
         break;
      else if (code == GO_RIGHT)
         base_motion = MOVING_RIGHT;
      else if (code == GO_LEFT)
         base_motion = MOVING_LEFT;
      else if (!base_fired && code == FIRE) {
         if (start_msl(w,gun_x)) { /** error! **/
            printf("can't make missle!\n");
            break;
            }
         hit();   /** Flush any previous colisions **/
         base_fired = TRUE;
         }
      else if (code!=FIRE_UP && code!=FIRE)
         base_motion = STILL;
      }

   else {
      if (base_fired) {
         if (bump_msl(w)) {
            end_msl(w);
            base_fired = FALSE;
            erase_vader(w,si_x,si_y);
            }
         }
      if (base_motion != STILL) {
         cleargun(w, gun_x, 185);
         if (base_motion == MOVING_RIGHT)
            gun_x += 4;
         if (base_motion == MOVING_LEFT)
            gun_x -= 4;
         if (gun_x < 20)
            gun_x = 20;
         if (gun_x+40 > USE_WIDTH)
            gun_x = USE_WIDTH - 40;
         drawgun(w, gun_x, 185);
         }

      if (timefor(&move_aliens)) {
         if (move_vaders(w, &si_x, &si_y, si_w, si_h, speed)) {
            printf("Game over!!\n\n");
            break;
            }
         }
      }
   }

if (base_fired)
   end_msl(w);
closedown(w);
}

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
