
/**** USE -b0 WHEN COMPILING THIS MODULE *****/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"
#include "graphics/view.h"
#include "vid.h"

extern short lace;
extern struct Screen *screen;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;
extern struct Screen *screen;

static UBYTE *red_buf,*grn_buf,*blu_buf;
static long bufsize;
static short bri_adj=0; /** color control paramaters **/
static short con_adj=0;
static short sat_adj=0;
static short radj=0;   /** These values are added to data when **/
static short badj=0;    /** createing a color picture **/
static short gadj=0;
static short lowmem;
static short num_lock = 0;
static short num_colors = 32;
static USHORT locked_palette[32];

ham_menu()
{
short menunum;

if (AvailMem(MEMF_FAST)==0)
   lowmem = TRUE;
else
   lowmem = FALSE;

if (lowmem && lace) {
   show_error(OUTOFMEM);
   return(0);
   }

if (lace)
   bufsize = 64000;
else
   bufsize = 32000;

if ((grn_buf = AllocMem(bufsize,MEMF_CLEAR))==0) {
   show_error(OUTOFMEM);
   return(0);
   }


if ((red_buf = AllocMem(bufsize,MEMF_CLEAR))==0) {
   FreeMem(grn_buf, bufsize);
   show_error(OUTOFMEM);
   return(0);
   }

if (lowmem) {
   blu_buf = grab_buffer+32000;
   }
else {
   if ((blu_buf = AllocMem(bufsize,MEMF_CLEAR))==0) {
      FreeMem(grn_buf, bufsize);
      FreeMem(red_buf, bufsize);
      show_error(OUTOFMEM);
      return(0);
      }
   }

while(TRUE) {
   show_color_menu0();
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:  /** Exit **/
         FreeMem(grn_buf, bufsize);
         FreeMem(red_buf, bufsize);
         if (!lowmem)
            FreeMem(blu_buf, bufsize);
         return(0);

      case 2:     /** Build buffers **/
         do_buffers();
         break;

      case 3:     /** HAM **/
         do_ham();
         break;

      case 4:     /*** 32 color **/
         do_32();
         break;
      }
   }
}


/**********************************************************************/
/** do_buffer():  This subroutine lets the user fill the red, green  **/
/** and blue buffers used to generate the color images. The user can **/
/** either digitizer, load, or save the buffers                      **/
/**********************************************************************/

do_buffers()

{
static short pause = FALSE;
static short cs_mode = RED;
short menunum;

set_cs_bits(cs_mode);
show_color_menu1(pause, cs_mode);

while(TRUE) {

   if (lowmem) {
      pack_grab(grab_buffer);
      pack_grab(grab_buffer);
      }
   else {
      pack_grab(grab_buffer);
      pack_grab(grab_buffer+32000);
      }

   if (lace) {
      ql_show(grab_buffer,
           rp->BitMap->Planes[0],
           rp->BitMap->Planes[1],
           rp->BitMap->Planes[2],
           rp->BitMap->Planes[3]);
      }
   else {
      quarter_show(grab_buffer,
           rp->BitMap->Planes[0],
           rp->BitMap->Planes[1],
           rp->BitMap->Planes[2],
           rp->BitMap->Planes[3]);
      }

   menunum=check_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:     /** Exit **/
         return(0);

      case 2:
         bb_file();
         show_color_menu1(pause, cs_mode);
         break;

      case 3:
         newpointer(menu_screen, BUSY);
         auto_capture();
         set_cs_bits(cs_mode);
         newpointer(menu_screen, NORMAL);
         break;

      case 4:
         if (cs_mode==RED) cs_mode=GREEN;
         else if (cs_mode==GREEN) cs_mode=BLUE;
         else cs_mode=RED;
         show_color_menu1(pause, cs_mode);
         set_cs_bits(cs_mode);

      case 5:     /** red **/
         newpointer(menu_screen, BUSY);
         if (pause) {
            Delay(250);
            grab3();
            beep(500,300);
            }
         if (lace)
            lacemov(grab_buffer, red_buf);
         else
            movmem(grab_buffer, red_buf, bufsize);

         newpointer(menu_screen, NORMAL);
         break;

      case 6:     /** green **/
         newpointer(menu_screen, BUSY);
         if (pause) {
            Delay(250);
            grab3();
            beep(500,300);
            }
         if (lace)
            lacemov(grab_buffer, grn_buf);
         else
            movmem(grab_buffer, grn_buf, bufsize);

         newpointer(menu_screen, NORMAL);
         break;

      case 7:     /** blue **/
         newpointer(menu_screen, BUSY);
         if (pause) {
            Delay(250);
            grab3();
            beep(500,300);
            }

         if (lace)
            lacemov(grab_buffer, blu_buf);
         else
            movmem(grab_buffer, blu_buf, bufsize);

         newpointer(menu_screen, NORMAL);
         break;

      case 8:           /*** Pause to give user time to run to camera? ***/
         pause = !pause;
         show_color_menu1(pause, cs_mode);
         break;
      }
   }
}


