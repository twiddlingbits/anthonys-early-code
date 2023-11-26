#include <exec/types.h>
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "libraries/dosextens.h"
#include "/include/psound.h"


/*** ERASE FUNCTION NEEDS WORK!!!!!!!!!! *****/

extern struct studioBase *studioBase;

struct Screen *ts;
int mark_drawn;               /** TRUE if part of graph outlined **/
int old_x1, old_x2;
int start_mark;
int end_mark;                 /** define points within a sample marked */
int insert_mark;              /** point within sample to insert at ***/
struct Disk_Samp *marked_samp;  /** sample that start, end mark apply to */
struct Disk_Samp *insert_samp;  /** sample where insertion is to happen **/
struct Disk_Samp *cur;        /** currently selected sample **/
int inserting;
short looping;                /** loop button was pressed **/
short digi_type;              /** PS_2 or PS_3 **/
short show_play=FALSE;        /** show line while playing??? **/
int vid_clock=3579546;        /** NTSC constant **/
short graph_mode;

int graph_width;
int graph_height;    /* edit window graph */

/** some wierd things that I shall try and fix in the future:

   1. Left always pointes to the data for mono sounds. Right points
      to the right channel data if it is a stereo sound.

   Anthony Wood 2-22-87 **/

/***
   1. when looping, start_mark is always on even and end_mark is always on
      an odd byte
   2. All samples are always even, and only an even number of bytes can
      be edited. Cut, deleted, etc.
   3. markers are inclusive ie. sm=4 and em=5 marks two bytes
**/

/** local static variables **/

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
short get_samplel1();
short get_samplel2();
short get_sampler1();
short get_sampler2();
short get_left_sample1();
short get_left_sample2();
short get_right_sample1();
short get_right_sample2();
struct Disk_Samp *get_sample_ptr();
struct Module *find_module();
void close_edit();

int start_dragging();
int set_new_em();
int in_graph();
static char *echo_buf;
static int echo_buf_len;
static APTR temp_ptr;
static struct Process *mproc;
extern struct Menu Titles[];    /* menus done with MenuEd, Shareware */
extern struct list *open_list;
static int memory,samplen;
static int record_speed;               /* record speed in samples/second */
static short loop_mode;
static short dragging;
static short do_on;
static short first_digi;
static int click;
static float flt;
static char *AllocMem(),*p1,*p2,*x;
static char line[80],buf[80];
static USHORT menunum,itemnum,subnum;
static SHORT oct[MAX_SAMPLES];
struct Window *edit_win;
extern int graph_width;
extern int graph_height;
static struct IntuiMessage *message;
static struct Image image;
static struct Screen *openscr();
static struct Disk_Samp temp_samp;

