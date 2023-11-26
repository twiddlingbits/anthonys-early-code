#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/text.h"
#include "graphics/gfx.h"
#include "graphics/rastport.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
#include "/texted/texted.h"


COORD columnwidth;
COORD x_gridsize;
COORD y_gridsize;
COORD xoutlinespc;  /* space page outline is offseted from margins **/
COORD youtlinespc;  /* space page outline is offseted from margins **/
SHORT snaptogrid=FALSE;
SHORT displaygrid=FALSE;
SHORT displayguides=TRUE;
SHORT texterrors=TRUE;        /* TRUE means display text errors */
SHORT screensize;             /* actual screen height */
SHORT strip_spaces=TRUE;      /* strip multiple space on load */
SHORT double_newlines=FALSE;  /* 2 CR marks para end, else use 1 CR **/
SHORT dot_commands=FALSE;     /* interpret scribble dot commands **/
SHORT scribble_style=FALSE;   /* interpret scribble embeded commands */
SHORT wp_style=FALSE;         /* Input Text is from word perfect. */
SHORT snaptotop=FALSE;        /* start flowing at top */
SHORT autoflow=FALSE;         /* autoflow to next column */
SHORT ansi_escapes=TRUE;      /* Notepage style text load */
SHORT real_ascii=FALSE;       /* filter nonpritable ascii */
SHORT flow_warnings=TRUE;     /* Warn when not finished flowing */
SHORT usedefault = TRUE;      /* Reflow uses default, not same */
SHORT time_for_same = FALSE;  /* Set when we want flow to use same */
SHORT do_trans = TRUE;        /* turns on and off translation */
SHORT std_height;             /* non interlaced screen height */
SHORT std_width;              /* screen width, typicly 640 */
SHORT greeking = FALSE;       /* fake output text. */
SHORT pointer_mode = PTR_NORMAL;
SHORT autohyphenate = FALSE;  /* Hyphenate words while flowing? */
SHORT default_mag = 1;        /* default magnification for new pages */
SHORT irregular = FALSE;      /* irregular flow around graphics? */
UBYTE *translate;             /* points to translation table */
struct object *last_picked=0;   /* point to last object picked when linking */
char default_font_name[40];   /* Just what it says. */
char default_cmd_list[256]=""; /* embeded cmds to execute before each flow */

struct pageinfo   page[MAXPAGES];
struct text_state defaultstate;
struct text_state samestate;     /* set by reflowchain, sometimes */
struct pageinfo   defaultpage;
struct rule       defaultrule={0xFFFF,8,0};
struct object     default_object;
struct Window     *clipboard;
struct laserfont  *firstlaserfont;
struct printerprefs printerprefs;
struct Window *canwin;   /** window used to display cancel msg on print **/
struct Window *coord_window;   /** used in displaying mouse coordinates **/
struct Screen *screen;         /** City Desk main screen **/
APTR  temp_ptr;                /** CB's old pr_WindowPtr to be restored **/

COORD xshadowsize;       /** size in coord's of shadow **/
COORD yshadowsize;       /** size in coord's of shadow **/
ULONG xpix;
ULONG ypix;
UWORD xinch,yinch;
FIXED cpertextpixx,cpertextpixy;
char printername[60];
char *pname;