/**************************************************************/
/** Capture a color picture automaticly using color splitter **/
/**************************************************************/

auto_capture()

{
set_cs_bits(RED);

pack_grab(grab_buffer);    /** clear digitizer **/
pack_grab(grab_buffer);

auto_capture_part(red_buf, GREEN);
auto_capture_part(grn_buf, BLUE);
auto_capture_part(blu_buf, GREEN);
}

auto_capture_part(buf, next_color)

UBYTE *buf;
int next_color;

{
if (lowmem) {
   pack_grab(grab_buffer);
   pack_grab(grab_buffer);
   }
else {
   pack_grab(grab_buffer);
   pack_grab(grab_buffer+32000);
   }

set_cs_bits(next_color);

if (lace)
   lacemov(grab_buffer, buf);
else
   movmem(grab_buffer, buf, bufsize);
}


/***********************************/
/** File menu under build buffers **/
/***********************************/

bb_file()

{
long f,len;
short menunum;

show_color_menu5();

while(TRUE) {

   menunum=check_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:     /** Exit **/
         return(0);

      case 2:     /** save **/
         if ((f=open_file("Load RGB",MODE_OLDFILE))==0)
            break;
         len = file_len(f)/3;
         if (len!=bufsize) {
            pop_up();
            if (len!=(bufsize*2)) {
               telluser2(menu_screen,"Error: file not in Perfect",
                        "Vision RAW format",FALSE);
               pop_down();
               Close(f);
               break;
               }
            else {
               telluser2(menu_screen,"Warning: RGB file does not match",
                        "current screen resolution.",FALSE);
               }
            pop_down();
            }
         newpointer(menu_screen, BUSY);

         if (Read (f,grab_buffer,len)==len) {
            move_loaded_data(len,bufsize,red_buf);
            if (Read (f,grab_buffer,len)==len) {
               move_loaded_data(len,bufsize,grn_buf);
               if (Read (f,grab_buffer,len)==len) {
                  move_loaded_data(len,bufsize,blu_buf);
                  newpointer(menu_screen, NORMAL);
                  Close(f);
                  break;
                  }
               }
            }

         Close(f);
         show_error(DISKERROR);
         newpointer(menu_screen, NORMAL);
         break;

      case 3:
         if ((f=open_file("Save RGB",MODE_NEWFILE))==0)
            break;
         newpointer(menu_screen, BUSY);

         if (Write(f,red_buf,bufsize)==bufsize) {
            if (Write(f,grn_buf,bufsize)==bufsize) {
               if (Write(f,blu_buf,bufsize)==bufsize) {
                  newpointer(menu_screen, NORMAL);
                  Close(f);
                  break;
                  }
               }
            }

         Close(f);
         show_error(DISKERROR);
         newpointer(menu_screen, NORMAL);
         break;
      }
   }
}

/*************************************************************************/
/** do_ham(): This lets the user generate ham pictures from the buffers **/
/** built with do_buffers().                                            **/
/*************************************************************************/

do_ham()

{
long i;
short menunum;
static short fast = FALSE;

if (open_ham_screen(screen_height())==0)
   return(0);

show_color_menu2(fast);
ScreenToFront(menu_screen);

while (TRUE) {
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:
         close_ham_screen();
         return(0);

      case 2:        /*** Save ham picture ***/
         do_hamfile();
         show_color_menu2(fast);
         break;

      case 3:     /** update ham picture **/
         newpointer(menu_screen, BUSY);
         setlut(radj,gadj,badj);
         i = cc_to_fixed(sat_adj);
         disp_saturate(i);
         i = cc_to_fixed(con_adj);
         disp_bright_cont(bri_adj, i);

         if (lace) {
            i=colordisp(400, red_buf, grn_buf, blu_buf, fast, 0,-1);
            }
         else {
            i=colordisp(200, red_buf, grn_buf, blu_buf, fast, 0,0);
            }
         newpointer(menu_screen, NORMAL);
         break;

      case 4:
         color_control(&bri_adj,&con_adj,&sat_adj,&radj,&gadj,&badj);
          break;

      case 5:           /*** Fast or Slow ham mode? ***/
         fast = !fast;
         show_color_menu2(fast);
         break;
      }
   }
}