void open_edit(m)
struct Module *m;
{
static struct NewWindow NewWindow = {
   0,
   0,
   500,
   75,
   BLACK,
   WHITE,
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|GADGETUP|RAWKEY|CLOSEWINDOW|NEWSIZE,
   WINDOWSIZING|REPORTMOUSE|ACTIVATE|NOCAREREFRESH|SMART_REFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   0, /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

memory=frags();        /* find largest amount of free memeory */
record_speed=20;
dragging   = FALSE;
inserting  = FALSE;        /** true if finding the insert point **/
mark_drawn = FALSE;
do_on = FALSE;
marked_samp = NULL;
insert_samp = NULL;
cur = NULL;
loop_mode = INC_SM;
graph_mode = GRAPH_LEFT;
first_digi = TRUE;      /** first time we call record or listen, set gain */
graph_width = GRAPH_W;
graph_height = GRAPH_H;

NewWindow.Screen=studioBase->screen;
NewWindow.Width = GRAPH_W+GRAPH_X+GRAPH_X;
NewWindow.Height = GRAPH_H+GRAPH_Y+GRAPH_Y;

if ((m->window=(struct Window *)OpenWindow(&NewWindow))==NULL) {
   telluser("Can't open edit window","");
   return;
   }

edit_win = m->window;


SetMenuStrip(edit_win,Titles);   /* turn on menus */

show_play=TRUE;        /** show line while playing **/
mproc = (struct Process *)FindTask(0L);
temp_ptr = mproc->pr_WindowPtr;
mproc->pr_WindowPtr = (APTR)edit_win;
if (open_list->current!=-1)
   cur=(struct Disk_Samp *)open_list->active_entry->user_data;
refresh_edit_win();
}

/*****************************************************/

void do_edit(class,code,mousex,mousey,gad)
struct Gadget *gad;
ULONG class;
USHORT code;
int mousey,mousex;
{
int len, j, k;
int i,temp;
struct Disk_Samp *s;
struct MinNode *node;

if (class==NEWSIZE) {
   graph_width = edit_win->Width - GRAPH_X - GRAPH_X;
   graph_height = edit_win->Height - GRAPH_Y - GRAPH_Y;
/*   BeginRefresh(edit_win); can't use with SMARTREFRESH in this case */
   refresh_edit_win();
/*    EndRefresh(edit_win, TRUE); */
   return;
   }

if (class==MOUSEMOVE) {       /* dragging **/
   if (dragging) {
      if (inserting) {
         insert_mark = set_new_em(mousex);
         insert_samp = cur;
         }
      else {
         end_mark = set_new_em(mousex);
         }
      draw_marked(edit_win);
      }
   do {
      message=(struct IntuiMessage *)GetMsg(edit_win->UserPort);
      if (message==0)  {
         if (dragging) update_se_mark(edit_win, line);
         goto ignore;
         }
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      gad=(struct Gadget *)message->IAddress;
      ReplyMsg(message);
/*      handle_list(&ed_list, class, code, mousex, mousey); */
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
        /* lgraph();
         rgraph(); */
         update_se_mark(edit_win, line);
         }
      goto ignore;
      }

   click = TRUE;

   if (in_graph(mousex, mousey) && cur!=NULL) {
      dragging = TRUE;
      if (inserting) {
         insert_mark = start_dragging(mousex);
         insert_samp = cur;
         }
      else {
         if (marked_samp==cur && closeto (mousex, end_mark)) {
            end_mark = set_new_em(mousex);
            draw_marked(edit_win);
            }
         else if (marked_samp==cur && closeto (mousex, start_mark)) {
            start_mark = end_mark-1;
            end_mark = set_new_em(mousex);
            draw_marked(edit_win);
            }
         else {
            start_mark = start_dragging(mousex);
            end_mark = start_mark+1;
            marked_samp = cur;
            }
         }
      update_se_mark(edit_win, line);
      /* erase_zoom_graphs(); */
      goto ignore;
      }
   }

if (class==RAWKEY) {    /** look for function key press. Plays octaves **/
/* printf("code %d\n",code); */
   if (code==64) {      /** space bar cancels **/
      if (inserting) {
         inserting=FALSE;
         show_msg(edit_win,101);
         goto graph_update;
         }
      goto ignore;
      }
   if (code==25) {
      play(edit_win,start_mark,end_mark-start_mark+1,cur);
      goto ignore;
      }

   if (code==40) {
      graph_mode=GRAPH_LEFT;
      if (cur->type==STEREO)
         show_msg(edit_win,102);
      goto graph_update;
      }

   if (code==19) {
      graph_mode=GRAPH_RIGHT;
      if (cur->type==STEREO)
         show_msg(edit_win,103);
      goto graph_update;
      }

   goto ignore;
   }

if (class==MENUPICK && code==MENUNULL) {
   if (marked_samp==cur)
      play(edit_win,0,start_mark, cur);
   goto ignore;
   }

#if 0
if (click && gad_sel(PLAY_RNG_X,PLAY_RNG_Y,mousex,mousey)) {
   if (marked_samp==cur)
      play(edit_win,start_mark,end_mark-start_mark+1,cur);
   goto ignore;
   }

if (click && gad_sel(PLAY_LP_X,PLAY_LP_Y,mousex,mousey)) {
   if (marked_samp!=cur)
      goto ignore;
   if (cur->type==STEREO) {
      show_msg(edit_win,109);
      goto ignore;
      }
   looping = TRUE;
   IOB.ioa_Request.io_Unit=(struct Unit *)1;
   tochan (&IOB, cur->lmem+start_mark,end_mark-start_mark+1,toperiod(cur->rate),0);
   goto ignore;
   }

if (click && gad_sel(STOP_X,STOP_Y,mousex,mousey)) {
   stop_looping();
   goto ignore;
   }

/** search for zero crossings **/

if (click && gad_sel(FIND_LOOP_X,FIND_LOOP_Y,mousex,mousey)) {
   if (marked_samp==NULL || marked_samp!=cur) {
      show_msg(edit_win,124);
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
         if (end_mark+4 >= cur->length) break;
         end_mark +=2;
         temp = end_mark;
         }
      else if (loop_mode==DEC_EM) {
         if (end_mark-4 <= start_mark) break;
         end_mark -=2;
         temp = end_mark;
         }
      i = get_left_sample1(cur, temp);
      j = get_left_sample1(cur, temp-1);
      k = get_left_sample1(cur, temp+1);
      if (i > -2 && i < 2)
         if ( (j>0 && k<0) || (j<0 && k>0)) break;
      }
   draw_marked(edit_win);
/*
   if (loop_mode==INC_SM || loop_mode==DEC_SM)
      lgraph();
   else
      rgraph();
*/
   update_se_mark(edit_win, line);
   goto ignore;
   }
