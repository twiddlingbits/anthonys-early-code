#include <exec/types.h>
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "psound.h"

struct Samp samp[MAX_SAMPLES];
struct IOAudio IOB;
struct IOAudio IOB2;
char *bufL[2],*bufR[2];
struct Screen *screen;           /** psound custom screen **/
int mark_drawn;                  /** TRUE if part of graph outlined **/
int old_x1, old_x2;
int start_mark;
int end_mark;                    /** define points within a sample marked */
int insert_mark;                 /** point within sample to insert at ***/
int marked_samp;               /** sample that start, end mark apply to */
int insert_samp;               /** sample where insertion is to happen **/
int cur;                       /** currently selected sample **/
int inserting;
short looping;                /** loop button was pressed **/

/** some wierd things that I shall try and fix in the future:

   1. lmem always pointes to the data for mono sounds. rmem points
      to the right channel data if it is a stereo sound.

   Anthony Wood 2-22-87 **/

/***
   1. when looping, start_mark is always on even and end_mark is always on
      an odd byte
   2. All samples are always even, and only an even number of bytes can
      be edited. Cut, deleted, etc.
   3. markers are inclusive ie. sm=4 and em=5 marks two bytes
**/


void main()
{
void inc_gain(),dec_gain();
void scale();
void ramp_up();
void ramp_dn();
void draw_marked();
void update_se_mark();
void delete_sample();
int start_dragging();
int set_new_em();
int in_graph();
extern struct Menu Titles[];    /* menus done with MenuEd, Shareware */
UBYTE cmap[16*3];
int memory,samplen;
int record_speed;               /* record speed in samples/second */
short loop_mode;
int i,ss,sl,temp;
int len,k,j;
short dragging;
short do_on;
short graph_mode;
int click;
char *AllocMem(),*p1,*p2,*x;
static char line[80],buf[80];
static char path[80]="";
UBYTE *pa=(UBYTE *)0xBFE001;   /* left mouse button is at this address */
SHORT mousex,mousey;
ULONG class;
USHORT code,menunum,itemnum,subnum;
SHORT oct[MAX_SAMPLES];
struct Gadget *gad;
struct Window *Window;
struct IntuiMessage *message;
struct Image image;
struct Screen *openscr();
static struct NewWindow NewWindow = {
   0,
   0,
   640,
   200,
   0,
   1,
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|GADGETUP|RAWKEY,
   REPORTMOUSE|ACTIVATE|NOCAREREFRESH|BORDERLESS|BACKDROP,
   0,0,
   0, /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

for(i=0;i<MAX_SAMPLES;i++) {
   samp[i].lmem=0;
   samp[i].rmem=0;
   samp[i].length=0;
   samp[i].rate=0;
   samp[i].name=0;
   samp[i].ctoctave=0;
   samp[i].oneshot=0;
   samp[i].repeat=0;
   samp[i].cycles=0;
   }

memory=frags();        /* find largest amount of free memeory */
record_speed=20;
dragging   = FALSE;
inserting  = FALSE;        /** true if finding the insert point **/
mark_drawn = FALSE;
do_on = FALSE;
marked_samp = -1;
insert_samp = -1;
cur = 0;
loop_mode = INC_SM;
graph_mode = GRAPH_LEFT;

openlibraries();

screen=openscr(640,3);     /** open a 640 X 200 8 color screen **/
if (screen==0) {
   telluser2("Can't open screen","",FALSE);
   closelibraries();
   exit(10);
   }

NewWindow.Screen=screen;

if ((Window=(struct Window *)OpenWindow(&NewWindow))==NULL) {
   telluser2("Can't open window","",FALSE);
   CloseScreen(screen);
   closelibraries();
   exit(10);
   }

ShowTitle(screen, FALSE);

/* allocate audio channels 1 and 2 */

if (allocate_channel (Window,&IOB)==FALSE) {
   CloseWindow(Window);
   CloseScreen(screen);
   closelibraries();
   exit(10);
   }

IOB2 = IOB;

/**** allocate "double buffers" for audio device ****/

for (i=0;i<2;i++) {
   if ((bufL[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      deallocate_channel (&IOB);
      CloseWindow(Window);
      CloseScreen(screen);
      closelibraries();
      exit(10);
      }

   if ((bufR[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      FreeMem(bufL[0],BUFSIZE);  /* Bad job of clean up here !!!!! */
      deallocate_channel (&IOB);
      CloseWindow(Window);
      CloseScreen(screen);
      closelibraries();
      exit(10);
      }
   }

/************************************/
/** Now do DEMOS and TITLE screen ***/
/************************************/

if (load_ilbm("ps.title",&image,cmap)==-1)
   goto nodemos;

/** set color registers **/
for (i=0;i<16*3;i=i+3)
   SetRGB4(&screen->ViewPort,i/3,cmap[i]>>4,cmap[i+1]>>4,cmap[i+2]>>4);

/** draw title screen **/
DrawImage(Window->RPort,&image,0,0);
FreeMem(image.ImageData,image.Width/8*image.Height*image.Depth);
ScreenToFront(screen);

/** load some sample sounds if they are on the disk **/

msg_off();      /** causes any calls to putxy to be ignored **/
load_it(Window,"sounds/demo1","bang");
load_it(Window,"sounds/demo2","Welcome");
load_it(Window,"sounds/demo3","Button");

/**** play the loaded sounds ******/

if (cur==2) {        /** make sure three sounds were loaded **/
   play(Window,&IOB,0,samp[0].length,&samp[0]);
   for (k=27000;k>=15000;k=k-4000) {
      samp[1].rate=k;
      play(Window,&IOB,1,samp[1].length,&samp[1]);
      }
   samp[1].rate=9859;
   play(Window,&IOB,1,samp[1].length,&samp[1]);
   if (load_ilbm("ps.main",&image,cmap)==-1) {
      telluser2("Can't open ps.main","",FALSE);
      CloseScreen(screen);
      closelibraries();
      exit(10);
      }
   play(Window,&IOB,2,samp[2].length,&samp[2]);
   while ((*pa&64)==64) ;   /** wait for left mouse button press **/
   msg_on();      /** allow messages to appear **/
   goto skip_load;
   }

/** Now load program screen **/

nodemos:

if (load_ilbm("ps.main",&image,cmap)==-1) {
   telluser2("Can't open ps.main","",FALSE);
   goto close_down;
   }

skip_load:
ShowTitle(screen, TRUE);

/** set color registers **/
for (i=0;i<16*3;i=i+3)
   SetRGB4(&screen->ViewPort,i/3,cmap[i]>>4,cmap[i+1]>>4,cmap[i+2]>>4);

/** draw title screen **/
DrawImage(Window->RPort,&image,0,0);
FreeMem(image.ImageData,image.Width/8*image.Height*image.Depth);
ScreenToFront(screen);

msg(Window,"Welcome to Perfect Sound 3.0");
SetMenuStrip(Window,Titles);   /* turn on menus */

/** show the names of any demos sounds that were loaded **/

if (samp[0].lmem)
   PutName(Window,samp[0].name,0,REG_VID);
if (samp[1].lmem)
   PutName(Window,samp[1].name,1,REG_VID);
if (samp[2].lmem)
   PutName(Window,samp[2].name,2,REV_VID);

graph_update:

graph(graph_mode);
lgraph();
rgraph();
mark_drawn = FALSE;

if (cur==marked_samp)
   draw_marked();

update:

memory=frags();

sprintf(line,"Length:      %d  samples    ", samp[cur].length);
putxy(Window,STAT_X,STAT_Y,line,WHITE,BLACK);

sprintf(line,"Play speed:  %d  samp/sec   ", samp[cur].rate);
putxy(Window,STAT_X,STAT_Y+9,line,WHITE,BLACK);


sprintf(line,"Memory:      %d K     ",memory/1024);
putxy(Window,STAT_X,STAT_Y+45,line,WHITE,BLACK);

if (samp[cur].type==RIGHT) {
   putxy(Window,STAT_X+176,STAT_Y+45,"(RIGHT) ",WHITE,BLACK);
   }
else if (samp[cur].type==LEFT) {
   putxy(Window,STAT_X+176,STAT_Y+45,"(LEFT)  ",WHITE,BLACK);
   }
else if (samp[cur].lmem!=0) {
   putxy(Window,STAT_X+176,STAT_Y+45,"(STEREO)",WHITE,BLACK);
   }
else {
   putxy(Window,STAT_X+176,STAT_Y+45,"        ",WHITE,BLACK);
   }

update_se_mark(Window, line);

ignore:       /** here when ready for new input event **/

while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
   Wait(1<<Window->UserPort->mp_SigBit);

class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
gad=(struct Gadget *)message->IAddress;
ReplyMsg(message);

if (class==MOUSEMOVE) {       /* dragging **/
   if (dragging) {
      if (inserting) {
         insert_mark = set_new_em(mousex);
         insert_samp = cur;
         }
      else {
         end_mark = set_new_em(mousex);
         }
      draw_marked();
      }
   do {
      message=(struct IntuiMessage *)GetMsg(Window->UserPort);
      if (message==0)  {
         if (dragging) update_se_mark(Window, line);
         goto ignore;
         }
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      gad=(struct Gadget *)message->IAddress;
      ReplyMsg(message);
      } while (class==MOUSEMOVE);
   }

click = FALSE;
if (class==MOUSEBUTTONS) {        /* selecting a new cur */
   if (code!=SELECTDOWN) {
      if (dragging) {
         dragging=FALSE;
         if (start_mark > end_mark) {     /** swap **/
            temp = start_mark;
            start_mark = end_mark-1;   /** -1 to make even **/
            end_mark = temp-1;         /** -1 to make odd **/
            }
         lgraph();
         rgraph();
         update_se_mark(Window, line);
         }
      goto ignore;
      }

   click = TRUE;

   if (in_graph(mousex, mousey) && samp[cur].lmem) {
      dragging = TRUE;
      if (inserting) {
         insert_mark = start_dragging(mousex);
         insert_samp = cur;
         }
      else {
         if (near (mousex, end_mark)) {
            end_mark = set_new_em(mousex);
            draw_marked();
            rgraph();
            }
         else if (near (mousex, start_mark)) {
            start_mark = end_mark-1;
            end_mark = set_new_em(mousex);
            draw_marked();
            rgraph();
            }
         else {
            start_mark = start_dragging(mousex);
            end_mark = start_mark+1;
            marked_samp = cur;
            }
         }
      update_se_mark(Window, line);
      lgraph();
      goto ignore;
      }

   if (end_up(mousex, mousey) && cur==marked_samp) {
      if (end_mark==samp[cur].length-1)
         goto ignore;
      end_mark +=2;
      rgraph();
      draw_marked();
      update_se_mark(Window, line);
      loop_mode = INC_EM;
      goto ignore;
      }

   if (end_dn(mousex, mousey) && cur==marked_samp) {
      if (end_mark == start_mark+1)
         goto ignore;
      end_mark -=2;
      rgraph();
      draw_marked();
      update_se_mark(Window, line);
      loop_mode = DEC_EM;
      goto ignore;
      }

   if (start_up(mousex, mousey) && cur==marked_samp) {
      if (start_mark+1==end_mark)
         goto ignore;
      start_mark +=2;
      lgraph();
      draw_marked();
      update_se_mark(Window, line);
      loop_mode = INC_SM;
      goto ignore;
      }

   if (start_dn(mousex, mousey) && cur==marked_samp) {
      if (start_mark==0)
         goto ignore;
      start_mark -=2;
      lgraph();
      draw_marked();
      update_se_mark(Window, line);
      loop_mode = DEC_SM;
      goto ignore;
      }

   i=(mousey-STAT_Y)/8;
   if (i >=0 && i < MAX_SAMPLES && samp[i].lmem!=0) {
      if (cur==i) {
         play(Window,&IOB,0,samp[cur].length,&samp[cur]);
         goto ignore;
         }
      PutName(Window,samp[cur].name,cur,REG_VID);
      cur=i;
      PutName(Window,samp[cur].name,cur,REV_VID);

      if (samp[cur].ctoctave>1 || samp[cur].repeat!=0) {
         sprintf(line,"OneShotHiSamples %d  RepeatHiSamples %d No. Octaves %d",
            samp[cur].oneshot,samp[cur].repeat,samp[cur].ctoctave);
         msg(Window,line);
         }
      goto graph_update;
      }
   }

if (class==RAWKEY) {    /** look for function key press. Plays octaves **/
printf("code %d\n",code);
   if (code==25) {
      play(Window,&IOB,start_mark,end_mark-start_mark+1,&samp[cur]);
      goto ignore;
      }

   if (code==76) {
      inc_gain();
      goto ignore;
      }

   if (code==77) {
      dec_gain();
      goto ignore;
      }

   if (code==40) {
      graph_mode=GRAPH_LEFT;
      if (samp[cur].type==STEREO)
         msg(Window,"main graph now shows LEFT channel");
      goto graph_update;
      }

   if (code==19) {
      graph_mode=GRAPH_RIGHT;
      if (samp[cur].type==STEREO)
         msg(Window,"main graph now shows RIGHT channel");
      goto graph_update;
      }

   if (code>89 || code<80)  goto ignore;

   if (samp[cur].ctoctave==0) {
      msg(Window,"This sample is not an instrument.");
      goto ignore;
      }
   if ((i=code-80)>=samp[cur].ctoctave) {
      msg(Window,"Octave not defined.");
      goto ignore;
      }
   k=(powr(2,i)-1)*(samp[cur].oneshot+samp[cur].repeat);
   len=powr(2,i)*samp[cur].oneshot;
   if (even(k)!=k) {
      msg(Window,"Warning: Octave address not on a word boundry!");
      k=even(k);
      }
   if (even(len)!=len) {
      msg(Window,"Warning: oneshot length not even!");
      len=even(len);
      }
   IOB.ioa_Request.io_Unit=(struct Unit *)1;
   if (len!=0) {
      tochan (&IOB,samp[cur].lmem+k,len,toperiod(samp[cur].rate),1);
      WaitIO(&IOB);
      }
   k=k+len;
   j=samp[cur].repeat;
   if (even(j)!=j) {
      msg(Window,"Warning: repeat length not even!");
      j=even(j);
      }
   if (j!=0)
      tochan (&IOB, samp[cur].lmem+k,powr(2,i)*j,toperiod(samp[cur].rate),0);
   while (class!=RAWKEY || code<127) {
      while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
         Wait(1<<Window->UserPort->mp_SigBit);
      class=message->Class;
      code=message->Code;
      ReplyMsg(message);
      }

   AbortIO(&IOB);
/*   IOB.ioa_Request.io_Unit=(struct Unit *)3;
   IOB.ioa_Request.io_Command=CMD_RESET;
   DoIO (&IOB); */
   goto ignore;
   }

if (class==MENUPICK && code==MENUNULL) {
   if (marked_samp==cur)
      play(Window,&IOB,0,start_mark,&samp[cur]);
   goto ignore;
   }

if (click && gad_sel(PLAY_RNG_X,PLAY_RNG_Y,mousex,mousey)) {
   if (marked_samp==cur)
      play(Window,&IOB,start_mark,end_mark-start_mark+1,&samp[cur]);
   goto ignore;
   }

if (click && gad_sel(PLAY_LP_X,PLAY_LP_Y,mousex,mousey)) {
   if (marked_samp!=cur)
      goto ignore;
   if (samp[cur].type==STEREO) {
      msg(Window,"Only mono samples can be looped");
      goto ignore;
      }
   if (!(samp[cur].flags&CHIP)) {
      if (telluser("Current sample is not in chip ram.","Should I convert it?")==FALSE)
         goto ignore;
      if ((p1=AllocMem(samp[cur].length, MEMF_CHIP))==0) {
         msg(Window, "Not enough free chip ram available");
         goto ignore;
         }
      movmem(samp[cur].lmem,p1,samp[cur].length);
      FreeMem(samp[cur].lmem, samp[cur].length);
      samp[cur].lmem = p1;
      samp[cur].rmem = p1;
      samp[cur].flags |= CHIP;
      }
   looping = TRUE;
   IOB.ioa_Request.io_Unit=(struct Unit *)1;
   tochan (&IOB, samp[cur].lmem+start_mark,end_mark-start_mark+1,toperiod(samp[cur].rate),0);
   goto ignore;
   }

if (click && gad_sel(STOP_X,STOP_Y,mousex,mousey)) {
   IOB2.ioa_Request.io_Unit=(struct Unit *)1;
   IOB2.ioa_Request.io_Command=CMD_RESET;
   DoIO (&IOB2);
/*   AbortIO(&IOB);*/
   looping = FALSE;
   goto ignore;
   }

if (click && gad_sel(DEL_RNG_X,DEL_RNG_Y,mousex,mousey)) {     /* delete a range */
   if (marked_samp==-1) {
      msg(Window,"Range no marked.");
      goto ignore;
      }
   if (end_mark==samp[cur].length-1 && start_mark==0) goto whole;
   msg(Window,"Working...");

   i=start_mark;  /* start deleting including this byte */
   j=end_mark;    /* include this byte.  Both markers are inclusive */
   j++;

   k=RUP(samp[cur].length-(j-i));

   if (j<samp[cur].length)
      movmem(samp[cur].lmem+j,samp[cur].lmem+i,samp[cur].length-j);

    if (k<RUP(samp[cur].length))
       FreeMem(samp[cur].lmem+k,RUP(samp[cur].length)-k);

   if (samp[cur].type==STEREO) {
      if (j<samp[cur].length)
         movmem(samp[cur].rmem+j,samp[cur].rmem+i,samp[cur].length-j);
      if (k < RUP(samp[cur].length))
         FreeMem(samp[cur].rmem+k,RUP(samp[cur].length)-k);
      }

   samp[cur].length=samp[cur].length-(j-i);
   msg(Window,"");
   marked_samp = -1;
   insert_samp = -1;
   goto graph_update;
  }

if (click && gad_sel(INSERT_X,INSERT_Y,mousex,mousey)) {      /* insert here */
   if (marked_samp==-1) {
      msg(Window, "You need to mark a range before inserting");
      goto ignore;
      }

   if (!inserting) {
      msg(Window, "Select the point to insert at.");
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==-1) {
      msg(Window, "Select insert point the click insert button again.");
      goto ignore;
      }

   inserting = FALSE;
   msg(Window, "Working...");
   move(Window,start_mark,end_mark,insert_mark,marked_samp,insert_samp);
   msg(Window,"Range inserted.");
   marked_samp = -1;
   insert_samp = -1;
   goto graph_update;
   }

if (click && gad_sel(SET_PLAYBACK_X,SET_PLAYBACK_Y,mousex,mousey)) {      /* enter new playback rate */
   if (samp[cur].lmem==0) goto ignore;
   samp[cur].rate=getint(Window,samp[cur].rate,1000,27000,
                  "Enter a new playback speed (samples/sec):",450);
   }

if (click && gad_sel(PASTE_X,PASTE_Y,mousex,mousey)) {      /* paste */
   if (marked_samp==-1) {
      msg(Window, "No are marked.");
      goto ignore;
      }
   if (!inserting) {
      msg(Window, "Select point to paste at then click paste button again.");
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==-1) {
      msg(Window, "You need to specify where to paste.");
      goto ignore;
      }

   inserting=FALSE;
   msg(Window, "Working...");

   i = insert_mark + end_mark - start_mark + 1;
   if (i >= samp[insert_samp].length)
      i = samp[insert_samp].length;

   for (k = insert_mark; k < i; k++) {
      j = *(samp[insert_samp].lmem+k);
      j = j + *(samp[marked_samp].lmem+k-insert_mark+start_mark);
      j = j/2;
      *(samp[insert_samp].lmem+k) = j;
      }

   if (samp[insert_samp].type==samp[marked_samp].type==STEREO)
      for (k = insert_mark; k < i; k++) {
         j = *(samp[insert_samp].rmem+k);
         j = j + *(samp[marked_samp].rmem+k-insert_mark+start_mark);
         j = j/2;
         *(samp[insert_samp].rmem+k) = j;
         }
   msg(Window, "Samples mixed.");
   goto graph_update;
   }

/* copy range to a new slot */

if (click && gad_sel(NEW_SLOT_X, NEW_SLOT_Y, mousex, mousey)) {
   if (samp[cur].lmem==0) goto ignore;
   len=end_mark-start_mark+1;
   if (marked_samp==-1) {
      msg(Window,"Markers Not Set");
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a label for the new sample",400);
   if (line[0]==0) strcpy(line,"A Nameless Sound");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   strcpy(samp[i].name,line);
   if ((samp[i].lmem=AllocMem(len,0))==0) {
      nomem(Window);
      FreeMem(samp[i].name,strlen(samp[i].name)+1);
      goto ignore;
      }

   samp[i].rmem=samp[i].lmem;

   if (samp[cur].type==STEREO)
      if ((samp[i].rmem=AllocMem(len,0))==0) {
         FreeMem(samp[i].lmem,len);
         FreeMem(samp[i].name,strlen(samp[i].name)+1);
         nomem(Window);
         samp[i].lmem=0;
         samp[i].name=0;
         goto ignore;
         }

   msg(Window,"Working...");

   movmem(samp[cur].lmem+start_mark,samp[i].lmem,len);
   if (samp[cur].type==STEREO)
      movmem(samp[cur].rmem+start_mark,samp[i].rmem,len);
   msg(Window,"");
   samp[i].length=len;
   samp[i].rate=samp[cur].rate;
   samp[i].type=samp[cur].type;
   PutName(Window,samp[cur].name,cur,REG_VID);
   cur=i;
   PutName(Window,samp[cur].name,cur,REV_VID);
   goto graph_update;
   }

/* append a slot to another slot */

if (click && gad_sel(APPEND_X, APPEND_Y, mousex, mousey)) {
   if (samp[cur].lmem==0) goto ignore;
   msg(Window,"Click slot to append");
   i=pickslot(Window);
   msg(Window,"Click slot to append this to...");
   k=pickslot(Window);
   len=samp[i].length+samp[k].length;
   move(Window,0,samp[i].length,samp[k].length,i,k);
   }

/** search for zero crossings **/

if (click && gad_sel(FIND_LOOP_X,FIND_LOOP_Y,mousex,mousey)) {
   if (marked_samp==-1 || marked_samp!=cur) {
      msg(Window,"Set mark range in current sample first.");
      goto ignore;
      }

   while (TRUE) {
      if (loop_mode==INC_SM) {
         if (start_mark+2 >= end_mark) break;
         start_mark +=2;
         temp = start_mark;
         }
      else if (loop_mode==DEC_SM) {
         if (start_mark-2 <= 0) break;
         start_mark -=2;
         temp = start_mark;
         }
      else if (loop_mode==INC_EM) {
         if (end_mark+4 >= samp[cur].length) break;
         end_mark +=2;
         temp = end_mark;
         }
      else if (loop_mode==DEC_EM) {
         if (end_mark-4 <= start_mark) break;
         end_mark -=2;
         temp = end_mark;
         }
      i = *(samp[cur].lmem+temp);
      j = *(samp[cur].lmem+temp-1);
      k = *(samp[cur].lmem+temp+1);
      if (i > -2 && i < 2)
         if ( (j>0 && k<0) || (j<0 && k>0)) break;
      }
   draw_marked();
   if (loop_mode==INC_SM || loop_mode==DEC_SM)
      lgraph();
   else
      rgraph();
   update_se_mark(Window, line);
   goto ignore;
   }

if (class!=MENUPICK)
   goto ignore;

/** ALL GADGET commands must be above here **/
/** Only menu options follow **/

menunum=MENUNUM(code);
itemnum=ITEMNUM(code);
subnum=SUBNUM(code);

if (menunum==0 && itemnum==0) {     /** New **/
   k=FALSE;
   for(i=0;i<MAX_SAMPLES;i++)
      if (samp[i].lmem!=0) {
         if (k==FALSE) {
            if (telluser("WARNING: This will erase all samples.",
                 "         DO IT?")==FALSE)   goto ignore;
            k=TRUE;
            }
         delete_sample(i);
         PutName(Window,"",i,REG_VID);
         }
      marked_samp = -1;
      insert_samp = -1;
      goto graph_update;
   }

if (menunum==0 && itemnum==1) {      /** Open **/
   if (load_samp(Window,path)==-1)
      goto ignore;
   goto graph_update;
   }

if (menunum==0 && itemnum==2) {      /* save ... */
   strcpy(buf,path);
   add(buf,samp[cur].name);
   switch (subnum) {
   case 0: save_IFF(Window,&samp[cur],buf,FALSE);
           break;
   case 1: save_samp(Window,&samp[cur],buf);
           break;
   case 2: save_IFF(Window,&samp[cur],buf,TRUE);
   }
   goto ignore;
   }

if (menunum==0 && itemnum==3) {      /* save AS ... */
   strcpy(line,samp[cur].name);
   i=get_fname(Window,screen,"Save a Sample",line,path);
   if (i==NULL) goto ignore;
   strcpy(buf,path);
   add(buf,line);   /* appends a file or directory to a path */

   switch (subnum) {
      case 0: save_IFF(Window,&samp[cur],buf,FALSE);
              break;
      case 1: save_samp(Window,&samp[cur],buf);
              break;
      case 2: save_IFF(Window,&samp[cur],buf,TRUE);
      }
   goto ignore;
   }

/** 4, delete sample **/
if (menunum==0 && itemnum==4) {      /* delete */
   strcpy(line,"");
   i=get_fname(Window,screen,"Pick File to Delete",line,path);
   if (i==NULL) goto ignore;
   strcpy(buf,path);
   add(buf,line);   /* appends a file or directory to a path */
   i=remove(buf);
   if (i)
      msg(Window,"DOS error while trying to delete file.");
   else
      msg(Window,"File deleted.");
   goto ignore;
   }

if (menunum==0 && itemnum==6) {    /* about */
   aboutps(Window);
   goto ignore;
   }

if (menunum==0 && itemnum==7) {     /* quit */
   if (!telluser("Do you really want to quit?",""))
      goto ignore;
close_down:
   filter_on();      /** just in case user has filter turned off **/
   deallocate_channel (&IOB);
   CloseWindow(Window);
   CloseScreen(screen);
   for(i=0;i<MAX_SAMPLES;i++)
      if (samp[i].lmem!=0) {
         FreeMem(samp[i].lmem,samp[i].length);
         FreeMem(samp[i].name,strlen(samp[i].name)+1);
         if (samp[i].type==STEREO)
            FreeMem(samp[i].rmem,samp[i].length);
         }

   for (i=0;i<2;i++) {
      FreeMem(bufL[i],BUFSIZE);
      FreeMem(bufR[i],BUFSIZE);
      }
   closelibraries();
   exit(0);
   }

if (menunum==1 && itemnum==0) {        /* flip marked range */
   if (samp[cur].lmem==0) goto ignore;
   msg(Window,"Flipping...");

   flip(samp[cur].lmem,samp[cur].length);
   if (samp[cur].type==STEREO) {
      flip(samp[cur].rmem,samp[cur].length);
   }
   msg(Window,"");
   goto graph_update;
   }

if (menunum==1 && itemnum==1) {   /* Zero Range */
   if (samp[cur].lmem==0) goto ignore;
   if (marked_samp==-1) {
      msg(Window,"Markers not set.");
      goto ignore;
      }
   msg(Window,"Working...");
   for (i=start_mark; i<=end_mark; i++) {
      if (samp[marked_samp].type==STEREO || samp[marked_samp].type==LEFT)
         *(samp[marked_samp].lmem+i)=0;
      if (samp[marked_samp].type==STEREO || samp[marked_samp].type==RIGHT)
         *(samp[marked_samp].rmem+i)=0;
      }
      msg(Window,"");
      goto graph_update;
   }

/** Range Up (2) **/

if (menunum==1 && itemnum==2) {  /** scale range from zero to full scale **/
   if (marked_samp==-1) {
      msg(Window, "Range not selected.");
      goto ignore;
      }
   ramp_up(samp[marked_samp].lmem, start_mark, end_mark);
   if (samp[marked_samp].type==STEREO)
      ramp_up(samp[marked_samp].rmem, start_mark, end_mark);
   goto graph_update;
   }

if (menunum==1 && itemnum==3) {  /** ramp down **/
   if (marked_samp==-1) {
      msg(Window, "Range not selected.");
      goto ignore;
      }
   ramp_dn(samp[marked_samp].lmem, start_mark, end_mark);
   if (samp[marked_samp].type==STEREO)
      ramp_dn(samp[marked_samp].rmem, start_mark, end_mark);
   goto graph_update;
   }


if (menunum==1 && itemnum==4) {  /** scale **/
   if (marked_samp==-1) {
      msg(Window, "Range not selected.");
      goto ignore;
      }
   i=getint(Window,100,1,300, "Enter percentage to scale by:",350);
   scale(samp[marked_samp].lmem, start_mark, end_mark, i);
   if (samp[marked_samp].type==STEREO)
      scale(samp[marked_samp].rmem, start_mark, end_mark, i);
   goto graph_update;
   }

/** Mix (5) **/
if (menunum==1 && itemnum==5) {
   gen_sin(&samp[cur], 4000);
   goto graph_update;
   }

/** Zoom Range (6) **/
if (menunum==1 && itemnum==6) {
   graph(graph_mode|GRAPH_ZOOM);
   msg(Window,"Zoom View... Press left mouse button to continue");
   while (class!=MOUSEBUTTONS) {
      while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
         Wait(1<<Window->UserPort->mp_SigBit);
      class=message->Class;
      ReplyMsg(message);
      }
   msg(Window,"");
   goto graph_update;
   }

if (menunum==1 && itemnum==7) {  /** Mark entire sample **/
   if (samp[cur].lmem!=0) {
      start_mark = 0;
      end_mark = samp[cur].length-1;
      marked_samp = cur;
      draw_marked();
      goto update;
      }
   }

/** Low pass filter (7) **/

if (menunum==1 && itemnum==8) {
   i=getint(Window,4000,1000,10000, "Enter cutoff frequency:",350);
   lp_filter(&samp[cur], i);
   goto graph_update;
   }

if (menunum==1 && itemnum==9) {
whole:         /* jump here from delete range if entire sample */
   delete_sample(cur);
   if (cur==marked_samp)
      marked_samp = -1;
   insert_samp = -1;
   i=0;
   while(samp[i].lmem==0 && i<MAX_SAMPLES) i++;
   PutName(Window,"",cur,REG_VID);
   if (i!=MAX_SAMPLES) {
      cur=i;
      PutName(Window,samp[cur].name,cur,REV_VID);
      }
   else
      cur=0;
   goto graph_update;
   }

/* create stereo from two mono samples */

if (menunum==1 && itemnum==10) {
   if (samp[cur].lmem==0) goto ignore;
   msg(Window,"Click slot for left channel.");
   i=pickslot(Window);
   if (samp[i].type==STEREO) {
      msg(Window,"Sample must be mono.");
      goto ignore;
      }
   msg(Window,"Click slot for right channel.");
   k=pickslot(Window);
   if (samp[k].type==STEREO) {
      msg(Window,"Sample must be mono.");
      goto ignore;
      }
   if (k==i) {
      msg(Window,"Samples must be in different slots.");
      goto ignore;
      }
   if (samp[i].length<samp[k].length) {ss=i;sl=k;}
   else {ss=k;sl=i;}
   if (samp[i].length!=samp[k].length)
      FreeMem(samp[sl].lmem+RUP(samp[ss].length),
         RUP(samp[sl].length) - RUP(samp[ss].length));
   FreeMem(samp[sl].name,strlen(samp[sl].name)+1);
   samp[ss].lmem=samp[i].lmem;
   samp[ss].rmem=samp[k].rmem;
   samp[ss].type=STEREO;
   PutName(Window,"",sl,REG_VID);
   cur=ss;
   samp[sl].lmem=0;
   samp[sl].rmem=0;
   samp[sl].length=0;
   samp[sl].name=0;
   PutName(Window,samp[cur].name,cur,REV_VID);
   msg(Window,"Current sample is now stereo.");
   goto update;
   }

/* Break up a stereo into two new slots */

if (menunum==1 && itemnum==11) {
   if (samp[cur].lmem==0) goto ignore;
   if (samp[cur].type!=STEREO) {
      msg(Window,"Sample must be stereo.");
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   samp[i].name=AllocMem(strlen(samp[cur].name)+2,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   *samp[i].name='L';
   strcpy(samp[i].name+1,samp[cur].name);
   samp[i].lmem=samp[cur].lmem;
   samp[i].rmem=samp[cur].lmem;
   samp[i].type=LEFT;
   samp[i].length=samp[cur].length;
   samp[i].rate=samp[cur].rate;
   x=AllocMem(strlen(samp[cur].name)+2,0);
   if (x==0) {
      nomem(Window);
      FreeMem(samp[i].name[i],strlen(samp[i].name+1));
      samp[i].lmem=0;
      samp[i].rmem=0;
      samp[i].length=0;
      goto ignore;
      }
   *x='R';
   strcpy(x+1,samp[cur].name);
   FreeMem(samp[cur].name,strlen(samp[cur].name)+1);
   samp[cur].name=x;
   samp[cur].lmem=samp[cur].rmem;
   samp[cur].type=RIGHT;
   PutName(Window,samp[cur].name,cur,REV_VID);
   PutName(Window,samp[i].name,i,REG_VID);
   msg(Window,"Created seperate left and right channels.");
   }

if (menunum==1 && itemnum==12) {       /** Resample to new sampling rate **/
   if (samp[cur].lmem==0) goto ignore;
   if ((i=getslot(Window))==-1) {
      msg(Window,"No free slots");
      goto ignore;
      }
   samp[i].name=AllocMem(strlen(samp[cur].name)+3,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   strcpy(samp[i].name,samp[cur].name);
   strcpy(samp[i].name+strlen(samp[cur].name),"-2");
   samp[i].rate=getint(Window,samp[cur].rate,4000,40000, "Enter new sample rate:",350);
   samp[i].length=(samp[i].rate*samp[cur].length)/samp[cur].rate;
   samp[i].lmem=AllocMem(samp[i].length, 0);
   if (samp[i].lmem==0) {
      FreeMem(samp[i].name[i],strlen(samp[i].name+1));
      samp[i].lmem=0;
      samp[i].rmem=0;
      samp[i].length=0;
      nomem(Window);
      goto ignore;
      }
   msg(Window, "Working...");
   for (j=0; j < samp[i].length; j++) {
      k = samp[cur].length*j;
      k = k/samp[i].length;
      samp[i].lmem[j]=samp[cur].lmem[k];
      }
   if (samp[cur].rmem==samp[cur].lmem) {     /** mono **/
      samp[i].rmem=samp[i].lmem;
      }
   else {
      samp[i].rmem=AllocMem(samp[i].length, 0);
      if (samp[i].rmem==0) {
         FreeMem(samp[i].lmem, samp[i].length);
         FreeMem(samp[i].name[i],strlen(samp[i].name+1));
         samp[i].lmem=0;
         samp[i].rmem=0;
         samp[i].length=0;
         nomem(Window);
         goto ignore;
         }
      for (j=0; j < samp[i].length; j++) {
         k = samp[cur].length*j;
         k = k/samp[i].length;
         samp[i].rmem[j]=samp[cur].rmem[k];
         }
      }
   samp[i].type=samp[cur].type;
   PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[i].name,i,REV_VID);
   cur=i;
   msg(Window,"Resampled sound to new rate.");
   goto graph_update;
   }

if (menunum==2 && itemnum==0) {   /* create a new instrument */
   if (samp[cur].lmem==0) goto ignore;

/* create list of octaves in assending order */
   k=0;
   for (i=0;i<MAX_SAMPLES;i++)
      if (samp[i].lmem!=0)
         oct[k++]=i;

/* sort */

   for (i=0;i<k-1;i++)
      for (j=i+1;j<k;j++)
         if (samp[oct[i]].length>samp[oct[j]].length) {
            temp=oct[i];
            oct[i]=oct[j];
            oct[j]=temp;
            }

   if (oct[0]!=marked_samp) {
      msg(Window,"You must set range of octave with highest frequency.");
      goto ignore;
      }

   if ((i=getslot(Window))==-1) goto ignore;
   len=(powr(2,k)-1)*samp[oct[0]].length;
   p1=AllocMem(len,MEMF_CHIP);
   if (p1==0) {
      msg(Window,"Not Enough Chip Memory.");
      goto ignore;
      }
   samp[i].length=len;
   samp[i].type=RIGHT;
   samp[i].rate=samp[oct[0]].rate;
   samp[i].lmem=p1;
   samp[i].rmem=p1;
   samp[i].ctoctave=k;
   samp[i].oneshot=start_mark;
   samp[i].repeat=samp[oct[0]].length-start_mark;

   /* move all octaves into new instrument */

   len=samp[oct[0]].length;
   temp=0;
   for (j=0;j<k;j++) {
      if (samp[oct[j]].length!=len)
         msg(Window,"Warning, an octave length is not the correct size.");
      movmem(samp[oct[j]].lmem,samp[i].lmem+temp,len);
      temp=temp+len;
      len=len*2;
      }


   /* get a name for this sample */

   line[0]=0;
   getstr(Window,line,"Enter a name for this instrument:",400);
   if (line[0]==0) strcpy(line,"Lazy typer");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      FreeMem(samp[i].lmem,samp[i].length);
      samp[i].lmem=0;
      samp[i].ctoctave=0;
      samp[i].oneshot=0;
      goto ignore;
      }
   strcpy(samp[i].name,line);

  samp[i].cycles=getint(Window,8,0,256,"Enter SamplesPerHiCycle",256);

 /* sampperhicycle should be power of 2 for most programs */

   j=FALSE;
   for (k=0;k<16;k++)
      if (powr(2,k)==samp[i].cycles) j=TRUE;
   if (j==FALSE)
       msg(Window,"Warning: SamplesPerHiCycle may need to be a power of 2.");
   else
      msg(Window,"Instrument created.");
  /* highlight new instrument */

   PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[cur].name,i,REV_VID);
   cur=i;
   }

/* cut period in half by eliminating samples (f=f*2) */

if (menunum==2 && itemnum==1) {
   if (samp[cur].lmem==0) goto ignore;
   if (samp[cur].type==STEREO) {
      msg(Window,"Sorry, this command does not work with stereo.");
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a name for the new sample:",400);
   if (line[0]==0) strcpy(line,"*2");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   msg(Window,"Working...");
   strcpy(samp[i].name,line);    /* move filename to permenant place */
   samp[i].length=samp[cur].length/2;
   samp[i].lmem=AllocMem(samp[i].length,0);
   if (samp[i].lmem==0) {
      nomem(Window);
      samp[i].name=0;
      samp[i].length=0;
      goto ignore;
      }
   samp[i].type=samp[cur].type;
   samp[i].rate=samp[cur].rate;
   samp[i].rmem=samp[i].lmem;
   j=0;
   for (k=0;k<samp[cur].length;k=k+2)
      *(samp[i].lmem+j++)=*(samp[cur].lmem+k);

   PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[i].name,i,REV_VID);
   cur=i;
   msg(Window,"");
   }
   

/* double period by interpolating  f=f/2*/

if (menunum==2 && itemnum==2) {
   if (samp[cur].lmem==0) goto ignore;
   if (samp[cur].type==STEREO) {
      msg(Window,"Sorry, this command does not work with stereo.");
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a name for the new sample:",400);
   if (line[0]==0) strcpy(line,"/2");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   msg(Window,"Working...");
   strcpy(samp[i].name,line);    /* move filename to permenant place */
   samp[i].length=samp[cur].length*2;
   samp[i].lmem=AllocMem(samp[i].length,0);
   if (samp[i].lmem==0) {
      nomem(Window);
      samp[i].name=0;
      samp[i].length=0;
      goto ignore;
      }
   samp[i].type=samp[cur].type;
   samp[i].rate=samp[cur].rate;
   samp[i].rmem=samp[i].lmem;
   j=0;
   for (k=0;k<samp[cur].length;k++) {
      *(samp[i].lmem+j++)=*(samp[cur].lmem+k);
      *(samp[i].lmem+j++)=(*(samp[cur].lmem+k)+*(samp[cur].lmem+k+1))/2;
      }
   *(samp[i].lmem+j-1)=*(samp[cur].lmem+k-1);    /* last interp. is wrong */

   PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[i].name,i,REV_VID);
   cur=i;
   msg(Window,"");
   }

if (menunum==3 && itemnum==0) {    /* change default recording speed */
   i=getint(Window,record_speed,4000,40000,
            "Enter a new recording speed (samples/sec): ",450);
   record_speed=357954/i;
   sprintf(line,"New record speed changed to %d.",357954/record_speed);
   msg(Window, line);
   }

if (menunum==3 && itemnum==1) {  /* View Signal Levels */
   level(Window);
   goto ignore;
   }
   
if (menunum==3 && itemnum==2) {            /* monitor the digitizer */
   msg(Window,"Press the left mouse button to exit.");
   switch (subnum) {

   case 0: listen(STEREO);
           break;
   case 1: listen(RIGHT);
           break;
   case 2: listen(LEFT);
           }
   msg(Window,"");
   goto ignore;
   }

if (menunum==3 && itemnum==3)  {  /* record */
   if (subnum==0 && record_speed > 30000) {
      msg(Window,"Stereo record is limited to 30000 samp/sec");
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   p1=(char *)1;p2=(char *)1;
   Forbid();
   memory=frags();
   if (memory<8000) {
      Permit();
      for (i=0;i<1000;i++);
      nomem(Window);
      goto ignore;
      }
   if (subnum==0) {      /* stereo */
      memory=memory/2;
      p1=AllocMem(memory,0);
      if (p1!=0) p2=AllocMem(memory,0);
      if (p2==0) FreeMem(p1,memory);
      }
   else  
      p1=AllocMem(memory,0);
   Permit();
   if (p1==0 || p2==0) {
      printf("out of memory here %d\n",memory);
      nomem(Window);
      goto ignore;
      }
   switch (subnum) {
      case 0: k=STEREO;
              samp[i].lmem=p1;
              samp[i].rmem=p2;
              break;
      case 1: k=RIGHT;
              samp[i].rmem=p1;
              samp[i].lmem=p1; /* use only to find free sample elsewhere */
              break;
      case 2: k=LEFT;
              samp[i].lmem=p1;
              samp[i].rmem=p1;    /* only for compatibility with prev.*/
              break;
      default: msg(Window,"BUG... code 01");
              goto ignore;
      }
   samp[i].type=k;
   msg(Window,"Monitoring... Press left mouse button to start recording.");
   listen(k);   /* listen before we start recording */

   samplen=digitze (Window,p1,p2,record_speed,memory,k);
   samplen=even(samplen);
   if (samplen==0) {
      msg(Window,"BUG... code 02");
      FreeMem(samp[i].lmem,memory);
      if (k==STEREO) FreeMem(samp[i].rmem,memory);
      samp[i].lmem=0;
      goto ignore;
      }
   if (samplen==memory)
      msg(Window,"Memory filled.");
   else {
      FreeMem(p1+RUP(samplen),RUP(memory) - RUP(samplen));
      sprintf(line,"%d bytes used in this recording.",samplen);
      if (k==STEREO) {
         FreeMem(p2+RUP(samplen),RUP(memory) - RUP(samplen));
         sprintf(line,"%d bytes used in this recording.",samplen*2);
         }
      msg(Window,line);
      }
   samp[i].length=samplen;
   samp[i].rate=357954/record_speed;
   line[0]=0;
   getstr(Window,line,"Enter a label for this sample:",400);
   if (line[0]==0) strcpy(line,"A Nameless Sound");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      FreeMem(samp[i].lmem,samplen);
      samp[i].lmem=0;
      if (k==STEREO) FreeMem(samp[i].rmem,samplen);
      goto ignore;
      }
   strcpy(samp[i].name,line);    /* move filename to premenant place */
   if (samp[cur].lmem!=0)
     PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[i].name,i,REV_VID);
   cur=i;
   msg(Window,"");     /* clear input line */
   goto graph_update;
   }

/** Auto gain set (3, 4) **/

if (menunum==4 && itemnum==0) {   /** filter on or off **/
   if (do_on) {
      filter_on();
      do_on = FALSE;
      msg(Window,"Amiga's low pass output filter turned on.");
      goto ignore;
      }
   else {
      filter_off();
      do_on = TRUE;
      msg(Window,"Amiga's low pass output filter is now bypassed.");
      goto ignore;
      }
   }

if (menunum==4 && itemnum==1) {   /** turn demos off **/
   if (rename("ps.title","no_ps.title")==0)
      msg(Window, "Renamed ps.title to no_ps.title");
   else
      msg(Window, "Error while trying to rename ps.title");
   goto ignore;
   }

if (menunum==4 && itemnum==2) {   /** turn demos on **/
   if (rename("no_ps.title","ps.title")==0)
      msg(Window, "Renamed no_ps.title to ps.title");
   else
      msg(Window, "Error while trying to rename no_ps.title");
   goto ignore;
   }

if (menunum==4 && itemnum==3) {   /** Help **/
   msg(Window,"No help yet!");
   goto ignore;
   }

   goto update;
}   

/** given the pointer position, update the global variable "marker" **/

int start_dragging(mousex)

int mousex;
{
int marker;      /** sample where dragging starts **/

SetDrMd(&screen->RastPort, COMPLEMENT);

if (mark_drawn)
   RectFill(&screen->RastPort, old_x1, GRAPH_Y, old_x2, GRAPH_Y+GRAPH_H-1);

mark_drawn=TRUE;    /** erase old marked range **/
old_x1 = old_x2 = mousex;

marker = ((mousex - GRAPH_X)*samp[cur].length) + (GRAPH_W/2); /* .5 round */
marker = even(marker / GRAPH_W);
Move (&screen->RastPort, mousex, GRAPH_Y);
Draw (&screen->RastPort, mousex, GRAPH_Y+GRAPH_H-1);
return(marker);
}

void draw_marked()

{
int x1,x2;
int temp;
extern int old_x1, old_x2;

SetDrMd(&screen->RastPort, COMPLEMENT);
if (inserting) {
   x1 = GRAPH_X + (insert_mark*GRAPH_W)/samp[cur].length;
   x2 = GRAPH_X + (insert_mark*GRAPH_W)/samp[cur].length;
   }
else {
   x1 = GRAPH_X + (start_mark*GRAPH_W)/samp[cur].length;
   x2 = GRAPH_X + (end_mark*GRAPH_W)/samp[cur].length;
   }

if (x2 < x1) {    /** swap **/
   temp = x1;
   x1 = x2;
   x2 = temp;
   }

if (looping) {
/*   IOB2.ioa_Request.io_Unit=(struct Unit *)1;
   IOB2.ioa_Request.io_Command=CMD_RESET;
   DoIO (&IOB); */
/*   AbortIO(&IOB); */
/*   IOB.ioa_Request.io_Unit=(struct Unit *)1; */
   if (end_mark < start_mark) {
      new_loop_section(0,samp[cur].lmem+end_mark-1,start_mark-end_mark+1);
/*      tochan (&IOB, samp[cur].lmem+end_mark-1,start_mark-end_mark+1,toperiod(samp[cur].rate),0); */
      }
   else {
      new_loop_section(0,samp[cur].lmem+start_mark,end_mark-start_mark+1);
/*      tochan (&IOB, samp[cur].lmem+start_mark,end_mark-start_mark+1,toperiod(samp[cur].rate),0); */
      }
   }

if (old_x1==x1 && old_x2==x2 && mark_drawn)
   return;

if (mark_drawn)
   RectFill(&screen->RastPort, old_x1, GRAPH_Y, old_x2, GRAPH_Y+GRAPH_H-1);
else
   mark_drawn=TRUE;

RectFill(&screen->RastPort, x1, GRAPH_Y, x2, GRAPH_Y+GRAPH_H-1);
old_x1 = x1;
old_x2 = x2;
}

int in_graph(x, y)

int x, y;

{
return (x>GRAPH_X && x<(GRAPH_X+GRAPH_W) && y>GRAPH_Y && y<(GRAPH_Y+GRAPH_H));
}

int end_up(x, y)

int x, y;

{
return (x>ENDUP_X && x<(ENDUP_X+ENDUP_W) && y>ENDUP_Y && y<(ENDUP_Y+ENDUP_H));
}

int end_dn(x, y)

int x, y;

{
return (x>ENDDN_X && x<(ENDDN_X+ENDDN_W) && y>ENDDN_Y && y<(ENDDN_Y+ENDDN_H));
}

int start_up(x, y)

int x, y;

{
return (x>STARTUP_X && x<(STARTUP_X+STARTUP_W) && y>STARTUP_Y && y<(STARTUP_Y+STARTUP_H));
}

int start_dn(x, y)

int x, y;

{
return (x>STARTDN_X && x<(STARTDN_X+STARTDN_W) && y>STARTDN_Y && y<(STARTDN_Y+STARTDN_H));
}

int set_new_em(mousex)
int mousex;
{
int marker;

marker = ((mousex - GRAPH_X)*samp[cur].length)/GRAPH_W;
marker = odd(marker);
if (marker >= samp[cur].length)
   marker = samp[cur].length-1;
if (marker < 0)
   marker = 1;
return (marker);
}

near(mousex, mark)
int mousex, mark;
{
int i;

i = mousex - GRAPH_X - mark*GRAPH_W/samp[cur].length;
if (i < 0)
   i = -i;
if (i < 15)
   return (TRUE);
else
   return(FALSE);
}

gad_sel(targx,targy,mx,my)

int targx, targy;    /** check if mouse is in this area **/
int mx, my;          /** mouse x,y to check **/

{
if ( mx < targx || mx > (targx+BUT_W))
   return(FALSE);

if ( my < targy || my > (targy+BUT_H))
   return(FALSE);
return(TRUE);
}

void delete_sample(val)

int val;

{
if (samp[val].lmem==0)
   return;

FreeMem(samp[val].lmem,samp[val].length);

if (samp[val].type==STEREO)
  FreeMem(samp[val].rmem,samp[val].length);

FreeMem(samp[val].name,strlen(samp[val].name)+1);

samp[val].lmem=0;
samp[val].rmem=0;
samp[val].length=0;
samp[val].repeat=0;
samp[val].oneshot=0;
samp[val].cycles=0;
samp[val].ctoctave=0;
if (marked_samp==val) {
   marked_samp = -1;
   mark_drawn = FALSE;
   }
}

void update_se_mark(Window, line)
struct Window *Window;
char *line;
{

if (marked_samp==-1)
   sprintf(line,"Start mark:  none");
else
   sprintf(line,"Start mark:  %d     ", start_mark);
putxy(Window,STAT_X,STAT_Y+18,line,WHITE,BLACK);

if (marked_samp==-1)
   sprintf(line,"End mark:    none");
else
   sprintf(line,"End mark:    %d     ", end_mark);
putxy(Window,STAT_X,STAT_Y+27,line,WHITE,BLACK);

if (insert_samp==-1)
   sprintf(line,"Insert mark: none");
else
   sprintf(line,"Insert mark: %d     ", insert_mark);
putxy(Window,STAT_X,STAT_Y+36,line,WHITE,BLACK);
}

/** scale samples in a ramp fashion **/

void ramp_dn(p, s, e)
char *p;
int s,e;
{
int len;
int k;
int v;

len = e - s;

for (k=s; k<=e; k++) {
   v = p[k];
   v = (v * (e - k))/len;
   p[k] = v;
   }
}


/** scale samples in a ramp fashion **/

void ramp_up(p, s, e)
char *p;
int s,e;
{
int len;
int k;
int v;

len = e - s;

for (k=s; k<=e; k++) {
   v = p[k];
   v = (v * (k-s))/len;
   p[k] = v;
   }
}

void scale(p, s, e, per)
char *p;
int s,e,per;
{
int k;
int v;

for (k=s; k<=e; k++) {
   v = p[k];
   v = (v * per)/100;
   if (v > 128) v = 128;
   if (v < -127) v = -127;
   p[k] = v;
   }
}