/*************************************************************************/
/** do_32():  This lets the user generate 32 color pictures from the    **/
/** buffers built with do_buffers().                                            **/
/*************************************************************************/

do_32()

{
long i;
short menunum;
short *cmap;

if (open_32_screen(screen_height())==0)
   return(0);

show_color_menu3();
ScreenToFront(menu_screen);

while (TRUE) {
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:
         close_32_screen();
         return(0);

      case 2:        /*** Save/Load ham picture ***/
         do_32file();
         show_color_menu3();
         break;

      case 3:     /** update ham picture **/
         newpointer(menu_screen, BUSY);
         setlut(radj,gadj,badj);
         i = cc_to_fixed(sat_adj);
         disp_saturate(i);
         i = cc_to_fixed(con_adj);
         disp_bright_cont(bri_adj, i);

/*         if (locked)
            show_32(bufsize, red_buf, grn_buf, blu_buf);
         else
            color32(bufsize, red_buf, grn_buf, blu_buf); */

         color32(bufsize, red_buf, grn_buf, blu_buf, locked_palette, num_lock, num_colors);

         newpointer(menu_screen, NORMAL);
         break;

      case 4:
         color_control(&bri_adj,&con_adj,&sat_adj,&radj,&gadj,&badj);
         break;

      case 5:           /** edit palette **/
         do_palette();
         show_color_menu3();
         break;

      }
   }
}

do_palette()

{
short menunum,i;

show_color_menu4();

while (TRUE) {
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:
         return(0);

      case 2:           /** edit palette **/
         if (lace)
            i = palette(screen,15,30,160,200);
         else
            i = palette(screen,15,15,160,100);

         if (i==0) {
            newpointer(menu_screen, BUSY);
            get_new_colors();
            show_32(bufsize, red_buf, grn_buf, blu_buf);
            newpointer(menu_screen, NORMAL);
            }
         break;

      case 3:           /** Get Number of color registers to use **/
         pop_up();
         num_colors=getint(menu_screen,num_colors,1,32,"Maximum Number of Colors to Use",320);
         pop_down();
         break;

      case 4:     /** Lock color registers **/
         pop_up();
         num_lock=getint(menu_screen,num_lock,0,32,"Number of Colors to Lock",250);
         pop_down();
         movmem(screen->ViewPort.ColorMap->ColorTable, locked_palette, 64);
         break;
      }
   }
}


/*******************************************************/
/** do_32file(): Save or Load a 32 color IFF picture  **/
/*******************************************************/

do_32file()
{


show_32file_menu();

while(TRUE) {
   switch(wait_menu()) {
      case 0:
         break;

      case 1:           /** Exit **/
         return(0);

      case 2:
         if (load_iff(screen)) {
            newpointer(menu_screen, BUSY);
            get_new_colors();
            newpointer(menu_screen, NORMAL);
            }
         break;

      case 3:
         save_iff(screen);   /** Save IFF HAM mode **/
         break;

      case 4:     /** save overscan picture **/
         do_save_overscan(screen);
         show_32file_menu();
         break;
      }
   }
}

/*************************************************/
/** do_hamfile(): Save a ham color IFF picture  **/
/*************************************************/

do_hamfile()
{

show_hamfile_menu();

while(TRUE) {
   switch(wait_menu()) {
      case 0:
         break;

      case 1:              /*** Exit ***/
         return(0);

      case 2:
         save_iff(screen);   /** Save IFF HAM mode **/
         break;

      case 3:     /** save overscan picture **/
         do_save_overscan(screen);
         show_hamfile_menu();
         break;
      }
   }
}

open_file(title,mode)

int mode;
char *title;