#endif

if (class==CLOSEWINDOW) {
   if (!askuser(get_msg_ptr(238),""))
      goto ignore;
   goto close_down;
   }

if (class!=MENUPICK)
   goto ignore;

/** ALL GADGET commands must be above here **/
/** Only menu options follow **/

menunum=MENUNUM(code);
itemnum=ITEMNUM(code);
subnum=SUBNUM(code);

if (menunum==0 && itemnum==0) {     /** New **/
   node=studioBase->samps.mlh_Head;
   if (node->mln_Succ)
      if (askuser(get_msg_ptr(236),get_msg_ptr(237))==FALSE)
         goto ignore;

   while (node->mln_Succ) {
      s=(struct Disk_Samp *)node;
      node = node->mln_Succ;
      delete_sample(s);
      }
   marked_samp = NULL;
   insert_samp = NULL;
   goto graph_update;
   }

if (menunum==0 && itemnum==1) {      /** Open **/
   newpointer(PTR_BUSY);
   i=load_samp(edit_win,studioBase->defaults.samp_io_path);
   if (i!=NO_ERROR) {
      show_error(i);
      goto ignore;
      }
newpointer(PTR_NORMAL);
printf("open name:%s len:%d",cur->name, cur->length);
   goto update;
   }

if (menunum==0 && itemnum==2) {      /* save ... */
#if DEMO
   msg(edit_win,"Function Disabled in DEMO");
#else
   strcpy(buf,studioBase->defaults.samp_io_path);
   add(buf,cur->name);
   newpointer(PTR_BUSY);
   switch (subnum) {
   case 0: telluser("AIFF not implemented yet",0);
           break;
   case 1: save_IFF(edit_win,cur,buf,FALSE);
            break;
   case 2: if (marked_samp==cur)
              save_samp(edit_win,cur,buf,start_mark,end_mark);
           else
              save_samp(edit_win,cur,buf,0,cur->length-1);
           break;
   }
   newpointer(PTR_NORMAL);
#endif
   goto ignore;
   }

if (menunum==0 && itemnum==3) {      /* save AS ... */
#if DEMO
   msg(edit_win,"Function Disabled in DEMO");
#else
   strcpy(line,cur->name);
   i=get_fname(edit_win,studioBase->screen,get_msg_ptr(227),line,studioBase->defaults.samp_io_path);
   if (i==NULL) goto ignore;
   strcpy(buf,studioBase->defaults.samp_io_path);
   add(buf,line);   /* appends a file or directory to a path */

   newpointer(PTR_BUSY);
   switch (subnum) {
      case 0: telluser("AIFF not implemented yet",0);
              break;
      case 1: save_IFF(edit_win,cur,buf,FALSE);
               break;
      case 2: if (marked_samp==cur)
                 save_samp(edit_win,cur,buf,start_mark,end_mark);
              else
                 save_samp(edit_win,cur,buf,0,cur->length-1);
      }
#endif
   newpointer(PTR_NORMAL);
   goto ignore;
   }

/** 4, delete sample **/
if (menunum==0 && itemnum==4) {      /* delete */
   strcpy(line,"");
   i=get_fname(edit_win,studioBase->screen,get_msg_ptr(228),line,studioBase->defaults.samp_io_path);
   if (i==NULL) goto ignore;
   strcpy(buf,studioBase->defaults.samp_io_path);
   add(buf,line);   /* appends a file or directory to a path */
   i=remove(buf);
   if (i)
      show_msg(edit_win,125);
   else
      show_msg(edit_win,126);
   goto ignore;
   }

if (menunum==0 && itemnum==6) {    /* about */
   aboutps(edit_win);
   goto ignore;
   }

if (menunum==0 && itemnum==7) {     /* quit */
   if (!askuser(get_msg_ptr(238),""))
      goto ignore;
close_down:
   close_edit(find_module("SoundEdit"));
   edit_win=0;
   return;
   }

if (menunum==1 && itemnum==0) {
   return;
   }

if (menunum==1 && itemnum==1) {
   return;
   }