void main(argc,argv)
int argc;
char *argv[];
{
extern struct Menu Titles[];
void settypeflag();
char *text,*text2,*texttoflow;
char buffer[81];
ULONG class;
USHORT code;
SHORT mousex,mousey;
SHORT menunum,itemnum,subnum;
SHORT openworkbench = FALSE;
COORD x,y;
int pg;      /* current page being displayed */
short lastpg;
int i;
SHORT j,toggle=0;
SHORT type;
SHORT xoff,yoff;
SHORT mode=0,alttool,oldmode;
LONG hpot,vpot;
LONG oldmag,newmag;
SHORT nice=FALSE;   /** graphic resize nice or anyway? **/
SHORT sizedir=0;   /** resize direction Xonly , Yonly, or ANYWAY **/
LONG len;
SHORT loadautoprefs; /** TRUE if autoprefs file was present on load **/
int status;
COORD box_x,box_y,box_h,box_w;

struct object *prevobj();
struct pageview *view;
struct PropInfo *pi;
struct MenuItem *ItemAddress();
struct IntuiMessage *message,*GetMsg();
struct Window *toolbox,*opentoolbox(),*thiswindow;
struct graphic *graphic;
struct object *obj,*objcopy,*flowobj,*obj_a;
struct object *flowtext(),*reflowchain();
struct object tempobj;
struct text_object *to;
struct object *createobject(),*matchrange();
struct viewmsg msg;
struct Node *node;
struct click *click,*getclick();
struct macro_text *mt;

openlibraries();

texttoflow=NULL;     /** get rid of compiler warning **/
flowobj=NULL;        /** get rid of compiler warning **/
coord_window = 0;    /** coordinate display not active **/

canwin=0;          /** So calls to checkcan() know we are not printing ***/
firstlaserfont=0;  /** No laserfonts loaded yet **/
pg=1;   /* starting page number */
lastpg = 1;
default_object.dpi = 300;
default_object.data = (APTR)&defaultrule;
default_object.type = RULE;
default_object.density = 0;

/** initilize all pages **/

for (i=0;i<MAXPAGES;i++) {
   NewList(&page[i].objects.lh_Head);
   page[i].view=NULL;
   page[i].pn=i;              /* set page number */
   page[i].flags = 0;
   }

ghost_save();          /** ghost save.. option in project menu **/
init_fonts();          /** initialize the available fonts chain **/

/** try and load autoprefs & loadfonts **/

screensize = get_screen_data();

if (loadprefs("AUTOPREFS",TRUE)==AOK) {
   loadautoprefs=TRUE;
   }

else {
   loadautoprefs=FALSE;
   }

if (argc > 1) {      /**!!!!!!! take this out?? **/
   std_height = 800;
   std_width = 1008;
   screensize = std_height;
   }

if (screensize >= 800 && AvailMem(MEMF_CHIP) < 400000)
   openworkbench = CloseWorkBench();

screen=(struct Screen *)openscreen(std_width,screensize,2);
if (screen==0) {
   telluser2(0,"Can't open screen!","",FALSE);
   close_fonts();
   closelibraries();
   exit(10);
   }


/** set up clipboard **/

if (createview(screen,&page[CB])!=AOK) {
   CloseScreen(screen);
   telluser2(0,"Can't open clipboard!","",FALSE);
   close_fonts();
   closelibraries();
   exit(10);
   }

msg.page=&page[CB];
msg.command=REFRESH;
viewmonitor(&msg);

/***** open the tool box ****/

toolbox=opentoolbox(screen,page[CB].view->window->UserPort);

newpointer(screen, BUSY);

if (!loadautoprefs) {  /** if no autoprefs, make smart printer choice **/

   printerprefs.printertype=PREFERENCES;
   if (loadfonts(screen)!=AOK) {  /** find what preferences printer **/
      newpointer(screen, NORMAL);
      CloseScreen(screen);
      close_fonts();
      closelibraries();
      exit(10);
      }

   initdefaultstate();     /** initilize default text state **/

   if (printername[0]=='H' && printername[1]=='P' && printername[2]=='_') {
      printerprefs.printertype=HPLASER;
      strcpy(default_font_name, "courier");
      }

   if (loadfonts(screen)!=AOK) {
      CloseScreen(screen);
      close_fonts();
      closelibraries();
      exit(10);
      }
   setdeffont(screen);  /** sets font ptr in default state from ***/
                        /** name in default_font_name **/
   settypeflag();          /** set xinch, yinch **/
   initdefaultstate2();    /** reset stuff dependent on xinch, yinch **/
   initdefaultpage();      /** initilize old default page structure **/

   copypage(&defaultpage,&page[DEFAULTEVEN]);
   copypage(&defaultpage,&page[DEFAULTODD]);

   x_gridsize = xinch/2;
   y_gridsize = yinch/2;
   }

settypeflag();          /** check submenu and set cpertextpix **/
initdefaultpage();      /** initilize old default page structure **/
copypage(&defaultpage,&page[CUTPAGE]);
copypage(&defaultpage,&page[CB]);   /* Clip Board opened when we start */

if (printerprefs.printertype==HPLASER)
   hp_font_setup("autofonts");      /** try and download hp fonts **/

/**** set up this page ****/

check_page(1); /** use default page **/

if (createview(screen,&page[pg])!=AOK) {
   CloseScreen(screen);
   telluser2(0,"Can't open page1","",FALSE);
   close_fonts();
   closelibraries();
   exit(10);
   }

newpointer(screen, BUSY);

if (autohyphenate) {
   if (load_patterns("hyphen.tex")!=AOK)
      autohyphenate=FALSE;
   }

load_macros("automacros"); /** load macros if they are there **/

msg.page=&page[pg];
msg.command=REFRESH;
viewmonitor(&msg);

setonoff();             /** set preferences submenu checkmarks **/
setmenus(screen, TRUE);  /* turn on menus */
xshadowsize = SHADOWSIZE*xinch/576;
yshadowsize = SHADOWSIZE*yinch/576;
xoutlinespc = 75*xinch/576;
youtlinespc = 75*yinch/576;
newpointer(screen, NORMAL);

/* printf("x y = %x %x\n",cpertextpixx,cpertextpixy); */

/*************************************/
/** main loop, wait for input event **/
/*************************************/

toploop:
/* setmenus(screen,TRUE);  */ /* turn on menus */

/* do { */
   while((message=(struct IntuiMessage *)GetMsg(clipboard->UserPort))==0) {
      Wait(1<<clipboard->UserPort->mp_SigBit);
      }

   class=message->Class;
   code=message->Code;
   thiswindow=message->IDCMPWindow;
   mousex=message->MouseX;
   mousey=message->MouseY;
   ReplyMsg(message);

/* printf("class %x code %x pg %d w %x\n",class,code,pg,thiswindow); */

   if (thiswindow!=page[pg].view->window) {   /* determine curent pæge */
      i=whatpage(thiswindow);
      if (i!=-1) pg=i;
      if (pg!=0 && pg < CUTPAGE ) lastpg = pg;
      if (class==ACTIVEWINDOW) goto toploop;
      } 


/*   } while (&clipboard->UserPort->mp_MsgList !=
      (struct List *)clipboard->UserPort->mp_MsgList.lh_TailPred); */


/* if (thiswindow->Flags&GIMMEZEROZERO)  {
   mousex -= thiswindow->BorderLeft;
   mousey -= thiswindow->BorderTop;
   }  */

/* setmenus(screen,FALSE);  now in newmode() */ /*  turn off menus */
oldmode=mode;     /** remember original mode **/


/*******************************/
/*** prop gadget relesed *******/
/*******************************/

if (class==GADGETUP) {
   hpot=page[pg].view->prl.HorizPot;
   vpot=page[pg].view->pud.VertPot;

    /** note change the shifts below to alter the resolution **/

 /*   page[pg].view->x=(hpot*((LONG)page[pg].pw*(LONG)page[pg].view->mag-
         (LONG)page[pg].view->w + (LONG)(1<<15) )) >>16;
   page[pg].view->y=(vpot*((LONG)page[pg].ph*(LONG)page[pg].view->mag-
         (LONG)page[pg].view->h + (LONG)(1<<15) )) >>16; */

   page[pg].view->x=((hpot>>4)*(ULONG)(page[pg].pw*page[pg].view->mag-page[pg].view->w))>>12;
   page[pg].view->y=((vpot>>4)*(ULONG)(page[pg].ph*page[pg].view->mag-page[pg].view->h))>>12;

   newpointer(screen, BUSY);
   msg.page=&page[pg];
   msg.command=REFRESH;
   viewmonitor(&msg);
   newpointer(screen, mode);
   goto toploop;
   }

if (class==MENUPICK && code==MENUNULL) {
   alttool=picktool(toolbox->MouseX,toolbox->MouseY);

   if (alttool==NO_TOOL_PICKED)
      if ((obj=matchrange(&page[pg].objects,
          mousetocx(&page[pg],mousex),mousetocy(&page[pg],mousey)))!=0) {

      Remove(&obj->node);
      AddTail(&page[pg].objects,obj);
      goto toploop;
      }

   if (alttool==EXPLODE) {    /* reduce window to size before expand */
      if (pg==CB)
         goto toploop;
      restore_window(pg);
      newpointer(screen, mode);
      Delay(10);
      WindowToFront(toolbox);
      goto newsize;
      }

   if (alttool==EDITOBJ) {
      objoptions(screen,&default_object, TRUE);
      goto toploop;
      }

   if (alttool==NORMAL) {
      pointer_options(screen);
      goto toploop;
      }

   if (alttool==SIZE) {
      sizemode(screen,&sizedir,&nice);
      mode = SIZE;
      newpointer(screen,mode);
      goto toploop;
      }

   if (alttool==REFLOWTEXT) {
      flow_options(screen);
      if (autohyphenate) {
         newpointer(screen, BUSY);
         if (load_patterns("hyphen.tex")!=AOK)
            autohyphenate=FALSE;
         newpointer(screen, mode);
         }
      else {
         free_patterns();
         }
      goto toploop;
      }

   if (alttool==EDITFORMAT) {
      newtextformat(screen,&defaultstate);
      if (defaultstate.lfont)
         strcpy(default_font_name, defaultstate.lfont->fontname);
      else
         strcpy(default_font_name,defaultstate.font->tf_Message.mn_Node.ln_Name);
      goto toploop;
      }

   goto toploop;
   }

/*** user selects an object ***/

if (class==MOUSEBUTTONS && code==SELECTDOWN) {

/*
get_bounds(pg, mousetocx(&page[pg],mousex), mousetocy(&page[pg],mousey),
          colwid(pg)-page[pg].cg, &x_left, &x_right);
printf("x %d y %d w %d xl %d xr %d\n",
        mousex, mousey, colwid(pg)-page[pg].cg, x_left, x_right); */


   if (thiswindow==toolbox) {
      i=picktool(mousex,mousey);

      if (i==EXPLODE) {    /** zap window to fill screen **/
         if (pg==CB)
            goto toploop;
         expand_window(pg);
         newpointer(screen, mode);
         Delay(10);
         WindowToFront(toolbox);
         goto newsize;
         }

      if (mode==FLOW || mode==BOX_FLOW) {
         if (flowobj==NULL && !(i==NEWPAGE || i==MAGNIFY)) {
#if GERMAN
            j=telluser(screen,"OK wählen, Text auszufügen","");
#else
            j=telluser(screen,"Press OK to Discard Text.","");
#endif
            if (j) {
               FreeMem(texttoflow,strlen(texttoflow)+1);
               mode = NORMAL;
               }
            else {
               goto toploop;
               }
            }

         if (flowobj!=NULL && flow_warnings) {
#if GERMAN
            telluser2(screen,"Warnung: Sie haben den Text noch",
             "nicht voll plaziert.",FALSE);
#else
            telluser2(screen,"Warning: You have not finished",
             "placing the text.",FALSE);
#endif
            }
         }

      mode = i;
      last_picked = 0;

      switch (mode) {

         case MAGNIFY:

         mode=oldmode;
         if (pg==CB) {
#if GERMAN
            telluser(screen,"Sie müssen zuerst eine seite wählen,",
                  "indem Sie in deren Fenster Klicken.");
#else
            telluser(screen,"You must first select a page",
                     "by clicking its window");
#endif
            goto toploop;
            }
         oldmag=page[pg].view->mag;   /* save for a few lines down */
         newpointer(screen,NORMAL);
         newmag = get_new_mag(screen, oldmag);
         newpointer(screen,mode);
         page[pg].view->mag=newmag;
         if (oldmag==page[pg].view->mag) goto toploop;

   /** if lower mag, make window smaller **/

         if (page[pg].view->window->Width >
              ctosx(page[pg].pw*newmag)+XBORDER_AREA) {
            xoff=ctosx(page[pg].pw*newmag)+XBORDER - page[pg].view->window->Width;
            page[pg].view->w=stocx(page[pg].pw*newmag);
            SizeWindow(page[pg].view->window,xoff,0);
            page[pg].view->x=0;
            }
         else {

            page[pg].view->x = (page[pg].view->x/oldmag)*newmag;
            page[pg].view->x =
               min(page[pg].pw*newmag - page[pg].view->w , page[pg].view->x);
            }

         if (page[pg].view->window->Height >
              ctosy(page[pg].ph*newmag)+YBORDER_AREA) {
            yoff=ctosy(page[pg].ph*newmag)+YBORDER_AREA- page[pg].view->window->Height;
            page[pg].view->h=stocy(page[pg].ph*newmag);
            SizeWindow(page[pg].view->window,0,yoff);
            page[pg].view->y=0;
            }
         else {

            page[pg].view->y = (page[pg].view->y/oldmag)*newmag;
            page[pg].view->y =
               min(page[pg].ph*newmag - page[pg].view->h , page[pg].view->y);
            }

   /* adjust page size max's for this magnification */
   /* I don't want the user sizing his page bigger than it is */

         page[pg].view->window->MaxWidth=
            min(std_width,XBORDER_AREA+ctosx(page[pg].pw*newmag));
         page[pg].view->window->MaxHeight=
            min(screensize,YBORDER_AREA+ctosy(page[pg].ph*newmag));
         Delay(10);   /* wait 10/50 of second */
         goto newsize;

         case ABCEDIT:        /** enter one line of text **/

         setmenus(screen, FALSE);    /** Turn off menus **/
         text2 = (char *)texted (screen,0,0,0,50,640,60,&status);
         setmenus(screen, TRUE);

         if (text2==NULL || status==JUSTQUIT) {
            mode=NORMAL;
            newpointer(screen,mode);
            goto toploop;
            }

         saveloadstat();   /** save strip_space, paraspace, etc. **/
         setansi();
         strip_spaces=FALSE;
         i=textconv(text2,strlen(text2),&texttoflow,&i);
         loadloadstat();
         showstat(screen,i);
         if (i!=AOK) {
            mode = NORMAL;
            }
         else {
            mode=FLOW;        /** flow text into page **/
            flowobj=NULL;     /** this is the first object **/
            }
         newpointer(screen,mode);
         goto toploop;


         case NEWPAGE:     /* user wants to edit another page */

         i=pg;   /* remember in case of bad page */
         newpointer(screen,NORMAL);
         if (page[lastpg].flags&PAGEINUSE)
            pg=open_page(screen,lastpg+1,&j);
         else
            pg=open_page(screen,lastpg,&j);  /** user erased page **/

         mode=oldmode;
         newpointer(screen,mode);
         if (pg==0) {
            pg = i;
            goto toploop;
            }
         if (pg <1 || pg >= MAXPAGES) {
            pg=i;
#if GERMAN
            telluser2("Tut mir Leid, Ich kann diese Seite nicht öffnen.",
                     "",FALSE);
#else
            telluser2(screen,"Sorry, I can't open that page.","",FALSE);
#endif
            goto toploop;
            }
         if (page[pg].view!=0) {   /* can't have two of same page open */
            WindowToFront(page[pg].view->window);
            pg=i;
            goto toploop;
            }

         if (j)
            close_all_views();

         check_page(pg);   /** use default page if unopened **/
         setmenus(screen, FALSE);  /* turn off menus */

         if (createview(screen,&page[pg])!=AOK) {
            showstat(screen,OUTOFMEM);
            pg=i;
            setmenus(screen, TRUE);  /* turn on menus */
            goto toploop;
            }

         setmenus(screen, TRUE);  /* turn on menus */

   /*** create bitmap data for all objects on page ***/

         newpointer(screen,BUSY);
         for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
            obj=(struct object *)node;
            makedisplay(obj);
            }


         msg.page=&page[pg];
         msg.command=REFRESH;
         viewmonitor(&msg);
         newpointer(screen,mode);
         goto toploop;

      /**** here if a tool picked but not handeled above. This will
            result in a pointer change to indicate a future action
            will occure. (such as resize) ****/

         default:
            newpointer(screen,mode);
            goto toploop;
         }   /*** close switch ***/
      }      /*** close if (toolbox==thiswindow) .... ***/

/*** The following code handles the case where the user has already 
     picked a tool and is now trying to use it. ***/

   switch(mode) {

      case DOBLOCK:

         showstat(screen,get_block(pg,thiswindow,mousex,mousey,0));
         newpointer(screen, mode);
         goto toploop;

      case LINE:

         showstat(screen,getline(pg,thiswindow,mousex,mousey,0));
         newpointer(screen, mode);
         goto toploop;

      case BOX_FLOW:
         get_box(pg,thiswindow,mousex,mousey,&box_x,&box_y,&box_h,&box_w);
         if (box_h==0)  /** aborted **/
            goto toploop;
         else
            goto flow_into_box;

      case FLOW:

         box_x=0;
         box_y=0;
         box_w=0;
         box_h=0;
flow_into_box:
         setmenus(screen, FALSE);    /** Turn off menus **/
         flowobj=flowtext(pg,thiswindow,mousex,mousey,
            flowobj,texttoflow,&mode,box_x,box_y,box_h,box_w);
         if (!(flowobj==NULL && (mode==FLOW||mode==BOX_FLOW)))  /** This swich should be **/
            time_for_same = FALSE;            /** off after first flow **/
         newpointer(screen,mode);
         setmenus(screen, TRUE);    /** Turn Menus back on **/
         goto toploop;
         }   /* end of switch */

   i = snaptogrid;   /** make sure we don't snap to grid... */
   snaptogrid = FALSE;
   obj=matchrange(&page[pg].objects, mousetocx(&page[pg],mousex),
       mousetocy(&page[pg],mousey));
   snaptogrid = i;

   if (obj!=0) {
   switch (mode) {

      case LINK:     /** link objects into a group **/
         showstat(screen,add_link(obj));
         goto toploop;

      case UNLINKALL:     /** link objects into a group **/
         showstat(screen,unlink_all(obj));
         goto toploop;

      case PICKTEXTB:
         if (obj->type!=TEXT) {
#if GERMAN
            telluser2(screen,"Sie müssen ein Text-Objekt wählen.","",FALSE);
#else
            telluser2(screen,"You Must Pick A","Text Object.",FALSE);
#endif
            goto toploop;
            }
         newpointer(screen,BUSY);
         showstat(screen,connect_chain(obj_a,obj));
         mode = NORMAL;
         newpointer(screen,mode);
         goto toploop;


      case PICKTEXTA:
         if (obj->type!=TEXT) {
#if GERMAN
            telluser2(screen,"Sie müssen ein Text-Objekt wählen.","",FALSE);
#else
            telluser2(screen,"You Must Pick A","Text Object.",FALSE);
#endif
            goto toploop;
            }
         obj_a = obj;
         mode = PICKTEXTB;
         newpointer(screen,mode);
         goto toploop;

      case BREAKCHAIN:
         if (verifypick(obj)!=AOK)
            goto toploop;

         if (obj->type!=TEXT) {
#if GERMAN
            telluser2(screen,"Sie müssen ein Text-Object wählen.","",FALSE);
#else
            telluser2(screen,"You Must Pick A","Text Object.",FALSE);
#endif
            goto toploop;
            }
         newpointer(screen,BUSY);
         showstat(screen,break_chain(obj));
         mode = NORMAL;
         newpointer(screen,mode);
         goto toploop;

      case TRASHCAN:
         if (verifypick(obj)!=AOK)
            goto toploop;
#if GERMAN
         if (!telluser(screen,"Im Begriffe, Objekt zu entfernen...",
                  "Ausführen?"))
            goto toploop;
#else
         if (!telluser(screen,"About to Delete Object...","DO IT?"))
            goto toploop;
#endif
         msg.obj=obj;
         msg.page=&page[pg];
         msg.command=CLEAR;
         viewmonitor(&msg);      /** clear objects image **/
         if (deleteobject(obj)==FALSE) {
            msg.command=DRAWIT;
            viewmonitor(&msg);
            telluser2(screen,"PGM_ERROR05","IN MAIN.C",FALSE);
            }
         goto toploop;

      case REFLOWTEXT:
         if (verifypick(obj)!=AOK)
            goto toploop;
         if (obj->type !=TEXT) {
#if GERMAN
            telluser2(screen,"Änderung des Textflusses ist",
               "nur bei Text-Objekten möglich.",FALSE);
#else
            telluser2(screen,"Reflow only works on","text objects.",FALSE);
#endif
            goto toploop;
            }

         to = (struct text_object *)obj->first->data;
         texttoflow = tobj_text(obj);

         if (!usedefault) {
            time_for_same = TRUE;
            samestate = to->state;
            }
         else {
            time_for_same = FALSE;
            }

         autofree_off(obj);        /** don't delete text **/
         deletetextobjlist(obj->first);
         flowobj=NULL;
         mode=FLOW;
         newpointer(screen,mode);
         goto toploop;

      case SAVEOBJ:
         if (verifypick(obj)!=AOK)
            goto toploop;
         newpointer(screen,BUSY);
         showstat(screen,saveobject(clipboard,obj));
         mode=NORMAL;
         newpointer(screen,mode);
         goto toploop;

      case CROP:
         if (verifypick(obj)!=AOK)
            goto toploop;
         if (obj->type!=GRAPHIC) {
#if GERMAN
            telluser2(screen,"Abschneiden ist nur mit Graphik",
               "Objekten möglich.",FALSE);
#else
            telluser2(screen,"Cropping only works with",
               "graphic objects.",FALSE);
#endif
            mode=NORMAL;
            newpointer(screen,mode);
            goto toploop;
            }
         cropobject(obj);
         msg.obj=obj;
         msg.page=&page[pg];
         msg.command=CLEAR;
         viewmonitor(&msg);
         msg.command=DRAWIT;
         viewmonitor(&msg);
         goto toploop;


      case EDITOBJ:        /** edit objects flags (outline, shadow etc.) **/
         if (verifypick(obj)!=AOK)
            goto toploop;
    /*     tempobj=*obj; */

         msg.obj=obj;
         msg.page=&page[pg];
         msg.command=CLEAR;
         viewmonitor(&msg);

         objoptions(screen,obj, FALSE);

         msg.obj=obj;
         msg.command=DRAWIT;
         viewmonitor(&msg);
         newpointer(screen, mode);
         goto toploop;

      case CHEAPEDIT:
         if (verifypick(obj)!=AOK)
            goto toploop;
         if (obj->type==GRAPHIC) {
            msg.obj=obj;
            msg.page=&page[pg];
            msg.command=CLEAR;
            viewmonitor(&msg);

            showstat(screen,(i=graphic_ed(obj->data)));

            newpointer(screen,BUSY);
            msg.command=DRAWIT;
            viewmonitor(&msg);
            mode=NORMAL;
            newpointer(screen,mode);
            goto toploop;
            }

      case EDITFORMAT:
         if (mode == EDITFORMAT)     /** can be cheapedit **/
            if (verifypick(obj)!=AOK)
               goto toploop;

         if (obj->type!=TEXT) {
#if GERMAN
            if (!telluser2(screen,"falscher Objecttyp für",
               "diese funktion.",FALSE)) {
#else
            if (!telluser2(screen,"Wrong object type ",
               "for this function.",FALSE)) {
#endif
               mode=NORMAL;
               newpointer(screen,mode);
               goto toploop;
               }
            goto toploop;
            }

         to=(struct text_object *)obj->first->data;
         if (mode==CHEAPEDIT) {
            setmenus(screen, FALSE);    /** Turn off menus **/
            text = texted(screen,tobj_text(obj),0,0,0,640,100,&status);
            setmenus(screen, TRUE);

            if (text==NULL || status==JUSTQUIT) {
               mode=NORMAL;
               newpointer(screen,mode);
               goto toploop;
               }

            if (status==DOREFLOW) {
               remove_link(obj);
               to = (struct text_object *)obj->first->data;
               if (!usedefault) {
                  time_for_same = TRUE;
                  samestate = to->state;
                  }
               else {
                  time_for_same = FALSE;
                  }
               texttoflow = text;
               deletetextobjlist(obj->first);
               flowobj=NULL;
               mode=FLOW;
               newpointer(screen,mode);
               goto toploop;
               }

            free_all_macros(obj->first);
            }
         else {  /* mode must be EDITFORMAT */
            if ((i=newtextformat(screen,&to->state))!=AOK) {
               showstat(screen,i);
               goto toploop;
               }
            text = tobj_text(obj);
            autofree_off(obj);
            free_all_macros(obj);
           }

         mode=NORMAL;
         newpointer(screen,BUSY);
         flowobj=reflowchain(obj->first, 0, text);
         if (flowobj!=NULL) {
            mode=FLOW;
            }
         newpointer(screen,mode);
         goto toploop;

      case INVERT:

         if (obj->type!=GRAPHIC) {
#if GERMAN
            if (!telluser2(screen,"Der Invers-Befehl ist nur mit",
               "Graphik-Objeckten möglich.",FALSE)) {
#else
            if (!telluser2(screen,"The invert command only",
               "works with graphic objects.",FALSE)) {
#endif
               mode=NORMAL;
               newpointer(screen,mode);
               goto toploop;
               }
            goto toploop;
            }
         newpointer(screen,BUSY);
         msg.obj=obj;
         msg.page=&page[pg];
         msg.command=CLEAR;
         viewmonitor(&msg);

         invert(obj->data);
         freedata(obj);

         msg.command=DRAWIT;
         viewmonitor(&msg);
         mode=NORMAL;
         newpointer(screen,mode);
         goto toploop;

      case NORMAL:        /* move object in a window */

         dragbox(obj,TRUE);
         if (obj->flags&MORETEXT) {
            mode=FLOW;
            newpointer(screen,mode);
            flowobj=obj;
            }
         goto toploop;

      case GRABIT:          /* move object from one window to another */

         dragbox(obj,FALSE);
         goto toploop;

      case SIZE:

         msg.page=&page[pg];
         msg.command=SIZED;
         msg.oldh=obj->h;
         msg.oldw=obj->w;

         if (obj->type==RULE) {
            i=getline(pg,thiswindow,0,0,obj);
            showstat(screen,i);
            if (i!=AOK)
               goto toploop;
            msg.obj=obj;
            viewmonitor(&msg);
            newpointer(screen, mode);
            goto toploop;
            }

         if (obj->type==BLOCK) {
            get_block(pg,thiswindow,0,0,obj);
            msg.obj=obj;
            viewmonitor(&msg);
            newpointer(screen, mode);
            goto toploop;
            }

         if (!sizeobject(page[pg].view,obj,nice,sizedir)) {
            newpointer(screen, mode);
            goto toploop;
            }

         if (obj->type==GRAPHIC) {
            newpointer(screen,BUSY);
            msg.obj=obj;
            viewmonitor(&msg);
            newpointer(screen, mode);
            goto toploop;
            }

         /** must be text object ***/

         x=obj->w;
         y=obj->h;
         obj->w=msg.oldw;
         obj->h=msg.oldh;
         msg.command=CLEAR;
         msg.obj=obj;
         viewmonitor(&msg);
         freedata(obj);
         obj->w=x;
         obj->h=y;

         newpointer(screen,BUSY);
         texttoflow = tobj_text(obj);

         if (obj==obj->first)
            autofree_off(obj);
         free_all_macros(obj);
         flowobj=reflowchain(obj, prevobj(obj), texttoflow);
         if (flowobj==NULL) {
            mode=SIZE;
            }
         else {
            mode=FLOW;
            }
         newpointer(screen,mode);
         goto toploop;


      case DUPOBJECT:

         if (verifypick(obj)!=AOK)
            goto toploop;

         if (obj->type!=GRAPHIC && obj->type!=TEXT && obj->type!=BLOCK) {
#if GERMAN
            telluser2(screen,"Sie müssen ein Graphik- oder ein",
               "Text - Objekt wählen.",FALSE);
#else
            telluser2(screen,"You must pick a graphic,",
                "text, or block  object.",FALSE);
#endif
            goto toploop;
            }

         newpointer(screen,BUSY);

         if (obj->type==TEXT) {
            to=(struct text_object *)obj->first->data;
            texttoflow = dup_string(to->macro->macrotext->start);
            if (texttoflow==NULL) {
               showstat(screen,OUTOFMEM);
               goto toploop;
               }
            mode=FLOW;
            flowobj=NULL;
            newpointer(screen,mode);
            goto toploop;
            }

         if (obj->type==GRAPHIC) {
            objcopy=(struct object *)dupgraphic(&page[pg],obj);
            if (objcopy==NULL) {
               showstat(screen,OUTOFMEM);
               newpointer(screen, mode);
               goto toploop;
               }
            }
         else if (obj->type==BLOCK) {
            objcopy=dup_block(obj);
            if (objcopy==NULL) {
               showstat(screen,OUTOFMEM);
               newpointer(screen, mode);
               goto toploop;
               }
            }

         msg.obj=objcopy;
         msg.page=&page[pg];
         msg.command=DRAWIT;
         viewmonitor(&msg);
         newpointer(screen, mode);
         goto toploop;
      }  /** switch **/
      }  /** matchrage **/
   goto toploop;
   }    /** MOUSBUTTONS **/

/*********************/
/** closeing a page **/
/*********************/

if (class==CLOSEWINDOW) {

   setmenus(screen, FALSE);  /* turn of menus */
   if (thiswindow==coord_window) {
      close_coord();
      pg = CB;
      setmenus(screen, TRUE);
      goto toploop;
      }

   /** first change magnification to one **/
   if (page[pg].view->mag != 1) {
      for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
         obj=(struct object *)node;
         freedata(obj);    /* free memory used by *screendata */
         obj->mag=1;
         }
      }

   /** now delete view **/
   deleteview(&page[pg]);
   pg=CB;         /** This is a Quick Fix, find what is the new */
   setmenus(screen, TRUE);  /* turn on menus */
   goto toploop;  /** active window !!! */
   }


/*********************/
/** resized window ***/
/*********************/

if (class==NEWSIZE) {

newsize:

view=page[pg].view;     /** for faster calculations below **/

#if 0
/** calculate the new page's viewing area **/

   view->w=stocx(view->window->Width - XBORDER_AREA);
   view->h=stocy(view->window->Height - YBORDER_AREA);


   pi=&view->prl;
   if (view->w >= (page[pg].pw*view->mag))
      pi->HorizBody=0xFFFF;
   else 
      pi->HorizBody=0xFFFFL*(LONG)view->w/((LONG)page[pg].pw*(LONG)view->mag);

   hpot=((LONG)view->x)<<16;

   i = page[pg].pw*view->mag - view->w;
   if (i)
      hpot=min(0xFFFF,hpot/i);
   else
      hpot = 0;

   pi->HorizPot=hpot;

   pi=&view->pud;
   if (view->h >= (page[pg].ph*view->mag))
      pi->VertBody=0xFFFF;
   else
      pi->VertBody=
       0xFFFFL*(LONG)view->h/((LONG)page[pg].ph*(LONG)view->mag);

   vpot=((LONG)view->y)<<16;
   i = page[pg].ph*view->mag - view->h;
   if (i)
      vpot=min(0xFFFF,vpot/i);
   else
      vpot = 0;

   pi->VertPot=vpot;

   RefreshGadgets(&view->ud,view->window,NULL);
#endif
   correct_sliders(pg);
   remove_clip(view);
   add_clip(view);
   msg.page=&page[pg];
   msg.command=REFRESH;
   newpointer(screen,BUSY);
   viewmonitor(&msg);
   newpointer(screen,mode);
   goto toploop;
   }

if (class==RAWKEY) {
/* printf("code %d\n",code); */
   switch(code) {
      case 80: /* f1 outline objects */

         outlineobjects(&page[pg]);
         do {
            while((message=(struct IntuiMessage *)
               GetMsg(clipboard->UserPort))==0) {
               Wait(1<<clipboard->UserPort->mp_SigBit);
               }
            class=message->Class;
            code=message->Code;
            ReplyMsg(message);
            } while (class!=RAWKEY || code==80);
         outlineobjects(&page[pg]);
         break;

      case 20:  /* 't' toggle toolbox to front/back */
         if (toggle) {
            WindowToBack(toolbox);
            toggle = FALSE;
            }
         else {
            WindowToFront(toolbox);
            toggle=TRUE;
            }
         break;

      case 53:    /* 'b' selects 'BOX_FLOW' **/
         if (mode==FLOW) {
            mode=BOX_FLOW;
            newpointer(screen, mode);
            }
         break;

      case 95:    /** help key brings up list of commands **/
         show_help(screen);
         break;
      }
   }

/********************************/
/*** Menu Selectio starts here **/
/********************************/

if (class!=MENUPICK) goto toploop;

   do {
   menunum=MENUNUM(code);
   itemnum=ITEMNUM(code);
   subnum=SUBNUM(code); 

   if (mode==FLOW || mode==BOX_FLOW) {
      if (flowobj==NULL) {
#if GERMAN
         j=telluser(screen,"OK wählen, Text auszufügen","");
#else
         j=telluser(screen,"Press OK to Discard Text.","");
#endif
         if (j) {
            FreeMem(texttoflow,strlen(texttoflow)+1);
            mode = NORMAL;
            newpointer(screen,mode);
            }
         else {
            continue;
            }
         }

      if (flowobj!=NULL && flow_warnings) {
#if GERMAN
         telluser2(screen,"Warnung: Sie haben den Text noch",
            "nicht voll plaziert.",FALSE);
#else
         telluser2(screen,"Warning: You have not finshed",
          "placing the text.",FALSE);
#endif
         }
      }

/*** NEW *****/

   if (menunum==0 && itemnum==0) {
      newpointer(screen, BUSY);
      setmenus(screen, FALSE);  /* turn off menus */
      do_new(screen,TRUE);
      setmenus(screen, TRUE);  /* turn on menus */
      mode = NORMAL;
      newpointer(screen, mode);
      pg=lastpg=1;
      }

/*** open new project **/

   if (menunum==0 && itemnum==1) {
      setmenus(screen, FALSE);  /* turn off menus */
      newpointer(screen, BUSY);
      i=loadall(FALSE);       /** false == don't append **/
      if (i!=AOK) {
         showstat(screen,i);
         mode=NORMAL;
         newpointer(screen,mode);
         setmenus(screen, TRUE);  /* turn on menus */
         continue;
         }
      pg=1;
/*      pageopened[pg]=TRUE;  page one is open   */

      if (createview(screen,&page[pg])!=AOK) {
         showstat(screen,OUTOFMEM);
         pg=0;
         mode=NORMAL;
         newpointer(screen,mode);
         setmenus(screen, TRUE);  /* turn on menus */
         continue;
         }

      newpointer(screen, BUSY);  /** createview makes busy ptr go away */
      for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
         obj=(struct object *)node;
         makedisplay(obj);
         }

      msg.page=&page[CB];
      msg.command=DRAWIT;
      for (node=page[CB].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
         msg.obj = (struct object *)node;
         viewmonitor(&msg);
         }

      msg.page=&page[pg];
      msg.command=REFRESH;
      viewmonitor(&msg);

      mode=NORMAL;
      newpointer(screen,mode);
      setmenus(screen, TRUE);  /* turn on menus */
      continue;
      }

/*** append.... ***/

   if (menunum==0 && itemnum==2) {
      newpointer(screen,BUSY);
      showstat(screen,loadall(TRUE));
      newpointer(screen,RESTORE);
      continue;
      }

/*** save.... ***/

   if (menunum==0 && itemnum==3) {
      newpointer(screen,BUSY);
      showstat(screen,saveall(FALSE));
      newpointer(screen,RESTORE);
      continue;
      }

/*** save as.... ***/

   if (menunum==0 && itemnum==4) {
      newpointer(screen,BUSY);
      showstat(screen,saveall(TRUE));
      newpointer(screen,RESTORE);
      continue;
      }

/*** load a graphic from disk ***/

   if (menunum==0 && itemnum==6) {

      i=graphicload(&graphic,clipboard);
      if (i!=AOK) {
          showstat(screen,i);
         continue;
         }

putgraphic:

      newpointer(screen,PLACEGRAPHIC);
      click=getclick(clipboard);
      thiswindow=click->Window;
      if (thiswindow!=page[pg].view->window) {   /* determine current page */
         i=whatpage(thiswindow);
         if (i!=-1) pg=i;
         }
      mousex=click->x;
      mousey=click->y;

      newpointer(screen,BUSY);
      obj=createobject(&page[pg].objects);

      if (obj==0) {
         free_graphic_data(graphic);
         FreeMem(graphic,sizeof(struct graphic));
         showstat(screen,OUTOFMEM);
         newpointer(screen,mode);
         continue;
         }
      obj->pn=pg;
      obj->x=(stocx(mousex)+page[pg].view->x)/page[pg].view->mag;
      obj->y=(stocy(mousey)+page[pg].view->y)/page[pg].view->mag;
      obj->type=GRAPHIC;
      obj->data=(APTR)graphic;
/*      obj->dpi=printerprefs.dotsperinch; */
      if (printerprefs.printertype==PREFERENCES) {
   /*      obj->w=(cpertextpixx*(FIXED)graphic->w)>>16; */
         obj->w=graphic->w;
         }
      else {
         obj->w=graphic->w*2;
         }
      obj->h=(((FIXED)obj->w)<<16)/graphic->aspect;
      obj->h = (obj->h * yinch)/xinch;
      obj->mag=page[pg].view->mag;
      msg.obj=obj;
/* bug_obj(obj); */
      msg.page=&page[pg];
      msg.command=DRAWIT;
      viewmonitor(&msg);
      newpointer(screen,mode);
      continue;
      }

   if (menunum==0 && itemnum==8) {  /** save object **/
      mode=SAVEOBJ;
      newpointer(screen,mode);
      continue;
      }

   if (menunum==0 && itemnum==14) {   /* quit */
#if GERMAN
      if (!telluser(screen,"Sind Sie sicher, das Sie","aufhören wollen?"))
#else
      if (!telluser(screen,"Are you sure you want to quit?",""))
#endif
         continue;
      newpointer(screen,NORMAL);     /* makesure no pointer mem allocated */
      setmenus(screen,FALSE);        /* turn off menus */
      for (i=MAXPAGES-1; i >= 0; i--) {  /* delete all objects and windows */
         delete_page(i);
         }
      free_all_symtabs();
      free_patterns();     /** free up auto hyphenation patterns if need */
      close_fonts();
      closelaserfonts();
      closetoolbox(toolbox);         /* close toolbox window */
      CloseScreen(screen);
      closelibraries();
      if (openworkbench)
         OpenWorkBench();
      exit(0);
      }


   if (menunum==0 && itemnum==7) {   /* load text */
      newpointer(screen,BUSY);
      i=loadtext(&text2,clipboard,&len);
      if (i!=AOK) {
         newpointer(screen,mode);
         showstat(screen,i);
         continue;
         }

      i=textconv(text2,len,&texttoflow,&i);
      showstat(screen,i);
      if (i!=AOK) {
         mode = NORMAL;
         }
      else {
         mode=FLOW;        /** flow text into page **/
         flowobj=NULL;     /** this is the first object **/
         }
      newpointer(screen,mode);
      continue;
      }


   if (menunum==0 && itemnum==10) {  /** print page **/
      if (pg==CB) {
#if GERMAN
         telluser2(screen,"Sie müssen zuerst eine Seite wählen,",
            "indem Sie in deren Fenster Klicken.",FALSE);
#else
         telluser2(screen,"You must first select a page",
                   "by clicking its window",FALSE);
#endif
         }
      print_doc(screen,pg,pg);
      continue;
      }

   if (menunum==0 && itemnum==11) {  /** print entire document **/
      print_doc(screen,1,last_page());
      continue;
      }

   if (menunum==2 && itemnum==0) {  /** save prefs...**/
      showstat(screen,savepreffile());
      continue;
      }

   if (menunum==2 && itemnum==1) {  /** load prefs...**/
/*      dpi=printerprefs.dotsperinch;  cant change printer type with **/
      type=printerprefs.printertype;   /** load. Must use auto load **/
      j=screensize;
      showstat(screen,loadpreffile());
      screensize=j;
/*      printerprefs.dotsperinch=dpi; */
      printerprefs.printertype=type;
      setdeffont(screen);
      setonoff();
      if (autohyphenate) {
         newpointer(screen, BUSY);
         if (load_patterns("hyphen.tex")!=AOK)
            autohyphenate=FALSE;
         newpointer(screen, mode);
         }
      else {
         free_patterns();
         }
      msg.page=&page[pg];
      msg.command=REFRESH;
      viewmonitor(&msg);
      continue;
      }

   if (menunum==2 && itemnum==2) { /** get new prferences **/
      j = displayguides;
      type = printerprefs.printertype;
      i = cdprefs(screen);

      if (type != printerprefs.printertype) {
#if GERMAN
         if (!telluser(screen,"Sie löschen alle Seiten...","durchführen?")) {
#else
         if (!telluser(screen,"This will erase every page...","DO IT?")) {
#endif
            printerprefs.printertype = type;
            settypeflag();
            }
         else {
            newpointer(screen, BUSY);
            if (printerprefs.printertype==HPLASER) {
               hp_font_setup("autofonts");  /** try and download hp fonts **/
               strcpy(default_font_name, "courier");
               }
            else if (printerprefs.printertype==PREFERENCES)
               strcpy(default_font_name, "topaz.font");
            else if (printerprefs.printertype==POSTSCRIPT)
               strcpy(default_font_name, "courier");
            else        /** VPAGE **/
               strcpy(default_font_name, "courier_10");
            loadfonts(screen);      /** load laserfonts or set xinch yinch **/
            setdeffont(screen);     /** tell defaultstate new font **/
            settypeflag();          /** check submenu and set cpertextpix **/
            initdefaultpage();
            setmenus(screen, FALSE);  /* turn off menus */
            erase_page(CB);
            do_new(screen,FALSE);
            setmenus(screen, TRUE);  /* turn on menus */
            xshadowsize = SHADOWSIZE*xinch/576;
            yshadowsize = SHADOWSIZE*yinch/576;
            xoutlinespc = 75*xinch/576;
            youtlinespc = 75*yinch/576;
            newpointer(screen, mode);
            }
         }

      if (i != screensize) {
         if (coord_window)
            close_coord();
         showstat(screen,newscreen(&screen,i,&pg,&toolbox));
         msg.page=&page[CB];  /** redraw everything on the clipboard **/
         msg.command=DRAWIT;
         for (node=page[CB].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
            msg.obj = (struct object *)node;
            viewmonitor(&msg);
            }

         setmenus(screen, TRUE);
         mode=NORMAL;
         newpointer(screen,mode);
         }
      if (j != displayguides) {
         msg.command=REFRESH;              /** re-draw pages opened **/
         for (i=1; i<MAXPAGES; i++) {
            if (page[i].view!=NULL) {
               msg.page=&page[i];
               viewmonitor(&msg);
               }
            }
         }
      continue;
      }

   if (menunum==2 && itemnum==3) {
      show_info(screen);
      continue;
      }

   if (menunum==2 && itemnum==4) {  /** assign new fonts: **/
      if (assign_fonts(screen)==AOK) {
         newpointer(screen, BUSY);
         init_fonts();        /** initialize the available fonts chain **/
         newpointer(screen, mode);
         }
      continue;
      }

   if (menunum==2 && itemnum==5) {  /** load macro file **/
      i=load_macros(0);
      if (i!=AOK) {
         showstat(screen,i);
         }
      continue;
      }

   if (menunum==3 && itemnum==0) {   /* Set grid options **/
      grid_options(screen);
      msg.page=&page[pg];
      msg.command=REFRESH;
      viewmonitor(&msg);
      continue;
      }

/*** EDIT MENU ***/

   if (menunum==1 && itemnum==0) {     /** Cut **/
      cut_page(pg);
      pg=CB;         /** This is a Quick Fix, find what is the new */
      continue;
      }

   if (menunum==1 && itemnum==1) {     /** Copy **/
      erase_page(CUTPAGE);
      move_page(pg,CUTPAGE);
      pg=CB;         /** This is a Quick Fix, find what is the new */
      continue;
      }

   if (menunum==1 && itemnum==2) {     /** Paste **/
      move_page(CUTPAGE,pg);
      draw_page(pg);
      pg=CB;         /** This is a Quick Fix, find what is the new */
      continue;
      }

   if (menunum==1 && itemnum==3) {     /** Erase **/
      erase_page(pg);
      if (pg!=CB) {
         deleteview(&page[pg]);
         page[pg].flags &= (~PAGEINUSE);
         }
      pg=CB;         /** This is a Quick Fix, find what is the new */
      continue;
      }

   if (menunum==3 && itemnum==1) {     /* Create Graphic */
      graphic=(struct graphic *)AllocMem(sizeof(struct graphic),MEMF_CLEAR);
      if (graphic==0) {
         showstat(screen,OUTOFMEM);
         continue;
         }
      showstat(screen,(i=graphic_ed(graphic)));

      if (i==AOK)
         goto putgraphic;
      FreeMem(graphic, sizeof(struct graphic));
      continue;
      }


   if (menunum==1 && itemnum==5) {     /* edit attributes of text */
      mode=EDITFORMAT;
      newpointer(screen,mode);
      continue;
      }

   if (menunum==1 && itemnum==4) {       /* EDIT page format */
      if (pg==CB) {
#if GERMAN
         telluser2(screen,"Sie müssen zuerst eine Seite wählen,",
            "indem Sie in deren Fenster Klicken.",FALSE);
#else
         telluser2(screen,"You must first select a page",
                  "by clicking its window",FALSE);
#endif
         continue;
         }
      updatemargins(screen,&page[pg]);
      msg.page=&page[pg];
      msg.command=REFRESH;
      viewmonitor(&msg);
      continue;
      }

   if (menunum==3 && itemnum==2) {    /* invert a graohic */
      mode=INVERT;
      newpointer(screen,mode);
      continue;
      }

   if (menunum==3 && itemnum==3) {    /* Connect Chain */
      mode=PICKTEXTA;
      newpointer(screen,mode);
      continue;
      }

   if (menunum==3 && itemnum==4) {    /* Break chain */
      mode=BREAKCHAIN;
      newpointer(screen,mode);
      continue;
      }

   if (menunum==3 && itemnum==5) {
      mode = LINK;
      newpointer(screen, mode);
      continue;
      }

   if (menunum==3 && itemnum==6) {
      mode = UNLINKALL;
      newpointer(screen, mode);
      continue;
      }

   if (menunum==3 && itemnum==7) {
      if (coord_window)
         close_coord();
      else
         open_coord(screen,page[CB].view->window->UserPort);
      continue;
      }

   if (menunum==0 && itemnum==13) {
      aboutcd(screen);
      continue;
      }

   } while ( (code = ItemAddress(Titles,code)->NextSelect) != MENUNULL);

goto toploop;
}