{
static char drawer[37] ={0};
static char fname[37] = {0};
char path[80];
long f,cancel;

pop_up();
cancel=get_fname(menu_window,menu_window->WScreen,
       title,fname,drawer);

pop_down();
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

show_color_menu0()

{
char *t[5];

t[0] = "Exit";
t[1] = "Build Buffers";
t[2] = "HAM";
t[3] = "32 Color";
t[4] = 0;

create_menu(t);
}

show_color_menu1(p, sel)       /** Build buffers submenu **/

short p;

{
char *t[10];

t[0] = "Exit";
t[1] = "File";
t[2] = "Auto";

if (sel==RED)
   t[3] = ">RED";
else if (sel==GREEN)
   t[3] = ">GRN";
else
   t[3] = ">BLU";

t[4] = "Red";
t[5] = "Grn";
t[6] = "Blu";

if (p)
   t[7] = "Pause";
else
   t[7] = "Now";

t[8] = 0;

create_menu(t);
}


show_color_menu2(f)       /*** HAM sub menu ***/

{
char *t[6];

t[0] = "Exit";
t[1] = "File";
t[2] = "Draw";
t[3] = "Control";

if (f)
   t[4] = "Fast Draw";
else
   t[4] = "Slow Draw";

t[5] = 0;

create_menu(t);
}

show_color_menu3()       /*** 32 color sub menu ***/

{
char *t[7];

t[0] = "Exit";
t[1] = "File";
t[2] = "Draw";
t[3] = "Control";
t[4] = "Palette";
t[5] = 0;

create_menu(t);
}

show_color_menu4()       /*** 32 color palate options ***/

{
char *t[7];

t[0] = "Exit";
t[1] = "Edit Palette";
t[2] = "Num Colors";
t[3] = "Num Lock";
t[4] = 0;

create_menu(t);
}

show_color_menu5()       /*** File menu under build buffers ***/

{
char *t[5];

t[0] = "Exit";
t[1] = "Load";
t[2] = "Save";
t[3] = 0;

create_menu(t);
}

show_32file_menu()
{
char *t[5];

t[0] = "Exit";
t[1] = "Load IFF";
t[2] = "Save IFF";
t[3] = "Overscan";
t[4] = 0;

create_menu(t);
}

show_hamfile_menu()
{
char *t[4];

t[0] = "Exit";
t[1] = "Save IFF";
t[2] = "Overscan";
t[3] = 0;

create_menu(t);
}

lacemov(s, d)
UBYTE *s, *d;
{
unsigned long i;

for (i=0; i<32000; i=i+160) {
/*   printf("s %d d %d\n", i, i+i); */
   movmem(s+i, d+i+i, 160);
   }

for (i=0; i<32000; i=i+160) {
   movmem(s+32000+i, d+i+i+160, 160);
/*   printf("s %d d %d\n", i+32000, i+i+160);  */
   }
}

grab3()
{
pack_grab(grab_buffer);
pack_grab(grab_buffer+32000);

pack_grab(grab_buffer);
pack_grab(grab_buffer+32000);

pack_grab(grab_buffer);
pack_grab(grab_buffer+32000);
}

/******************************************************************/
/** transfer the screen color palette to the one used by bobby's **/
/** color algorithems for computing 32 color pictures            **/
/******************************************************************/

get_new_colors()
{
extern UBYTE colormapr[32], colormapg[32], colormapb[32];
short *cmap;
short i,num_colors;

cmap       = screen->ViewPort.ColorMap->ColorTable;
num_colors = screen->ViewPort.ColorMap->Count;

for (i=0; i < num_colors; i++) {
   colormapr[i] = 15&(cmap[i]>>8);
   colormapg[i] = 15&(cmap[i]>>4);
   colormapb[i] = 15&cmap[i];
   }
disp_precalcreg(num_colors);
}

/*********************************************************************/
/*** get_cmap(): Given a pointer to an array of USHORTs, get_cmap() **/
/*** fills it with the current 32 color color map in format for use **/
/*** with LoadRGB4()                                               ***/
/*********************************************************************/

get_cmap(cmap,len)

int len;
USHORT *cmap;

{
extern UBYTE colormapr[32], colormapg[32], colormapb[32];
short i;

for (i=0; i < len; i++) {
   cmap[i] = (colormapr[i]<<8) | (colormapg[i]<<4) | (colormapb[i]);
   }
}


move_loaded_data(len,bufsize,buf)

long len,bufsize;
char *buf;

{
long i;

if (len == bufsize)
   movmem(grab_buffer, buf, len);

else if (bufsize < len) {
   for (i=0; i<64000; i=i+320)
   movmem(grab_buffer+i,buf+(i/2), 160);
   }

else {
   movmem(grab_buffer,grab_buffer+32000,32000);
   lacemov(grab_buffer, buf);
   }
}

/******************************************************************/
/*** Convert the color control values to 16bit fixed for bobby's **/
/*** routines.                                                   **/
/******************************************************************/

cc_to_fixed(val)

short val;

{
static short ctbl[32] = {
   0x0000,0x0011,0x0022,0x0033,0x0044,0x0055,0x0066,0x0077,
   0x0088,0x0099,0x00aa,0x00bb,0x00cc,0x00dd,0x00ee,0x0100,
   0x0140,0x0180,0x01c0,0x0200,0x0240,0x0280,0x02c0,0x0300,
   0x0380,0x0400,0x0500,0x0600,0x0800,0x1000,0x3000
   };

return(ctbl[val+15]);
}

screen_height()
{
if (lace)
   return(400);
return(200);
}