if (menunum==1 && itemnum==2) {     /* paste */
   if (marked_samp==NULL) {
      show_msg(edit_win, 117);
      goto ignore;
      }
   if (!inserting) {
      show_msg(edit_win, 118);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==NULL) {
      show_msg(edit_win, 119);
      goto ignore;
      }

   inserting=FALSE;
   newpointer(PTR_BUSY); 

   i = insert_mark + end_mark - start_mark + 1;
   if (i >= insert_samp->length)
      i = insert_samp->length;

   init_sample1(insert_samp, insert_mark);
   init_sample2(marked_samp, start_mark);

   for (k = insert_mark; k < i; k++) {
      j = get_left_sample1(insert_samp, k);
/* printf("1st j %d\n",j); */
      j = j + get_samplel2();
      j = j/2;
      put_samplel1(j);
      }

   if (insert_samp->type==marked_samp->type==STEREO) {
      init_sample1(insert_samp, k);
      init_sample2(marked_samp, start_mark);
      for (k = insert_mark; k < i; k++) {
         j = get_right_sample1(insert_samp, k);
         j = j + get_sampler2();
         j = j/2;
         put_sampler1(j);
         }
      }
   newpointer(PTR_NORMAL);
   show_msg(edit_win, 120);
   goto graph_update;
   }

if (menunum==1 && itemnum==3) {     /** erase **/
   if (marked_samp==NULL) {
      show_msg(edit_win,111);
      goto ignore;
      }
   if (end_mark==marked_samp->length-1 && start_mark==0) {
      cur = marked_samp;
      goto whole;
      }
   
   newpointer(PTR_BUSY);

   /* start_mark;  start deleting including this byte */
   /* end_mark;    End here.  Both markers are inclusive */

   create_sample(marked_samp->name,marked_samp->version+1,&s);  /** CHECK FOR ERROR HERE! **/

   init_sample2(marked_samp,0);
   for (i=0; i < start_mark; i++)
      put_samplel1(get_samplel2());

   init_sample2(marked_samp,end_mark+1);
   for (i=end_mark+1; i < marked_samp->length; i++)
      put_samplel1(get_samplel2());

   if (marked_samp->type==STEREO) {
      init_sample2(marked_samp,0);
      for (i=0; i < start_mark; i++)
         put_sampler1(get_sampler2());

      init_sample2(marked_samp,end_mark+1);
      for (i=end_mark+1; i < marked_samp->length; i++)
         put_sampler1(get_sampler2());
      }

   delete_sample(marked_samp); /** just close for undo ?**/
   add_list_entry(open_list, s->name, s);
   marked_samp=NULL;
   insert_samp = NULL;
   newpointer(PTR_NORMAL);
   goto graph_update;
  }

if (menunum==1 && itemnum==4) {  /** insert **/
   if (marked_samp==NULL) {
      show_msg(edit_win, 113);
      goto ignore;
      }

   if (!inserting) {
      show_msg(edit_win, 114);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==NULL) {
      show_msg(edit_win, 115);
      goto ignore;
      }

   inserting = FALSE;
   newpointer(PTR_BUSY);
   move(edit_win,start_mark,end_mark,insert_mark,marked_samp,insert_samp);
   show_msg(edit_win,116);
   marked_samp = NULL;
   insert_samp = NULL;
   newpointer(PTR_NORMAL);
   goto graph_update;
   }

if (menunum==1 && itemnum==5) {  /* copy range to a new slot */
   if (cur==NULL) goto ignore;
   len=end_mark-start_mark+1;
   if (marked_samp==NULL) {
      show_msg(edit_win,121);
      goto ignore;
      }
   line[0]=0;
   getstr(edit_win,line,get_msg_ptr(225),400);
   if (line[0]==0) strcpy(line,get_msg_ptr(226));
   i = create_sample(line, 0, &s);
   if (i!=NO_ERROR) {
      show_error(i);       /** prints error code **/
      goto ignore;
      }
   s->length=len;
   s->rate=cur->rate;
   s->type=cur->type;

   show_msg(edit_win,112);

   init_sample2(cur, start_mark);
   for (i=0; i < len; i++)
      put_samplel1(get_samplel2());

   if (cur->type==STEREO) {
      init_sample1(s, 0);
      init_sample2(cur, start_mark);
      for (i=0; i < len; i++)
         put_sampler1(get_sampler2());
      }

   msg(edit_win,"");
   add_list_entry(open_list, s->name, s);
   cur = s;
   goto graph_update;
   }

if (menunum==1 && itemnum==6) { /* append a slot to another slot */

#if 0 /******!!!!!!!!!!!!!! FIX THIS MENU!!!!! ***/

   if (cur==NULL) goto ignore;
   show_msg(edit_win,122);
   if ((i=pickslot(edit_win))==-1) goto ignore;
   show_msg(edit_win,123);
   if ((k=pickslot(edit_win))==-1) goto ignore;
   len=samp[i].length+samp[k].length;
   move(edit_win,0,samp[i].length,samp[k].length,i,k);
   goto graph_update;
#endif
   }

if (menunum==1 && itemnum==7) {  /** set playback rate **/
   if (cur==NULL) goto ignore;
   cur->rate=getint(edit_win,cur->rate,1000,28000,get_msg_ptr(235),450);
   }

