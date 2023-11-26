#include <exec/types.h>
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "libraries/dosextens.h"
#include "psound.h"

struct Samp samp[MAX_SAMPLES];
struct IOAudio IOB;
struct IOAudio IOB2;
char *bufL[2],*bufR[2];
struct Screen *screen;        /** psound custom screen **/
struct Screen *ts;
int mark_drawn;               /** TRUE if part of graph outlined **/
int old_x1, old_x2;
int start_mark;
int end_mark;                 /** define points within a sample marked */
int insert_mark;              /** point within sample to insert at ***/
int marked_samp;              /** sample that start, end mark apply to */
int insert_samp;              /** sample where insertion is to happen **/
int cur;                      /** currently selected sample **/
int inserting;
short looping;                /** loop button was pressed **/
short digi_type;              /** PS_2 or PS_3 **/
short show_play=FALSE;        /** show line while playing??? **/
int vid_clock=3579546;        /** NTSC constant **/

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


void _main()
{
void stop_looping();
void pad_str();
void inc_gain(),dec_gain();
void mix_samples();
void scale();
void ramp_up();
void ramp_dn();
void draw_marked();
void update_se_mark();
void delete_sample();
int start_dragging();
int set_new_em();
int in_graph();
char *echo_buf;
int echo_buf_len;
APTR temp_ptr;
struct Process *mproc;
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
short first_digi;
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
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|GADGETUP|RAWKEY, /** also in filerequest!**/
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
first_digi = TRUE;      /** first time we call record or listen, set gain */

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

if (init_msg()) {    /** load all text msgs **/
   telluser2("Can't load ps.txt","",FALSE);
   CloseWindow(Window);
   CloseScreen(screen);
   closelibraries();
   exit(10);
   }


ShowTitle(screen, FALSE);

/* allocate audio channels 1 and 2 */

if (allocate_channel (Window,&IOB)==FALSE) {
   free_msg();
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
      free_msg();
      CloseWindow(Window);
      CloseScreen(screen);
      closelibraries();
      exit(10);
      }

   if ((bufR[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      FreeMem(bufL[0],BUFSIZE);  /* Bad job of clean up here !!!!! */
      deallocate_channel (&IOB);
      free_msg();
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
      telluser2(get_msg_ptr(232),"",FALSE);
      free_msg();
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
   telluser2(get_msg_ptr(232),"",FALSE);
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

show_msg(Window,100);   /** Welcome to perfect sound **/
SetMenuStrip(Window,Titles);   /* turn on menus */

/** show the names of any demos sounds that were loaded **/

if (samp[0].lmem)
   PutName(Window,samp[0].name,0,REG_VID);
if (samp[1].lmem)
   PutName(Window,samp[1].name,1,REG_VID);
if (samp[2].lmem)
   PutName(Window,samp[2].name,2,REV_VID);

show_play=TRUE;        /** show line while playing **/
mproc = (struct Process *)FindTask(0L);
temp_ptr = mproc->pr_WindowPtr;
mproc->pr_WindowPtr = (APTR)Window;

graph_update:

graph(graph_mode);
mark_drawn = FALSE;

sub_graph_update:
lgraph();
rgraph();

/* printf("cur==mark? %d %d\n",cur,marked_samp); */

if (cur==marked_samp)
   draw_marked(Window);

update:

memory=frags();


sprintf(line, get_msg_ptr(204), samp[cur].length);
pad_str(line);
putxy(Window,STAT_X,STAT_Y,line,WHITE,BLACK);

sprintf(line,get_msg_ptr(205), samp[cur].rate);
pad_str(line);
putxy(Window,STAT_X,STAT_Y+9,line,WHITE,BLACK);


sprintf(line,get_msg_ptr(206),memory/1024);
pad_str(line);
putxy(Window,STAT_X,STAT_Y+45,line,WHITE,BLACK);

if (samp[cur].type==RIGHT) {
   putxy(Window,STAT_X+176,STAT_Y+45,get_msg_ptr(222),WHITE,BLACK);
   }
else if (samp[cur].type==LEFT) {
   putxy(Window,STAT_X+176,STAT_Y+45,get_msg_ptr(223),WHITE,BLACK);
   }
else {
   putxy(Window,STAT_X+176,STAT_Y+45,get_msg_ptr(224),WHITE,BLACK);
   }
if (samp[cur].lmem==0) {
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
      draw_marked(Window);
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
         if (marked_samp==cur && close_to (mousex, end_mark)) {
            end_mark = set_new_em(mousex);
            draw_marked(Window);
            }
         else if (marked_samp==cur && close_to (mousex, start_mark)) {
            start_mark = end_mark-1;
            end_mark = set_new_em(mousex);
            draw_marked(Window);
            }
         else {
            start_mark = start_dragging(mousex);
            end_mark = start_mark+1;
            marked_samp = cur;
            }
         }
      update_se_mark(Window, line);
      erase_zoom_graphs();
      goto ignore;
      }

   if (end_up(mousex, mousey) && cur==marked_samp) {
      if (end_mark==samp[cur].length-1)
         goto ignore;
      end_mark +=2;
      rgraph();
      draw_marked(Window);
      update_se_mark(Window, line);
      loop_mode = INC_EM;
      goto ignore;
      }

   if (end_dn(mousex, mousey) && cur==marked_samp) {
      if (end_mark == start_mark+1)
         goto ignore;
      end_mark -=2;
      rgraph();
      draw_marked(Window);
      update_se_mark(Window, line);
      loop_mode = DEC_EM;
      goto ignore;
      }

   if (start_up(mousex, mousey) && cur==marked_samp) {
      if (start_mark+1==end_mark)
         goto ignore;
      start_mark +=2;
      lgraph();
      draw_marked(Window);
      update_se_mark(Window, line);
      loop_mode = INC_SM;
      goto ignore;
      }

   if (start_dn(mousex, mousey) && cur==marked_samp) {
      if (start_mark==0)
         goto ignore;
      start_mark -=2;
      lgraph();
      draw_marked(Window);
      update_se_mark(Window, line);
      loop_mode = DEC_SM;
      goto ignore;
      }

   i=(mousey-STAT_Y)/8;
   if (i >=0 && i < MAX_SAMPLES && samp[i].lmem!=0 && mousex < 300) {
      if (cur==i) {
         if (looping)
            stop_looping(Window);
         play(Window,&IOB,0,samp[cur].length,&samp[cur]);
         goto ignore;
         }
      PutName(Window,samp[cur].name,cur,REG_VID);
      cur=i;
      PutName(Window,samp[cur].name,cur,REV_VID);

      if (samp[cur].ctoctave>1 || samp[cur].repeat!=0) {
         sprintf(line,get_msg_ptr(207),
            samp[cur].oneshot,samp[cur].repeat,samp[cur].ctoctave);
         msg(Window,line);
         }
      goto graph_update;
      }
   }

if (class==RAWKEY) {    /** look for function key press. Plays octaves **/
/* printf("code %d\n",code); */
   if (code==64) {      /** space bar cancels **/
      if (inserting) {
         inserting=FALSE;
         show_msg(Window,101);
         goto graph_update;
         }
      goto ignore;
      }
   if (code==25) {
      if (looping)
         stop_looping(Window);
      play(Window,&IOB,start_mark,end_mark-start_mark+1,&samp[cur]);
      goto ignore;
      }

   if (code==40) {
      graph_mode=GRAPH_LEFT;
      if (samp[cur].type==STEREO)
         show_msg(Window,102);
      goto graph_update;
      }

   if (code==19) {
      graph_mode=GRAPH_RIGHT;
      if (samp[cur].type==STEREO)
         show_msg(Window,103);
      goto graph_update;
      }

   if (code>89 || code<80)  goto ignore;

   if (mem_type(samp[cur].lmem)!=MEMF_CHIP) {
      show_msg(Window,105);
      goto ignore;
      }

   if (samp[cur].ctoctave==0) {
      show_msg(Window,104);
      goto ignore;
      }
   if ((i=code-80)>=samp[cur].ctoctave) {
      show_msg(Window,105);
      goto ignore;
      }
   k=(powr(2,i)-1)*(samp[cur].oneshot+samp[cur].repeat);
   len=powr(2,i)*samp[cur].oneshot;
   if (even(k)!=k) {
      show_msg(Window,106);
      k=even(k);
      }
   if (even(len)!=len) {
      show_msg(Window,107);
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
      show_msg(Window,108);
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
   if (looping)
      stop_looping(Window);
   if (marked_samp==cur)
      play(Window,&IOB,0,start_mark,&samp[cur]);
   goto ignore;
   }

if (click && gad_sel(PLAY_RNG_X,PLAY_RNG_Y,mousex,mousey)) {
   if (looping)
      stop_looping(Window);
   if (marked_samp==cur)
      play(Window,&IOB,start_mark,end_mark-start_mark+1,&samp[cur]);
   goto ignore;
   }

if (click && gad_sel(PLAY_LP_X,PLAY_LP_Y,mousex,mousey)) {
   if (looping)
      stop_looping(Window);
   if (marked_samp!=cur)
      goto ignore;
   if (samp[cur].type==STEREO) {
      show_msg(Window,109);
      goto ignore;
      }
   if (mem_type(samp[cur].lmem)!=MEMF_CHIP) {
      if (telluser(get_msg_ptr(233),get_msg_ptr(234))==FALSE)
         goto ignore;
      if ((p1=AllocMem(samp[cur].length, MEMF_CHIP))==0) {
         show_msg(Window, 110);
         goto ignore;
         }
      movmem(samp[cur].lmem,p1,samp[cur].length);
      FreeMem(samp[cur].lmem, samp[cur].length);
      samp[cur].lmem = p1;
      samp[cur].rmem = p1;
      }
   looping = TRUE;
   ClearMenuStrip(Window);   /** no menus till looping finishes **/
   IOB.ioa_Request.io_Unit=(struct Unit *)1;
   tochan (&IOB, samp[cur].lmem+start_mark,end_mark-start_mark+1,toperiod(samp[cur].rate),0);
   goto ignore;
   }

if (click && gad_sel(STOP_X,STOP_Y,mousex,mousey)) {
   stop_looping(Window);
   goto ignore;
   }

if (click && gad_sel(DEL_RNG_X,DEL_RNG_Y,mousex,mousey)) {     /* delete a range */
   if (looping)
      stop_looping(Window);
   if (marked_samp==-1) {
      show_msg(Window,111);
      goto ignore;
      }
   if (end_mark==samp[marked_samp].length-1 && start_mark==0) {
      cur = marked_samp;
      goto whole;
      }
   show_msg(Window,112);

   i=start_mark;  /* start deleting including this byte */
   j=end_mark;    /* include this byte.  Both markers are inclusive */
   j++;

   k=RUP(samp[marked_samp].length-(j-i));

   if (j<samp[marked_samp].length)
      movmem(samp[marked_samp].lmem+j,samp[marked_samp].lmem+i,samp[marked_samp].length-j);

    if (k<RUP(samp[marked_samp].length))
       FreeMem(samp[marked_samp].lmem+k,RUP(samp[marked_samp].length)-k);

   if (samp[marked_samp].type==STEREO) {
      if (j<samp[marked_samp].length)
         movmem(samp[marked_samp].rmem+j,samp[marked_samp].rmem+i,samp[marked_samp].length-j);
      if (k < RUP(samp[marked_samp].length))
         FreeMem(samp[marked_samp].rmem+k,RUP(samp[marked_samp].length)-k);
      }

   samp[marked_samp].length=samp[marked_samp].length-(j-i);
   msg(Window,"");
   samp[marked_samp].oneshot=0;
   marked_samp = -1;
   insert_samp = -1;
   goto graph_update;
  }

if (click && gad_sel(INSERT_X,INSERT_Y,mousex,mousey)) {      /* insert here */
   if (looping)
      stop_looping(Window);
   if (marked_samp==-1) {
      show_msg(Window, 113);
      goto ignore;
      }

   if (!inserting) {
      show_msg(Window, 114);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==-1) {
      show_msg(Window, 115);
      goto ignore;
      }

   inserting = FALSE;
   show_msg(Window, 112);
   move(Window,start_mark,end_mark,insert_mark,marked_samp,insert_samp);
   show_msg(Window,116);
   marked_samp = -1;
   insert_samp = -1;
   goto graph_update;
   }

if (click && gad_sel(SET_PLAYBACK_X,SET_PLAYBACK_Y,mousex,mousey)) {      /* enter new playback rate */
   if (looping)
      stop_looping(Window);
   if (samp[cur].lmem==0) goto ignore;
   samp[cur].rate=getint(Window,samp[cur].rate,1000,28867,get_msg_ptr(235),450);
   goto update;
   }

if (click && gad_sel(PASTE_X,PASTE_Y,mousex,mousey)) {      /* paste */
   if (looping)
      stop_looping(Window);
   if (marked_samp==-1) {
      show_msg(Window, 117);
      goto ignore;
      }
   if (!inserting) {
      show_msg(Window, 118);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==-1) {
      show_msg(Window, 119);
      goto ignore;
      }

   inserting=FALSE;
   show_msg(Window, 112);

   i = insert_mark + end_mark - start_mark + 1;
   if (i >= samp[insert_samp].length)
      i = samp[insert_samp].length;

   for (k = insert_mark; k < i; k++) {
      j = *(samp[insert_samp].lmem+k);
      j = j + *(samp[marked_samp].lmem+k-insert_mark+start_mark);
      j = j/2;
      *(samp[insert_samp].lmem+k) = j;
      }

   if (samp[insert_samp].type==STEREO && samp[marked_samp].type==STEREO)
      for (k = insert_mark; k < i; k++) {
         j = *(samp[insert_samp].rmem+k);
         j = j + *(samp[marked_samp].rmem+k-insert_mark+start_mark);
         j = j/2;
         *(samp[insert_samp].rmem+k) = j;
         }
   show_msg(Window, 120);
   goto graph_update;
   }

/* copy range to a new slot */

if (click && gad_sel(NEW_SLOT_X, NEW_SLOT_Y, mousex, mousey)) {
   if (looping)
      stop_looping(Window);
   if (samp[cur].lmem==0) goto ignore;
   len=end_mark-start_mark+1;
   if (marked_samp==-1) {
      show_msg(Window,121);
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,get_msg_ptr(225),400);
   if (line[0]==0) strcpy(line,get_msg_ptr(226));
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

   show_msg(Window,112);

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
   if (looping)
      stop_looping(Window);
   if (samp[cur].lmem==0) goto ignore;
   show_msg(Window,122);
   if ((i=pickslot(Window))==-1) goto ignore;
   show_msg(Window,123);
   if ((k=pickslot(Window))==-1) goto ignore;
   len=samp[i].length+samp[k].length;
   move(Window,0,samp[i].length,samp[k].length,i,k);
   goto graph_update;
   }

/** search for zero crossings **/

if (click && gad_sel(FIND_LOOP_X,FIND_LOOP_Y,mousex,mousey)) {
   if (marked_samp==-1 || marked_samp!=cur) {
      show_msg(Window,124);
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
   draw_marked(Window);
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
   for(i=0;i<MAX_SAMPLES;i++) {
      if (samp[i].lmem!=0) {
         if (k==FALSE) {
            if (telluser(get_msg_ptr(236),get_msg_ptr(237))==FALSE)   goto ignore;
            k=TRUE;
            }
         delete_sample(i);
         PutName(Window,"",i,REG_VID);
         }
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
#if DEMO
   msg(Window,"Function Disabled in DEMO");
#else
   strcpy(buf,path);
   add(buf,samp[cur].name);
   switch (subnum) {
   case 0: save_IFF(Window,&samp[cur],buf,FALSE);
           break;
   case 1: if (marked_samp==cur)
              save_samp(Window,&samp[cur],buf,start_mark,end_mark);
           else
              save_samp(Window,&samp[cur],buf,0,0);
           break;
   case 2: save_IFF(Window,&samp[cur],buf,TRUE);
           break;
   case 3: save_ASF(Window,&samp[cur],buf);
           break;
   }
#endif
   goto ignore;
   }

if (menunum==0 && itemnum==3) {      /* save AS ... */
#if DEMO
   msg(Window,"Function Disabled in DEMO");
#else
   strcpy(line,samp[cur].name);
   i=get_fname(Window,screen,get_msg_ptr(227),line,path);
   if (i==NULL) goto ignore;
   strcpy(buf,path);
   add(buf,line);   /* appends a file or directory to a path */

   switch (subnum) {
      case 0: save_IFF(Window,&samp[cur],buf,FALSE);
              break;
      case 1: if (marked_samp==cur)
                 save_samp(Window,&samp[cur],buf,start_mark,end_mark);
              else
                 save_samp(Window,&samp[cur],buf,0,0);
              break;
      case 2: save_IFF(Window,&samp[cur],buf,TRUE);
              break;
      case 3: save_ASF(Window,&samp[cur],buf);
              break;
      }
   
   /** FIX bug in filerequest that turns off MOUSEMOVES **/

ModifyIDCMP(Window,MOUSEMOVE|MENUPICK|MOUSEBUTTONS|GADGETUP|RAWKEY);
ReportMouse(TRUE,Window);

#endif
   goto ignore;
   }

/** 4, delete sample **/
if (menunum==0 && itemnum==4) {      /* delete */
   strcpy(line,"");
   i=get_fname(Window,screen,get_msg_ptr(228),line,path);
   if (i==NULL) goto ignore;
   strcpy(buf,path);
   add(buf,line);   /* appends a file or directory to a path */
   i=remove(buf);
   if (i)
      show_msg(Window,125);
   else
      show_msg(Window,126);
   goto ignore;
   }

if (menunum==0 && itemnum==6) {    /* about */
   show_msg(Window,255);   /* not public domain msg */
   aboutps(Window);
   goto ignore;
   }

if (menunum==0 && itemnum==7) {     /* quit */
   if (!telluser(get_msg_ptr(238),""))
      goto ignore;
close_down:
   ClearMenuStrip(Window);
   filter_on();      /** just in case user has filter turned off **/
   deallocate_channel (&IOB);
   free_msg();
   mproc = (struct Process *)FindTask(0L);
   mproc->pr_WindowPtr = temp_ptr;        /** restore original pointer **/
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
   if (marked_samp==-1) {
      show_msg(Window,128);
      goto ignore;
      }
   show_msg(Window,127);

   flip(samp[marked_samp].lmem+start_mark,end_mark-start_mark+1);
   if (samp[marked_samp].type==STEREO) {
      flip(samp[marked_samp].rmem+start_mark,end_mark-start_mark+1);
   }
   msg(Window,"");
   goto graph_update;
   }

if (menunum==1 && itemnum==1) {   /* Zero Range */
   if (samp[cur].lmem==0) goto ignore;
   if (marked_samp==-1) {
      show_msg(Window,128);
      goto ignore;
      }
   show_msg(Window,112);
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
      show_msg(Window, 130);
      goto ignore;
      }
   show_msg(Window,112);
   ramp_up(samp[marked_samp].lmem, start_mark, end_mark);
   if (samp[marked_samp].type==STEREO)
      ramp_up(samp[marked_samp].rmem, start_mark, end_mark);
   msg(Window,"");
   goto graph_update;
   }

if (menunum==1 && itemnum==3) {  /** ramp down **/
   if (marked_samp==-1) {
      show_msg(Window, 130);
      goto ignore;
      }
   show_msg(Window,112);
   ramp_dn(samp[marked_samp].lmem, start_mark, end_mark);
   if (samp[marked_samp].type==STEREO)
      ramp_dn(samp[marked_samp].rmem, start_mark, end_mark);
   msg(Window,"");
   goto graph_update;
   }


if (menunum==1 && itemnum==4) {  /** scale **/
   if (marked_samp==-1) {
      show_msg(Window, 130);
      goto ignore;
      }
   i=getint(Window,100,1,300, get_msg_ptr(239),350);
   show_msg(Window,112);
   scale(samp[marked_samp].lmem, start_mark, end_mark, i);
   if (samp[marked_samp].type==STEREO)
      scale(samp[marked_samp].rmem, start_mark, end_mark, i);
   msg(Window,"");
   goto graph_update;
   }

/** Mix (5) **/
if (menunum==1 && itemnum==5) {
   if (marked_samp==-1) {
      show_msg(Window, 133);
      goto ignore;
      }

   if (!inserting) {
      show_msg(Window, 134);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==-1) {
      show_msg(Window, 135);
      goto ignore;
      }

   inserting=FALSE;
   show_msg(Window, 112);
   mix_samples(&samp[insert_samp],&samp[marked_samp]);
   show_msg(Window, 120);
   goto graph_update;
   }

/** Zoom Range (6) **/
if (menunum==1 && itemnum==6) {
   graph(graph_mode|GRAPH_ZOOM);
   show_msg(Window,137);
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
      goto sub_graph_update;
      }
   }

/** Low pass filter (7) **/

if (menunum==1 && itemnum==8) {
   if (samp[cur].lmem==0) goto ignore;
   i=getint(Window,4000,1000,10000, get_msg_ptr(240),350);
   show_msg(Window,112);
   lp_filter(samp[cur].lmem, samp[cur].length, samp[cur].rate, i);
   if (samp[cur].type==STEREO)
      lp_filter(samp[cur].rmem, samp[cur].length, samp[cur].rate, i);
   msg(Window,"");
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
   show_msg(Window,138);
   if ((i=pickslot(Window))==-1) goto ignore;
   if (samp[i].type==STEREO) {
      show_msg(Window,139);
      goto ignore;
      }
   show_msg(Window,140);
   if ((k=pickslot(Window))==-1) goto ignore;
   if (samp[k].type==STEREO) {
      show_msg(Window,139);
      goto ignore;
      }
   if (k==i) {
      show_msg(Window,142);
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
   samp[ss].oneshot=0;
   PutName(Window,"",sl,REG_VID);
   cur=ss;
   samp[sl].lmem=0;
   samp[sl].rmem=0;
   samp[sl].length=0;
   samp[sl].name=0;
   samp[sl].oneshot=0;
   PutName(Window,samp[cur].name,cur,REV_VID);
   show_msg(Window,143);
   if (marked_samp==i || marked_samp==k) {
      marked_samp = -1;
      insert_samp = -1;
      }
   goto graph_update;
   }

/* Break up a stereo into two new slots */

if (menunum==1 && itemnum==11) {
   if (samp[cur].lmem==0) goto ignore;
   if (samp[cur].type!=STEREO) {
      show_msg(Window,144);
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
      FreeMem(samp[i].name,strlen(samp[i].name+1));
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
   show_msg(Window,145);
   if (marked_samp==cur) {
      marked_samp = -1;
      insert_samp = -1;
      }
   goto graph_update;
   }

if (menunum==1 && itemnum==12) {       /** Resample to new sampling rate **/
   if (samp[cur].lmem==0) goto ignore;
   j=cur;
   cur=resample(Window, cur);
   if (cur==j)
      goto ignore;
   else
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

   if ((i=getslot(Window))==-1) goto ignore;
   len=(powr(2,k)-1)*samp[oct[0]].length;
   p1=AllocMem(len,MEMF_CHIP);
   if (p1==0) {
      show_msg(Window,148);
      goto ignore;
      }
   samp[i].length=len;
   samp[i].type=RIGHT;
   samp[i].rate=samp[oct[0]].rate;
   samp[i].lmem=p1;
   samp[i].rmem=p1;
   samp[i].ctoctave=k;
   if (marked_samp==oct[0]) {
      show_msg(Window,149);
      samp[i].oneshot=start_mark;
      samp[i].repeat=samp[marked_samp].length-start_mark;
      }
   else {
      show_msg(Window,150);
      samp[i].oneshot=samp[oct[0]].length;
      samp[i].repeat=0;
      }

   /* move all octaves into new instrument */

   len=samp[oct[0]].length;
   temp=0;
   for (j=0;j<k;j++) {
      if (samp[oct[j]].length!=len)
         show_msg(Window,151);
      movmem(samp[oct[j]].lmem,samp[i].lmem+temp,len);
      temp=temp+len;
      len=len*2;
      }


   /* get a name for this sample */

   line[0]=0;
   getstr(Window,line,get_msg_ptr(229),400);
   if (line[0]==0) strcpy(line,get_msg_ptr(230));
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

  samp[i].cycles=getint(Window,8,0,256,get_msg_ptr(231),256);

 /* sampperhicycle should be power of 2 for most programs */

   j=FALSE;
   for (k=0;k<16;k++)
      if (powr(2,k)==samp[i].cycles) j=TRUE;
   if (j==FALSE)
       show_msg(Window,152);
   else
      show_msg(Window,153);
  /* highlight new instrument */

   PutName(Window,samp[cur].name,cur,REG_VID);
   cur=i;
   PutName(Window,samp[cur].name,i,REV_VID);
   goto graph_update;
   }

/* cut period in half by eliminating samples (f=f*2) */

if (menunum==2 && itemnum==1) {
   if (samp[cur].lmem==0) goto ignore;
   if (samp[cur].type==STEREO) {
      show_msg(Window,154);
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,get_msg_ptr(225),400);
   if (line[0]==0) strcpy(line,"*2");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   show_msg(Window,112);
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
      show_msg(Window,154);
      goto ignore;
      }
   if ((i=getslot(Window))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,get_msg_ptr(225),400);
   if (line[0]==0) strcpy(line,"/2");
   samp[i].name=AllocMem(strlen(line)+1,0);
   if (samp[i].name==0) {
      nomem(Window);
      goto ignore;
      }
   show_msg(Window,112);
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
   show_msg(Window,256);
   i=getsamprate(Window,(vid_clock/10)/record_speed,2000,50000,
            get_msg_ptr(242),450);
   record_speed=(vid_clock/10)/i;
   i=(vid_clock/10)/record_speed;
   if (i > 32000) {
      show_msg(Window,257);
      }
   else {
      sprintf(line,get_msg_ptr(208),i);
      msg(Window, line);
      }
   }

if (menunum==3 && itemnum==1) {  /* View Signal Levels */
	Disable();
	for (i=0; i < 2000000; i++) ;
	Enable();
   /* level(Window); */
   goto ignore;
   }
   
if (menunum==3 && itemnum==2) {            /* monitor the digitizer */
   set_digi_type();    /** PS_3 or PS_2 ? **/
   if (digi_type==PS_2) {
      show_msg(Window, 254);  /** only works with 3 **/
      goto ignore;
      }
   ts=openscr(320,1);
   if (ts==0) {
      nomem(Window);
      goto ignore;
      }
   rec_msg(ts,get_msg_ptr(156),0);
   rec_msg(ts,get_msg_ptr(157),1);
   if (digi_type!=PS_2) {
      rec_msg(ts,get_msg_ptr(158),2);
      rec_msg(ts,get_msg_ptr(159),3);
      rec_msg(ts,get_msg_ptr(160),4);
      if (first_digi) {
         rec_msg(ts,get_msg_ptr(161),6);
         set_auto_gain();
         rec_msg(ts,"",6);
         first_digi = FALSE;
         }
      }
   switch (subnum) {
   case 0: listen(RIGHT);
           break;
   case 1: listen(LEFT);
           }
   CloseScreen(ts);
   ActivateWindow(Window);
   goto ignore;
   }

if (menunum==3 && itemnum==3)  {  /* record */
#if DEMO
   msg(Window,"This function disabled for DEMO");
#else
   set_digi_type();     /** PS_3 or PS_2 ? **/
   if (digi_type==PS_2) {
      i=15500;
      show_msg(Window, 254);  /** only works with 3 **/
      goto ignore;
      }
   else {
      i=14000;
      }
   if (subnum==0 && (vid_clock/10)/record_speed > i) {
      sprintf(line,get_msg_ptr(209),i);
      msg(Window,line);
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
      default: show_msg(Window,162);
              goto ignore;
      }
   samp[i].type=k;
   ts=openscr(320,1);
   if (ts==0) {
      FreeMem(p1,memory);
      if (k==STEREO) FreeMem(p2,memory);
      nomem(Window);
      goto ignore;
      }
   rec_msg(ts,get_msg_ptr(156),0);
   rec_msg(ts,get_msg_ptr(157),1);
   if (digi_type!=PS_2) {
      rec_msg(ts,get_msg_ptr(158),2);
      rec_msg(ts,get_msg_ptr(159),3);
      rec_msg(ts,get_msg_ptr(160),4);
      if (first_digi) {
         rec_msg(ts,get_msg_ptr(161),6);
         set_auto_gain();
         rec_msg(ts,"",6);
         first_digi = FALSE;
         }
      }
   listen(k);   /* listen before we start recording */
   rec_msg(ts,get_msg_ptr(163),0);
   rec_msg(ts,get_msg_ptr(164),1);
   rec_msg(ts,"",2);
   rec_msg(ts,"",3);
   rec_msg(ts,"",4);

   samplen=digitze (Window,p1,p2,record_speed,memory,k);
   CloseScreen(ts);
   ActivateWindow(Window);
   samplen=even(samplen);
   if (samplen==0) {
      show_msg(Window,165);
      FreeMem(samp[i].lmem,memory);
      if (k==STEREO) FreeMem(samp[i].rmem,memory);
      samp[i].lmem=0;
      goto ignore;
      }
   if (samplen==memory)
      show_msg(Window,166);
   else {
      FreeMem(p1+RUP(samplen),RUP(memory) - RUP(samplen));
      sprintf(line,get_msg_ptr(210),samplen);
      if (k==STEREO) {
         FreeMem(p2+RUP(samplen),RUP(memory) - RUP(samplen));
         sprintf(line,get_msg_ptr(210),samplen*2);
         }
      msg(Window,line);
      }
   samp[i].length=samplen;
   samp[i].rate=(vid_clock/10)/record_speed;
   line[0]=0;
   getstr(Window,line,get_msg_ptr(225),400);
   if (strlen(line) > 30) line[31]=0;
   if (line[0]==0) strcpy(line,get_msg_ptr(226));
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
#endif
   goto graph_update;
   }

if (menunum==4 && itemnum==0) {   /** filter on or off **/
   if (do_on) {
      filter_on();
      do_on = FALSE;
      show_msg(Window,167);
      goto ignore;
      }
   else {
      filter_off();
      do_on = TRUE;
      show_msg(Window,168);
      goto ignore;
      }
   }

if (menunum==4 && itemnum==1) {   /** turn demos off **/
   if (rename("ps.title","no_ps.title")==0)
      show_msg(Window, 169);
   else
      show_msg(Window, 170);
   goto ignore;
   }

if (menunum==4 && itemnum==2) {   /** turn demos on **/
   if (rename("no_ps.title","ps.title")==0)
      show_msg(Window, 171);
   else
      show_msg(Window, 172);
   goto ignore;
   }

if (menunum==4 && itemnum==3) {   /** real time echo **/
   set_digi_type();     /** PS_3 or PS_2 ? **/
   if (digi_type!=PS_3) {
      show_msg(Window, 248);     /** only works with 3.0 **/
      goto ignore;
      }
   echo_buf_len=getint(Window,300,10,3000,get_msg_ptr(246),450);
   echo_buf_len = (echo_buf_len*(vid_clock/250L))/1000L;
   ts=openscr(320,1);
   if (ts==0) {
      nomem(Window);
      goto ignore;
      }
   rec_msg(ts,get_msg_ptr(244),0);
   rec_msg(ts,get_msg_ptr(157),1);
   echo_buf = AllocMem(echo_buf_len, MEMF_CLEAR);
   if (echo_buf) {
      Disable();
      real_echo(echo_buf, echo_buf_len);
      Enable();
      FreeMem(echo_buf, echo_buf_len);
      }
   CloseScreen(ts);
   ActivateWindow(Window);
   goto ignore;
   }

if (menunum==4 && itemnum==4) {   /** real time delay **/
   set_digi_type();     /** PS_3 or PS_2 ? **/
   if (digi_type!=PS_3) {
      show_msg(Window, 248);     /** only works with 3.0 **/
      goto ignore;
      }
   echo_buf_len=getint(Window,500,10,3000,get_msg_ptr(247),450);
   echo_buf_len = (echo_buf_len*(vid_clock/250))/1000;
   ts=openscr(320,1);
   if (ts==0) {
      nomem(Window);
      goto ignore;
      }
   rec_msg(ts,get_msg_ptr(245),0);
   rec_msg(ts,get_msg_ptr(157),1);
   echo_buf = AllocMem(echo_buf_len, MEMF_CLEAR);
   if (echo_buf) {
      Disable();
      real_delay(echo_buf, echo_buf_len);
      Enable();
      FreeMem(echo_buf, echo_buf_len);
      }
   CloseScreen(ts);
   ActivateWindow(Window);
   goto ignore;
   }

   goto update;
}   

/** given the pointer position, update the global variable "marker" **/

int start_dragging(mousex)

int mousex;
{
int marker;    /** sample where dragging starts **/

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

void draw_marked(Window)

struct Window *Window;

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
   if (end_mark < start_mark) {
      if (start_mark - end_mark >= 131072) {
         show_msg(Window, 251);
         stop_looping(Window);
         }
      else {
         new_loop_section(0,samp[cur].lmem+end_mark-1,start_mark-end_mark+1);
         }
      }
   else {
      if (end_mark-start_mark >= 131072) {
         show_msg(Window, 251);
         stop_looping(Window);
         }
      else {
         new_loop_section(0,samp[cur].lmem+start_mark,end_mark-start_mark+1);
         }
      }
   }

if (old_x1==x1 && old_x2==x2 && mark_drawn)
   return;

if (mark_drawn) {    /** just draw the new stuff.  x1  < x2 **/
   if (x1 < old_x1)
      RectFill(&screen->RastPort, x1, GRAPH_Y, old_x1-1, GRAPH_Y+GRAPH_H-1);
   if (x1 > old_x1)
      RectFill(&screen->RastPort, old_x1, GRAPH_Y, x1-1, GRAPH_Y+GRAPH_H-1);
   if (x2 > old_x2)
      RectFill(&screen->RastPort, old_x2+1, GRAPH_Y, x2, GRAPH_Y+GRAPH_H-1);
   if (x2 < old_x2)
      RectFill(&screen->RastPort, x2+1, GRAPH_Y, old_x2, GRAPH_Y+GRAPH_H-1);
   }
else {
   mark_drawn=TRUE;
   RectFill(&screen->RastPort, x1, GRAPH_Y, x2, GRAPH_Y+GRAPH_H-1);
   }
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

close_to(mousex, mark)
int mousex, mark;
{
int i;

i = mousex - GRAPH_X - mark*GRAPH_W/samp[cur].length;
if (i < 0)
   i = -i;
if (i < 10)
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
samp[val].name=0;
samp[val].rate=0;
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
   sprintf(line,get_msg_ptr(212));
else
   sprintf(line,get_msg_ptr(213), start_mark,samp[marked_samp].lmem[start_mark]);
putxy(Window,STAT_X,STAT_Y+18,line,WHITE,BLACK);

if (marked_samp==-1)
   sprintf(line,get_msg_ptr(214));
else
   sprintf(line,get_msg_ptr(215), end_mark,samp[marked_samp].lmem[end_mark]);
putxy(Window,STAT_X,STAT_Y+27,line,WHITE,BLACK);

if (insert_samp==-1)
   sprintf(line,get_msg_ptr(216));
else
   sprintf(line,get_msg_ptr(217), insert_mark,samp[insert_samp].lmem[insert_mark]);
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
   if (v > 126) v = 127;
   if (v < -126) v = -127;
   p[k] = v;
   }
}

void pad_str(line)

char *line;

{
strncat(line,"            ",30-strlen(line));
}

void stop_looping(Window)
struct Window *Window;
{
IOB2.ioa_Request.io_Unit=(struct Unit *)1;
IOB2.ioa_Request.io_Command=CMD_RESET;
DoIO (&IOB2);
looping = FALSE;
SetMenuStrip(Window,Titles);   /** menus were disabled when looping started*/
}

