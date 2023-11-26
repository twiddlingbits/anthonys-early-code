#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "graphics/view.h"
#include "libraries/dos.h"
#include "stdio.h"
#include "fcntl.h"
#include "ilbm.h"
#include "readpict.h"
#include "vid.h"

extern short lace;
extern struct Window *main_window;
extern struct Screen *main_screen;
extern struct Window *menu_window;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;

char fname[35]={0};
char drawer[35]={0};

save_menu()
{
int menunum;

show_save_menu();

while(TRUE) {
   menunum=wait_menu();   /** Change to wait!! **/
   switch (menunum) {
      case 0:
            break;

      case 1:
         return;

      case 2:
         save_iff(main_screen);
         break;

      case 3:
         save_raw();
         break;

      case 4:
         load_iff(main_screen);
         break;

      case 5:
         load_raw();
         break;

      case 6:
         do_save_overscan(main_screen);
         show_save_menu();
         break;
      }
   }
}


show_save_menu()

{
char *t[10];

t[0] = "Exit";
t[1] = "SaveIFF";
t[2] = "SaveRAW";
t[3] = "LoadIFF";
t[4] = "LoadRAW";
t[5] = "OS";
t[6] = 0;

create_menu(t);
}

save_iff(screen)

struct Screen *screen;

{
int cancel,i,f;
char path[50];
BYTE *buf;
APTR cm;
struct RastPort *the_rp;

/* printf("modes=%x\n",window->WScreen->ViewPort.Modes); */

the_rp = &screen->RastPort;

cm = screen->ViewPort.ColorMap->ColorTable;

pop_up();
cancel=get_fname(menu_window,menu_window->WScreen,
       "Save IFF...",fname,drawer);

pop_down();
if (cancel==NULL)
   return(FALSE);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if (!ok_to_use(path))
   return(FALSE);

if ((f=Open(path,MODE_NEWFILE))==0) {
   show_error(CANTOPENFILE);
   return(FALSE);
   }

buf=(UBYTE *)AllocMem(8192L,0);
if (buf==0) {
   Close(f);
   return(FALSE);
   }

newpointer(menu_screen, BUSY);
PutPict(f, the_rp->BitMap,the_rp->BitMap->BytesPerRow*8,
        the_rp->BitMap->Rows, cm, buf, 8192L);

FreeMem(buf,8192L);
Close(f);
newpointer(menu_screen, NORMAL);
i=IffErr();
if (i==0) {
   return(TRUE);
   }
else {
   show_error(DISKERROR);
   return(FALSE);
   }
}

save_raw()

{
extern short fourbits;
int f;

if ((f=open_file("Save RAW",MODE_NEWFILE))==0) {
   return;
   }

newpointer(menu_screen, BUSY);

if (fourbits) {
   if (lace) {
      if (Write(f,grab_buffer,64000)!=64000)
         show_error(DISKERROR);
      }
   else {
      if (Write(f,grab_buffer,32000)!=32000)
         show_error(DISKERROR);
      }
   }

else {
   if (lace) {
      if (Write(f,grab_buffer,128000)!=128000)
         show_error(DISKERROR);
      }
   else {
      if (Write(f,grab_buffer,64000)!=64000)
          show_error(DISKERROR);
      }
   }

Close(f);
newpointer(menu_screen, NORMAL);
}

load_raw()
{
int f,bufsize;

if ((f=open_file("Load RAW",MODE_OLDFILE))==0) {
   return;
   }

if (lace)
   bufsize = 64000;
else
   bufsize = 32000;

newpointer(menu_screen, BUSY);
if (Read(f,grab_buffer,bufsize)!=bufsize) {
   Close(f);
   show_error(DISKERROR);
   newpointer(menu_screen, NORMAL);
   show(grab_buffer,NORMAL,0,FALSE);
   return(0);
   }

show(grab_buffer,NORMAL,0,FALSE);
newpointer(menu_screen, NORMAL);
Close(f);
}


load_iff(screen)

struct Screen *screen;

{
int i,f,cancel,depth;
char path[50];
struct BitMap bitmap0;
ILBMFrame iFrame;
extern ChipAlloc();

pop_up();
cancel=get_fname(menu_window,menu_window->WScreen,
       "Load IFF...",fname,drawer);

pop_down();
if (cancel==NULL)
   return(FALSE);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if ((f=Open(path,MODE_OLDFILE))==0) {
   show_error(CANTOPENFILE);
   return(FALSE);
   }

newpointer(menu_screen, BUSY);
i=ReadPicture(f, &bitmap0, &iFrame, ChipAlloc);
Close(f);

if (i!=IFF_DONE) {
   show_error(BADIFF);
   newpointer(menu_screen, NORMAL);
   return(FALSE);
   }

if (lace)
   i = min(400,iFrame.bmHdr.h);
else
   i = min(200,iFrame.bmHdr.h);

depth = screen->BitMap.Depth;

LoadRGB4(&screen->ViewPort, iFrame.colorMap, iFrame.nColorRegs);

BltBitMap(&bitmap0,0,0, &screen->BitMap,0,0,320,i, 0xC0,0xFF,0);
RemFree(bitmap0.Planes[0]);

if (i != iFrame.bmHdr.h || iFrame.bmHdr.w!=320 ||
   iFrame.bmHdr.nPlanes!=depth)
   show_error(WRONGFILETYPE);

rp_to_buf();   /** copies screen to RAW buffer **/
newpointer(menu_screen, NORMAL);
return(TRUE);
}

pop_up()
{
MoveScreen(menu_screen, 0, -150);
}

pop_down()
{
MoveScreen(menu_screen, 0, 150);
}

ok_to_use(path)

char *path;

{
int i;

if ((i=Open(path,MODE_OLDFILE))!=0) {      /** File exists! **/
   Close(i);
   pop_up();
   i=telluser2(menu_screen,"File Already Exists.","Write Over it?",TRUE);
   pop_down();
   if (!i)
      return(FALSE);
   }
return(TRUE);
}