if (menunum==2 && itemnum==0) {        /* flip marked range */
   if (cur==NULL) goto ignore;
   if (marked_samp==NULL) {
      show_msg(edit_win,128);
      goto ignore;
      }
   show_msg(edit_win,127);

   flip(marked_samp, start_mark, end_mark);    /** THIS NEEDS TO FLIP RANGE!!! not entire sample */
   msg(edit_win,"");
   goto graph_update;
   }

if (menunum==2 && itemnum==1) {   /* Zero Range */
   if (cur==NULL) goto ignore;
   if (marked_samp==NULL) {
      show_msg(edit_win,128);
      goto ignore;
      }
   show_msg(edit_win,112);
   init_sample1(marked_samp, start_mark);
   for (i=start_mark; i<=end_mark; i++)
      put_samplel1(0);
   if (marked_samp->type==STEREO) {
      init_sample1(marked_samp, start_mark);
      for (i=start_mark; i<=end_mark; i++)
         put_sampler1(0);
      }
      msg(edit_win,"");
      goto graph_update;
   }

/** Range Up (2) **/

if (menunum==2 && itemnum==2) {  /** scale range from zero to full scale **/
   if (marked_samp==NULL) {
      show_msg(edit_win, 130);
      goto ignore;
      }
   show_msg(edit_win,112);
   ramp_up(marked_samp, start_mark, end_mark, LEFT);
   if (marked_samp->type==STEREO)
      ramp_up(marked_samp, start_mark, end_mark, LEFT);
   msg(edit_win,"");
   goto graph_update;
   }

if (menunum==2 && itemnum==3) {  /** ramp down **/
   if (marked_samp==NULL) {
      show_msg(edit_win, 130);
      goto ignore;
      }
   show_msg(edit_win,112);
   ramp_dn(marked_samp, start_mark, end_mark, LEFT);
   if (marked_samp->type==STEREO)
      ramp_dn(marked_samp, start_mark, end_mark, RIGHT);
   msg(edit_win,"");
   goto graph_update;
   }


if (menunum==2 && itemnum==4) {  /** scale **/
   if (marked_samp==NULL) {
      show_msg(edit_win, 130);
      goto ignore;
      }
   i=getint(edit_win,100,1,300, get_msg_ptr(239),350);
   show_msg(edit_win,112);
   scale(marked_samp, start_mark, end_mark, i, LEFT);
   if (marked_samp->type==STEREO)
      scale(marked_samp, start_mark, end_mark, i, RIGHT);
   msg(edit_win,"");
   goto graph_update;
   }

/** Mix (5) **/
if (menunum==2 && itemnum==5) {
   if (marked_samp==NULL) {
      show_msg(edit_win, 133);
      goto ignore;
      }

   if (!inserting) {
      show_msg(edit_win, 134);
      inserting=TRUE;
      goto ignore;
      }

   if (insert_samp==NULL) {
      show_msg(edit_win, 135);
      goto ignore;
      }

   inserting=FALSE;
   show_msg(edit_win, 112);
   mix_samples(insert_samp, marked_samp);
   show_msg(edit_win, 120);
   goto graph_update;
   }

/** Zoom Range (6) **/
if (menunum==2 && itemnum==6) {
   graph(graph_mode|GRAPH_ZOOM);
   show_msg(edit_win,137);
   while (class!=MOUSEBUTTONS) {
      while ((message=(struct IntuiMessage *)GetMsg(edit_win->UserPort))==0)
         Wait(1<<edit_win->UserPort->mp_SigBit);
      class=message->Class;
      ReplyMsg(message);
      }
   msg(edit_win,"");
   goto graph_update;
   }

if (menunum==2 && itemnum==7) {  /** Mark entire sample **/
   if (cur==NULL) {
      start_mark = 0;
      end_mark = cur->length-1;
      marked_samp = cur;
      goto sub_graph_update;
      }
   }

/** Low pass filter (7) **/

if (menunum==2 && itemnum==8) {
   if (cur==NULL) goto ignore;
   i=getint(edit_win,4000,1000,10000, get_msg_ptr(240),350);
   show_msg(edit_win,112);
   lp_filter(cur, cur->length, cur->rate, i);
   msg(edit_win,"");
   goto graph_update;
   }

if (menunum==2 && itemnum==9) {
whole:         /* jump here from delete range if entire sample */
   delete_sample(cur);
   insert_samp = NULL;
   goto graph_update;
   }

/* create stereo from two mono samples */

