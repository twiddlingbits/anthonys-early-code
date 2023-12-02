#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "graphics/view.h"
#include "libraries/dos.h"
#include "stdio.h"
#include "fcntl.h"
#include "ilbm.h"
#include "readpict.h"
#include "scan.h"

extern struct Window *is_window;
extern struct Screen *is_screen;
extern struct config config;

char fname[40]={0};
char drawer[40]={0};

save_iff()

{
int cancel,i,f;
char path[100];
BYTE *buf;
APTR cm;
struct RastPort *the_rp;

/* printf("modes=%x\n",window->WScreen->ViewPort.Modes); */

the_rp = &is_screen->RastPort;

cm = is_screen->ViewPort.ColorMap->ColorTable;

cancel=get_fname(is_window,is_window->WScreen,
       "Save IFF...",fname,drawer);

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

newpointer(is_screen, BUSY);
PutPict(f, the_rp->BitMap,the_rp->BitMap->BytesPerRow*8,
        the_rp->BitMap->Rows, cm, buf, 8192L);

FreeMem(buf,8192L);
Close(f);
newpointer(is_screen, NORMAL);
i=IffErr();
if (i==0) {
   return(TRUE);
   }
else {
   show_error(DISKERROR);
   return(FALSE);
   }
}

void save_raw()

{
int f;

if ((f=open_file("Save RAW",MODE_NEWFILE))==0) {
   return;
   }

newpointer(is_screen, BUSY);

if (Write(f,config.rawdata,config.raw_data_size)!=config.raw_data_size)
   show_error(DISKERROR);
Close(f);
newpointer(is_screen, NORMAL);
}

void load_raw()
{
char *buf;
int f;
int len;
int i;

if ((f=open_file("Load RAW",MODE_OLDFILE))==0) {
   return;
   }

newpointer(is_screen, BUSY);
len=file_len(f);

if (len!=256000 && len!=128000) {
   Close(f);
   show_error(WRONGFILETYPE);
   newpointer(is_screen, NORMAL);
   return;
   }

if (len < config.raw_data_size) {      /** len 128, raw 256 **/
   buf = config.rawdata;
   for (i=0; i < 200; i++) {
      if (Read(f, buf, 640)!=640) {
         Close(f);
         show_error(DISKERROR);
         newpointer(is_screen, NORMAL);
         return;
         }
      movmem(buf, buf+640, 640);
      buf = buf + 1280;
      }
   }

else if (len > config.raw_data_size) {      /** len 256, raw 128 **/
   buf = config.rawdata;
   for (i=0; i < 200; i++) {
      Read(f, buf, 640);         /** skip one line **/
      if (Read(f, buf, 640)!=640) {
         Close(f);
         show_error(DISKERROR);
         newpointer(is_screen, NORMAL);
         return;
         }
      buf = buf + 640;
      }
   }

else {         /*** len = raw **/
   if (Read(f,config.rawdata,config.raw_data_size)!=config.raw_data_size) {
      Close(f);
      show_error(DISKERROR);
      newpointer(is_screen, NORMAL);
      return;
      }
   }

if (config.flags&IMG_NOLACE)
   config.actual_height = 200;
else
   config.actual_height = 400;

refresh_screen();
newpointer(is_screen, NORMAL);
Close(f);
}

load_iff()

{
int cancel;
char path[50];

cancel=get_fname(is_window,is_window->WScreen,
       "Load IFF...",fname,drawer);

if (cancel==NULL)
   return(FALSE);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);
return(load_iff_path(path));
}

load_iff_path(path)

char *path;

{
int i,f,depth,return_code;
struct BitMap bitmap0;
ILBMFrame iFrame;
extern ChipAlloc();

if ((f=Open(path,MODE_OLDFILE))==0) {
   show_error(CANTOPENFILE);
   return(FALSE);
   }

newpointer(is_screen, BUSY);
if (config.rawdata && (config.flags&IMG_NOLACE)) {
   FreeMem(config.rawdata, config.raw_data_size);  /** need mem to load pic*/
   config.rawdata = 0;
   }
i=ReadPicture(f, &bitmap0, &iFrame, ChipAlloc);
Close(f);

if (i!=IFF_DONE) {
   show_error(BADIFF);
   return_code = FALSE;
   }

else {
   i = min(config.sh,iFrame.bmHdr.h);
   depth = is_screen->BitMap.Depth;
   LoadRGB4(&is_screen->ViewPort, iFrame.colorMap, iFrame.nColorRegs);
   BltBitMap(&bitmap0,0,0, &is_screen->BitMap,0,0,config.sw,i, 0xC0,0xFF,0);
   RemFree(bitmap0.Planes[0]);
   return_code = TRUE;
   }

if (config.rawdata==0)
   config.rawdata = (char *)AllocMem(config.raw_data_size,0);
if (config.rawdata==0) {
   telluser2(is_screen,"Not Enough Memory Available","",FALSE);
   newpointer(is_screen, NORMAL);
   closewindow(is_window);
   CloseScreen(is_screen);
   closelibraries();
   exit(10);
   }

if (!return_code) {
   if (config.flags&IMG_NOLACE) {
      refresh_buffer();   /** copies screen to RAW buffer **/
      telluser2(is_screen,"Warning:","Raw Data altered",FALSE);
      }
   newpointer(is_screen, NORMAL);
   return(return_code);
   }

if (i != iFrame.bmHdr.h || iFrame.bmHdr.w!=config.sw ||
   iFrame.bmHdr.nPlanes!=depth)
   show_error(WRONGFILETYPE);

refresh_buffer();   /** copies screen to RAW buffer **/
newpointer(is_screen, NORMAL);
return(TRUE);
}

ok_to_use(path)

char *path;

{
int i;

if ((i=Open(path,MODE_OLDFILE))!=0) {      /** File exists! **/
   Close(i);
   i=telluser2(is_screen,"File Already Exists.","Write Over it?",TRUE);
   if (!i)
      return(FALSE);
   }
return(TRUE);
}

open_file(title,mode)

int mode;
char *title;

{
static char drawer[37] ={0};
static char fname[37] = {0};
char path[80];
long f,cancel;

cancel=get_fname(is_window,is_window->WScreen,
       title,fname,drawer);

if (cancel==NULL)
   return(0);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if ((f=Open(path,mode))==0) {
   show_error(CANTOPENFILE);
   return(0);
   }

return(f);
}

/** Returns the length of an Amiga Dos file ***/

file_len(f)

long f;

{
long len,pos;

Seek (f, 0L, 0L);        /** Seek returns previous position, not current! **/
pos = Seek (f, 0L, 1L);     /** seek to end of file, and get old pos  ***/
len = Seek (f, pos, -1L);   /** Move back to pos, and get length **/
return(len);
}