/*************************************/
/*** does the project NEW function ***/
/*************************************/

do_new(screen, ask)

struct Screen *screen;
int ask;

{
SHORT pg,i,type;
struct viewmsg msg;
extern struct pageinfo page[MAXPAGES];

if (ask) {
#if GERMAN
   if (!telluser(screen,"Sie löschen alle Seiten...","durchführen?"))
      return(CANCEL);

#else

   if (!telluser(screen,"This will erase every page...","DO IT?"))
      return(CANCEL);
#endif
   }

ghost_save();
for (i=1; i<MAXPAGES; i++) {
   delete_page(i);
   }

pg=1;
copypage(&defaultpage,&page[DEFAULTEVEN]);   /** Restore PAGEINUSE flags */
copypage(&defaultpage,&page[DEFAULTODD]);
copypage(&defaultpage,&page[CUTPAGE]);

type=printerprefs.printertype;   /** can't change screen or printer type **/
i=screensize;
loadprefs("AUTOPREFS",FALSE);
copypage(&page[DEFAULTODD],&page[pg]);
printerprefs.printertype=type;
screensize=i;
setdeffont(screen);  /** sets font ptr in default state from ***/
                     /** name in default_font_name **/
settypeflag();       /** set xinch, yinch **/
if (autohyphenate) {
   if (load_patterns("hyphen.tex")!=AOK)
      autohyphenate=FALSE;
   }
else {
   free_patterns();
   }

if (createview(screen,&page[pg])!=AOK) {
   return(OUTOFMEM);
   }

free_all_symtabs();

msg.page=&page[pg];
msg.command=REFRESH;
viewmonitor(&msg);
return(AOK);
}