if (menunum==2 && itemnum==10) {
#if 0    /*!!!!!!!!!!!!!!!!!!!!!!!! **************/
   if (cur==NULL) goto ignore;
   show_msg(edit_win,138);
   if ((i=pickslot(edit_win))==-1) goto ignore;
   if (samp[i].type==STEREO) {
      show_msg(edit_win,139);
      goto ignore;
      }
   show_msg(edit_win,140);
   if ((k=pickslot(edit_win))==-1) goto ignore;
   if (samp[k].type==STEREO) {
      show_msg(edit_win,139);
      goto ignore;
      }
   if (k==i) {
      show_msg(edit_win,142);
      goto ignore;
      }
   if (samp[i].length<samp[k].length) {ss=i;sl=k;}
   else {ss=k;sl=i;}

   samp[ss].rfp=samp[sl].lfp;
   samp[ss].type=STEREO;
   samp[sl].flags = 0;
   cur=ss;
   samp[sl].flags = 0;
   samp[sl].length=0;
   FreeMem(samp[sl].name,strlen(samp[sl].name)+1);
   samp[sl].name=0;
   show_msg(edit_win,143);
   if (marked_samp==i || marked_samp==k) {
      marked_samp = NULL;
      insert_samp = NULL;
      }
   goto graph_update;
#endif
   }

/* Break up a stereo into two new slots */

if (menunum==2 && itemnum==11) {
#if 0    /******@!!!!!!!!!@@@@@@@@@@ ******/
   if (cur==NULL) goto ignore;
   if (cur->type!=STEREO) {
      show_msg(edit_win,144);
      goto ignore;
      }
   if ((i=getslot(edit_win))==-1) goto ignore;
   samp[i].name=AllocMem(strlen(cur->name)+2,0);
   if (samp[i].name==0) {
      nomem(edit_win);
      goto ignore;
      }
   *samp[i].name='L';
   strcpy(samp[i].name+1,cur->name);
   samp[i].lfp=cur->rfp;
   samp[i].type=LEFT;
   samp[i].length=cur->length;
   samp[i].rate=cur->rate;
   samp[i].flags=SAMPLE_OPEN;
   x=AllocMem(strlen(cur->name)+2,0);
   if (x==0) {
      nomem(edit_win);
      FreeMem(samp[i].name,strlen(samp[i].name+1));
      samp[i].length=0;
      samp[i].flags=0;
      goto ignore;
      }
   *x='R';
   strcpy(x+1,cur->name);
   FreeMem(cur->name,strlen(cur->name)+1);
   cur->name=x;
   cur->type=RIGHT;
   show_msg(edit_win,145);
   if (marked_samp==cur) {
      marked_samp = NULL;
      insert_samp = NULL;
      }
   goto graph_update;
#endif
   }

if (menunum==2 && itemnum==12) {       /** Resample to new sampling rate **/
   if (cur==NULL) goto ignore;
   sprintf(line,"%s-2",cur->name);
   i = create_sample(line, 0, &s);
   if (i!=NO_ERROR) {
      show_error(i);
      goto ignore;
      }
   s->type=cur->type;
   s->rate=getint(edit_win,cur->rate,4000,40000, get_msg_ptr(241),350);
   s->length=even((int)((float)s->rate*(float)cur->length/(float)cur->rate));
   show_msg(edit_win, 112);
   for (j=0; j < s->length; j++) {
      flt = (float)cur->length*(float)j;
      k = (int)(flt/(float)s->length);
      put_samplel1(get_left_sample2(cur,k));
      }
   if (cur->type==STEREO) {
      init_sample1(s,0);
      for (j=0; j < s->length; j++) {
         flt = (float)cur->length*(float)j;
         k = (int)(flt/(float)s->length);
         put_sampler1(get_right_sample2(cur,k));
         }
      }
   cur=s;
   show_msg(edit_win,147);
   goto graph_update;
   }


if (menunum==3 && itemnum==0) {    /* change default recording speed */
   }

if (menunum==3 && itemnum==1) {  /* View Signal Levels */
   }
   
if (menunum==3 && itemnum==2) {            /* monitor the digitizer */
   }

if (menunum==3 && itemnum==3)  {  /* record */
   }

if (menunum==4 && itemnum==0) {   /** filter on or off **/
   }

if (menunum==4 && itemnum==1) {   /** turn demos off **/
   }

if (menunum==4 && itemnum==2) {   /** turn demos on **/
   }

if (menunum==4 && itemnum==3) {   /** real time echo **/
   }

if (menunum==4 && itemnum==4) {   /** real time delay **/
   }

   goto update;

/**** everything comes here when done ****/

graph_update:

refresh_edit_win();

sub_graph_update:
update:

memory=frags();

sprintf(line, get_msg_ptr(204), cur->length);
pad_str(line);
putxy(edit_win,STAT_X,STAT_Y,line,WHITE,BLACK);

