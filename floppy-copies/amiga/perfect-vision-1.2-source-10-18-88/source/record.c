#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"
#include "vid.h"

extern short lace;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;

extern char fname[35];
extern char drawer[35];
static UBYTE *frame[MAXFRAMES];

record(bmp)
struct BitMap *bmp;
{
static short frame_limit = 20;
short frame_shown = 0;
short frame_kount = 0;
int i,size,menunum;
char line[20];

i = 0;
if (lace)
   size = 64000;
else
   size = 32000;

show_record_menu();

while (TRUE) {
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:     /** Stop **/
         i=0;
         while (i < frame_kount) {
            FreeMem(frame[i++],size);
            }
         return(0);

      case 2:     /** Grab Sequence **/
         newpointer(menu_screen,BUSY);
         i=0;
         while (i < frame_kount) {        /** Free old seq, if any **/
            FreeMem(frame[i++],size);
            }

         i=0;
         while ((frame[i]=SafeAllocMem(size,0))!=0) {
            if (i==0) {          /** First frame is garbage, so skip **/
               pack_grab(frame[i]);
               pack_grab(frame[i]);
               }

            pack_grab(frame[i]);
            if (lace)
               pack_grab(frame[i]+32000);
            else
               pack_grab(frame[i]);
            i++;
            if (i >= MAXFRAMES || i >= frame_limit)
               break;
            }

         newpointer(menu_screen,NORMAL);

         if (i==0) {
            show_error(OUTOFMEM);
            break;
            }

         frame_kount = i;
         frame_shown = 0;
         sprintf(line,"%d",i);
         pop_up(); 
         telluser2(menu_screen,"Frames Captured:",line,FALSE);
         pop_down();
         show(frame[frame_shown],NORMAL,0,FALSE);
         break;

      case 3:     /** let user set max frame to grab **/
         pop_up();
         frame_limit=getint(menu_screen,frame_limit,1,MAXFRAMES,"Max Frames to Grab",170);
         pop_down();
         break;

      case 5:        /*** advance a frame ***/
         if (frame_kount == 0) {
            show_error(NOFRAMES);
            break;
            }

         frame_shown++;
         if (frame_shown == frame_kount) {
            pop_up();
            telluser2(menu_screen,
            "You are viewing the last frame.","",FALSE);
            pop_down();
            frame_shown--;
            break;
            }
         show(frame[frame_shown],NORMAL,0,FALSE);
         break;

      case 4:        /*** backup a frame ***/
         if (frame_kount == 0) {
            show_error(NOFRAMES);
            break;
            }

         frame_shown--;
         if (frame_shown < 0) {
            pop_up();
            telluser2(menu_screen,
            "You are viewing the first frame.","",FALSE);
            pop_down();
            frame_shown=0;
            break;
            }
         show(frame[frame_shown],NORMAL,0,FALSE);
         break;

      case 6:        /*** goto a frame **/
         if (frame_kount == 0) {
            show_error(NOFRAMES);
            break;
            }
         if (frame_kount == 1) {
            show_error(ONLY_ONE_FRAME);
            break;
            }
         pop_up();
         i=getint(menu_screen,frame_shown,0,frame_kount-1,"Show New Frame",170);
         if (i<0 || i>=frame_kount)
            telluser2(menu_screen, "Invalid frame","",FALSE);
         else
            frame_shown = i;
         pop_down();
         show(frame[frame_shown],NORMAL,0,FALSE);
         break;

      case 7:        /** show **/
         newpointer(menu_screen,BUSY);
         i = 0;
         while (i < frame_kount) {
            show(frame[i++],NORMAL,0,FALSE);
            }
         newpointer(menu_screen,NORMAL);
         frame_shown = frame_kount-1;
         break;

      case 8:
         show_recsave_menu();

         do {
            menunum=wait_menu();
            if (menunum==1)
               break;
            else if (menunum==2)
               save_iff(main_screen);
            else if (menunum == 3)
               save_seq(frame_kount);
            } while (menunum == 0);
         show_record_menu();
         break;

      }
   }
}

show_record_menu()

{
char *t[10];

t[0] = "Exit";
t[1] = "Grab";
t[2] = "Setmax";
t[3] = "<<<";
t[4] = ">>>";
t[5] = "Goto";
t[6] = "Show";
t[7] = "Save";
t[8] = 0;

create_menu(t);
}

show_recsave_menu()

{
char *t[5];

t[0] = "Exit";
t[1] = "Screen";
t[2] = "Sequence";
t[3] = 0;

create_menu(t);
}

save_seq (frame_kount)

{
int cancel,i,f;
char path[50],newpath[55];
BYTE *buf;
SHORT cm[16]; /* = {0xFFFF,0x0000}; */
struct RastPort *the_rp;
extern struct Window *window;

the_rp = rp;

for (i=0; i<16; i++)
   cm[i]=i|(i<<4)|(i<<8);

pop_up();
cancel=get_fname(menu_window,menu_window->WScreen,
       "Save Sequence...",fname,drawer);

pop_down();
if (cancel==NULL)
   return(FALSE);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

buf=(UBYTE *)AllocMem(8192L,0);
if (buf==0) {
   return(FALSE);
   }

newpointer(menu_screen, BUSY);

i = 0;
while (i < frame_kount) {
   sprintf(newpath,"%s%d",path,i);
   if (check_menu()==1) {
      FreeMem(buf,8192L);
      newpointer(menu_screen, NORMAL);
      return(FALSE);
      }

   if ((f=Open(newpath,MODE_NEWFILE))==0) {
      FreeMem(buf,8192L);
      show_error(CANTOPENFILE);
      newpointer(menu_screen, NORMAL);
      return(FALSE);
      }

   show(frame[i],NORMAL,0,FALSE);
   PutPict(f, the_rp->BitMap,the_rp->BitMap->BytesPerRow*8,
           the_rp->BitMap->Rows, cm, buf, 8192L);

   Close(f);
   cancel=IffErr();
   if (cancel!=0) {
      show_error(DISKERROR);
      FreeMem(buf,8192L);
      newpointer(menu_screen, NORMAL);
      return(FALSE);
      }
   i++;
   }

newpointer(menu_screen, NORMAL);
return (TRUE);
}

SafeAllocMem(size,type)

long size;
int type;

{
char *mem;

mem = AllocMem(size,type);

if (mem) {
   if (AvailMem(0) < 25000) {
      FreeMem(mem,size);
      mem = 0;
      }
   }
return(mem);
}