/**************************************/
/** Find object previous to this one **/
/**************************************/

struct object *prevobj(obj)

struct object *obj;

{
struct object *prevobj;

if (obj->first==obj)
   return(0);

prevobj = obj->first;
while ( ((struct text_object *)(prevobj->data))->next!=obj) 
   prevobj = ((struct text_object *)(prevobj->data))->next;

return(prevobj);
}

#if 0
/**********************/
/*** Break a chain ****/
/**********************/

breakchain(obj)

struct object *obj;

{
struct text_object *to,*t_obj,*t_first;
struct object *objcopy;
char *p,*text,*text2;
LONG len,totallen;
if (obj == obj->first)
   return(CANCEL);
t_obj=(struct text_object *)obj->data;
t_first=(struct text_object *)obj->first->data;
p=t_obj->textstart;   /** break point in string **/
totallen=strlen(t_first->textstart);
for (len=0; t_first->textstart+len != p; len++) ;
text = (char *)AllocMem(len + 1,0);
if (text==0) {
   return(OUTOFMEM);
   }
text2 = (char *)AllocMem(totallen - len + 1,0);
if (text2==0) {
   FreeMem(text,len+1);
   return(OUTOFMEM);
   }
movmem(t_first->textstart,text,len);
text[len]=0;
movmem(p,text2,totallen - len);
text2[totallen - len]=0;
FreeMem(t_first->textstart,totallen);
/* t_first->textstart = text;
t_obj->textstart = text2; */
objcopy = prevobj(obj);
to=(struct text_object *)objcopy->data;
to->next=0;
adjust_lines(obj->first,text);
for (objcopy = obj; objcopy!=0;
  objcopy = ((struct text_object *)(objcopy->data))->next)
  objcopy->first=obj;
adjust_lines(obj,text2);
return(AOK);
}