sprintf(line,get_msg_ptr(205), cur->rate);
pad_str(line);
putxy(edit_win,STAT_X,STAT_Y+9,line,WHITE,BLACK);


sprintf(line,get_msg_ptr(206),memory/1024);
pad_str(line);
putxy(edit_win,STAT_X,STAT_Y+45,line,WHITE,BLACK);

if (cur->type==RIGHT) {
   putxy(edit_win,STAT_X+176,STAT_Y+45,get_msg_ptr(222),WHITE,BLACK);
   }
else if (cur->type==LEFT) {
   putxy(edit_win,STAT_X+176,STAT_Y+45,get_msg_ptr(223),WHITE,BLACK);
   }
else {
   putxy(edit_win,STAT_X+176,STAT_Y+45,get_msg_ptr(224),WHITE,BLACK);
   }
if (cur==NULL) {
   putxy(edit_win,STAT_X+176,STAT_Y+45,"        ",WHITE,BLACK);
   }

update_se_mark(edit_win, line);

ignore:       /** here when ready for new input event **/
return;
}

/*****************************************************/

void close_edit(m)
struct Module *m;
{
ClearMenuStrip(m->window);
CloseWindow(m->window);
m->window=0;
filter_on();      /** just in case user has filter turned off **/
mproc = (struct Process *)FindTask(0L);
mproc->pr_WindowPtr = temp_ptr;        /** restore original pointer **/
show_play=FALSE;        /** show line while playing **/
return;
}

/*****************************************************/


/** given the pointer position, update the global variable "marker" **/

int start_dragging(mousex)

int mousex;
{
int marker;    /** sample where dragging starts **/

SetDrMd(edit_win->RPort, COMPLEMENT);

if (mark_drawn)
   RectFill(edit_win->RPort, old_x1, GRAPH_Y, old_x2, GRAPH_Y+graph_height-1);

mark_drawn=TRUE;    /** erase old marked range **/
old_x1 = old_x2 = mousex;

marker = ((mousex - GRAPH_X)*cur->length) + (graph_width/2); /* .5 round */
marker = even(marker / graph_width);
Move (edit_win->RPort, mousex, GRAPH_Y);
Draw (edit_win->RPort, mousex, GRAPH_Y+graph_height-1);
return(marker);
}

/*****************************************************/

void draw_marked(Window)

struct Window *Window;

{
int x1,x2;
int temp;
extern int old_x1, old_x2;

SetDrMd(edit_win->RPort, COMPLEMENT);
if (inserting) {
   x1 = GRAPH_X + (insert_mark*graph_width)/cur->length;
   x2 = GRAPH_X + (insert_mark*graph_width)/cur->length;
   }
else {
   x1 = GRAPH_X + (start_mark*graph_width)/cur->length;
   x2 = GRAPH_X + (end_mark*graph_width)/cur->length;
   }

if (x2 < x1) {    /** swap **/
   temp = x1;
   x1 = x2;
   x2 = temp;
   }

#if 0 /***** NEED TO RE-DO LOOPING!!!! ***/
if (looping) {
   if (end_mark < start_mark) {
      if (start_mark - end_mark >= 131072) {
         show_msg(Window, 251);
         stop_looping();
         }
      else {
         new_loop_section(0,cur->lmem+end_mark-1,start_mark-end_mark+1);
         }
      }
   else {
      if (end_mark-start_mark >= 131072) {
         show_msg(Window, 251);
         stop_looping();
         }
      else {
         new_loop_section(0,cur->lmem+start_mark,end_mark-start_mark+1);
         }
      }
   }
#endif

if (old_x1==x1 && old_x2==x2 && mark_drawn)
   return;

if (mark_drawn) {
   if (x1 < old_x1)
      RectFill(edit_win->RPort, x1, GRAPH_Y, old_x1-1, GRAPH_Y+graph_height-1);
   if (x1 > old_x1)
      RectFill(edit_win->RPort, old_x1, GRAPH_Y, x1-1, GRAPH_Y+graph_height-1);
   if (x2 > old_x2)
      RectFill(edit_win->RPort, old_x2+1, GRAPH_Y, x2, GRAPH_Y+graph_height-1);
   if (x2 < old_x2)
      RectFill(edit_win->RPort, x2+1, GRAPH_Y, old_x2, GRAPH_Y+graph_height-1);
   }
/*    RectFill(edit_win->RPort, old_x1, GRAPH_Y, old_x2, GRAPH_Y+graph_height-1); */
else {
   mark_drawn=TRUE;
   RectFill(edit_win->RPort, x1, GRAPH_Y, x2, GRAPH_Y+graph_height-1);
   }

/* RectFill(edit_win->RPort, x1, GRAPH_Y, x2, GRAPH_Y+graph_height-1); */
old_x1 = x1;
old_x2 = x2;
}