/**************************************************************************/
/** This routine changes text pointers in text_object and text_line      **/
/** so they point to a new base address. This is done with some very     **/
/** illegal armithitic that severly limits the portability of this code. **/
/** But, it is a lot faster than reflowing.                              **/
/**************************************************************************/

adjust_lines(obj,new_text_start)

struct object *obj;
long new_text_start;    /* this is really a pointer to char */

{
return(0);
struct text_object *t;
struct text_line *tl;
long old_text_start;


t = (struct text_object *)obj->data;
old_text_start = (long)told_text_start = (long)t->textstart;

while (obj != NULL) {
   t->textstart = (char *)
       ((long)t->textstart - old_text_start + new_text_start);

   t->textend = (char *)
       ((long)t->textend - old_text_start + new_text_start);
   tl = t->line;

   while (tl!=NULL) {
      tl->start = (char *)
          ((long)tl->start - old_text_start + new_text_start);

      tl->text = (char *)
          ((long)tl->text - old_text_start + new_text_start);
      tl = tl->next;
      }

   obj = t->next;
   t = (struct text_object *)obj->data;
   }
}


/*************************************************************/
/** Connect one object (b) to the end of another object (a) **/
/*************************************************************/

connect(obj_a,obj_b)

struct object *obj_a;
struct object *obj_b;

{
struct text_object *ta;
struct text_object *fta,*ftb;
struct object *objcopy;
char *newtext;
LONG len_a,len_b;

ta = (struct text_object *)obj_a->data;

fta = (struct text_object *)obj_a->first->data;
ftb = (struct text_object *)obj_b->data;

if (ta->next != NULL)         /** must be last object in chain **/
   return(CANCEL);

if (obj_b != obj_b->first)    /** must be first object in chain **/
   return(CANCEL);

if (obj_a->first == obj_b)   /** objects members of same chain? **/
   return(CANCEL);

len_a = strlen(fta->textstart);
len_b = strlen(ftb->textstart);
newtext = (char *)AllocMem(len_a + len_b + 1,0);
if (newtext == NULL)
   return(OUTOFMEM);

if (ftb->textstart[0]=='\n')
   ftb->textstart[0]=' ';  /* convert leading newline to space */

movmem(fta->textstart, newtext, len_a);
movmem(ftb->textstart, newtext + len_a, len_b);
newtext[len_a + len_b] = 0;

FreeMem(fta->textstart, len_a + 1);
FreeMem(ftb->textstart, len_b + 1);

fta->textstart = newtext;

for (objcopy = obj_b; objcopy!=0;
  objcopy = ((struct text_object *)(objcopy->data))->next)
  objcopy->first=obj_a->first;

ta->next = obj_b;

reflowchain(obj_a->first);
return(AOK);
}
#endif