/*****************************************************/

int in_graph(x, y)

int x, y;

{
return (x>GRAPH_X && x<(GRAPH_X+graph_width) && y>GRAPH_Y && y<(GRAPH_Y+graph_height));
}

/*****************************************************/

#if 0
int end_up(x, y)

int x, y;

{
return (x>ENDUP_X && x<(ENDUP_X+ENDUP_W) && y>ENDUP_Y && y<(ENDUP_Y+ENDUP_H));
}

/*****************************************************/

int end_dn(x, y)

int x, y;

{
return (x>ENDDN_X && x<(ENDDN_X+ENDDN_W) && y>ENDDN_Y && y<(ENDDN_Y+ENDDN_H));
}

/*****************************************************/

int start_up(x, y)

int x, y;

{
return (x>STARTUP_X && x<(STARTUP_X+STARTUP_W) && y>STARTUP_Y && y<(STARTUP_Y+STARTUP_H));
}

/*****************************************************/

int start_dn(x, y)

int x, y;

{
return (x>STARTDN_X && x<(STARTDN_X+STARTDN_W) && y>STARTDN_Y && y<(STARTDN_Y+STARTDN_H));
}
#endif

/*****************************************************/

int set_new_em(mousex)
int mousex;
{
int marker;

marker = ((mousex - GRAPH_X)*cur->length)/graph_width;
marker = odd(marker);
if (marker >= cur->length)
   marker = cur->length-1;
if (marker < 0)
   marker = 1;
return (marker);
}

/*****************************************************/

closeto(mousex, mark)
int mousex, mark;
{
int i;

i = mousex - GRAPH_X - mark*graph_width/cur->length;
if (i < 0)
   i = -i;
if (i < 10)
   return (TRUE);
else
   return(FALSE);
}

/*****************************************************/

#if 0
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
#endif

/*****************************************************/

void update_se_mark(Window, line)
struct Window *Window;
char *line;
{

if (marked_samp==NULL)
   sprintf(line,get_msg_ptr(212));
else
   sprintf(line,get_msg_ptr(213), start_mark);
putxy(Window,STAT_X,STAT_Y+18,line,WHITE,BLACK);

if (marked_samp==NULL)
   sprintf(line,get_msg_ptr(214));
else
   sprintf(line,get_msg_ptr(215), end_mark);
putxy(Window,STAT_X,STAT_Y+27,line,WHITE,BLACK);

if (insert_samp==NULL)
   sprintf(line,get_msg_ptr(216));
else
   sprintf(line,get_msg_ptr(217), insert_mark);
putxy(Window,STAT_X,STAT_Y+36,line,WHITE,BLACK);
}

/*****************************************************/

/** scale samples in a ramp fashion **/

void ramp_dn(p, s, e, chan)
struct Disk_Samp *p;
int s,e,chan;
{
int len;
int k;
int v;

len = e - s;
init_sample1(p,s);
init_sample2(p,s);

for (k=s; k<=e; k++) {
   if (chan==LEFT)  v = get_samplel2(); else v = get_sampler2();
   v = (v * (e - k))/len;
   if (chan==LEFT) put_samplel1(v); else put_sampler1(v);
   }
}

/*****************************************************/

/** scale samples in a ramp fashion **/

void ramp_up(p, s, e, chan)
struct Disk_Samp *p;
int s,e,chan;
{
int len;
int k;
int v;

len = e - s;

init_sample1(p,s);
init_sample2(p,s);

for (k=s; k<=e; k++) {
   if (chan==LEFT)  v = get_samplel2(); else v = get_sampler2();
   v = (v * (k-s))/len;
   if (chan==LEFT) put_samplel1(v); else put_sampler1(v);
   }
}

/*****************************************************/

void scale(p, s, e, per, chan)
char *p;
int s,e,per;
{
int k;
int v;

init_sample1(p,s);
init_sample2(p,s);

for (k=s; k<=e; k++) {
   if (chan==LEFT)  v = get_samplel2(); else v = get_sampler2();
   v = (v * per)/100;
   if (v > 32767) v = 32767;
   if (v < -32767) v = -32767;
   if (chan==LEFT) put_samplel1(v); else put_sampler1(v);
   }
}

/*****************************************************/

void pad_str(line)

char *line;

{
strncat(line,"            ",30-strlen(line));
}

void stop_looping()
{
/*
IOB2.ioa_Request.io_Unit=(struct Unit *)1;
IOB2.ioa_Request.io_Command=CMD_RESET;
DoIO (&IOB2); */
looping = FALSE;
}