close_all_views()

{
int i;

for (i=1; i<MAXPAGES; i++)
   deleteview(&page[i]);
}

bug_obj(obj)
struct object *obj;
{
printf("obj = %x\n",obj);
printf("screendata %x\n",obj->screendata);
printf("top_link %x\n",obj->top_link);
printf("next %x\n",obj->next);
printf("first %x\n",obj->first);
printf("data %x\n",obj->data);
printf("flags %x\n",obj->flags);
printf("x %d y %d h %d w %d\n",obj->x,obj->y,obj->h,obj->w);
printf("pn %d type %d\n",obj->pn,obj->type);
printf("dpi %d numplanes %d density %d mag %d\n",obj->dpi, obj->numplanes,obj->density, obj->mag);
}

free_all_macros(obj)

struct object *obj;

{
char *text;

text = tobj_text(obj);

while (obj) {
 /* printf("MAIN: B4 REFLOW: ref count = %d\n",
 ((struct text_object *)(obj->data))->macro->macrotext->ref_count); */

   free_macro_chain(((struct text_object *)(obj->data))->macro);
   ((struct text_object *)(obj->data))->macro=NULL;
   free_macro_chain(((struct text_object *)(obj->data))->endmacro);
   ((struct text_object *)(obj->data))->endmacro=NULL;
   obj=((struct text_object *)(obj->data))->next;     /** tobj->next **/
   }
return(text);
}

tobj_text(obj)

struct object *obj;

{
return( ((struct text_object *)(obj->data))->macro->macrotext->start);
}

dup_string(p)

char *p;

{
char *newp;

newp = AllocMem(strlen(p)+1,0);
if (newp)
   strcpy(newp, p);
return(newp);
}

autofree_on(obj)

struct object *obj;

{
struct text_object *to;

to = (struct text_object *)obj->first->data;
(to->macro->macrotext->flags) |= MTEXT_FREETEXT;
}

autofree_off(obj)

struct object *obj;

{
struct text_object *to;

/* printf("called autofree_off\n"); */

to = (struct text_object *)obj->first->data;
(to->macro->macrotext->flags) &= (~MTEXT_FREETEXT);
}

