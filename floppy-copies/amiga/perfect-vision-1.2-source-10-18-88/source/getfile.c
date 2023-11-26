/************************************************************************
req.c
   This file contains a general-purpose <requester> that
will prompt the user for a filename input.
   The program actually uses a window instead of a 'Requester'
for greater flexibility. It will take control of your window's
IDCMP Port when called.

***   This material is copyright (c) 1986 by C. Heath of Microsmiths, Inc.
Permission is granted to use these files in any way with the following
exceptions:

1) The files shall not be posted on any telecommunications service, public
or private, except for BIX until January 15, 1987.

2) The files may only be distributed in archive format, with no modifications.
If you make any improvements on the file requester and would like to
generally distribute them, please contact "cheath" on BIX, or write to:
   Microsmiths Inc, PO Box 561, Cambridge, MA 02140

3) The requester may be used in any commercial product, but must be in
object code format.  You are free to make modifications for use in your
product.  Permission is granted to Lattice, Inc, and to Manx, Inc, to
include the source files in archive format.

   Thank you, and enjoy.
      ...cheath

************************************************************************/

#include "standard.h"
#include "safeclose.h"

#define GERMAN 0
#define FAST register
#define NL NULL

extern char *dmore(), *dinit();

static struct FileLock      *pdir = NL;
static struct FileInfoBlock *dir_info;

static struct Window   *eW;      /* Parent Window. Uck   */

static struct TextAttr MyFont = {"topaz.font",TOPAZ_EIGHTY,FS_NORMAL,FPF_ROMFONT };

/* FGAD requires a few unique gadget-ids, origined at FGAD   */
/* In the unlikely event this conflicts with any of your gadgets, */
/* change this equate to allow 16 contiguous unique ID's   */

#define FGAD   0x76c0L

#define FCHARS   32L   /* Number of chars allowed in file name   */

#define DIR_SIZ   50L   /* Number of chars in a dir name...   */
#define MAX_STR   DIR_SIZ+2L

#define DENTS   5L      /* Number of entries on screen   */
/* It's not as simple as changing 'DENTS'...      */

#define DSIZE   FCHARS+1L   /* Size of a directory entry   */

#define   DBUFSIZ   3000L   /* Number of bytes to allocate for all ents */

#define BCOL   1L   /* Background color   */
#define FCOL   2L   /* Foreground color   */

#define RHGHT   120L
#define RWDTH   320L

#define ZWDTH   270L

static char   ubuf[MAX_STR];      /* Undo buffer   */

static struct dirent { struct dirent *next; BOOL isfile; char dE[DSIZE+2]; };
static struct dirent *FirstEntry;
static struct dirent *NextEntry;

static struct dirhead { struct dirent *next; };
static struct dirhead ListHead;

static long   curent,maxent;
static BOOL   more;

static struct Window   *wRq = NL;      /* Requester window   */
static struct RastPort   *wRp;

/* Requester "Hailing" prompt */
static struct IntuiText oTxt = {2,2,JAM1,10,11,NL, NL ,NL};

static struct IntuiText saydir = {0,1,JAM2,0,1,&MyFont,(UBYTE *)"(dir) ",NL};

static struct IntuiText rname[DENTS] = { /* File name list */
   {2,1,JAM2,48,1,NL,NL, NL},
   {2,1,JAM2,48,1,NL,NL, NL},
   {2,1,JAM2,48,1,NL,NL, NL},
   {2,1,JAM2,48,1,NL,NL, NL},
   {2,1,JAM2,48,1,NL,NL, NL} };

/* Display for file name ... */

static SHORT oXY2[] = {-2,-2, RWDTH-78,-2, RWDTH-78,9, -2,9, -2,-2};
static struct Border thebd = {0,0, 2,3,JAM1, 5,oXY2, NL};

static struct IntuiText otxt = {2,2,JAM1,-40,0,&MyFont,(UBYTE *)"File",NL};
static struct StringInfo osx = { NL ,ubuf, NL,DSIZE,NL,NL,NL,NL,NL,NL,NL,NL,NL};
static struct Gadget ogx = { NL, 60,RHGHT-35,RWDTH-80 ,10,     /* File name gadget */
   GADGHCOMP, RELVERIFY , STRGADGET,
   (APTR)&thebd,NL,&otxt,NL,(APTR)&osx, FGAD+11,NL };

static struct Gadget oga = { &ogx, 10,70, ZWDTH,10,   /* Gadgets For   */
   GADGHCOMP, RELVERIFY, BOOLGADGET,     /* Directory entries   */
   NL,NL, &rname[4] ,NL,NL, FGAD+10,NL };
static struct Gadget og9 = {&oga, 10,60, ZWDTH,10,
   GADGHCOMP, RELVERIFY, BOOLGADGET,
   NL,NL, &rname[3] ,NL,NL, FGAD+9,NL };
struct Gadget og8 = {&og9, 10,50, ZWDTH,10,
   GADGHCOMP, RELVERIFY, BOOLGADGET,
   NL,NL, &rname[2] ,NL,NL, FGAD+8,NL };
static struct Gadget og7 = {&og8, 10,40, ZWDTH,10,
   GADGHCOMP, RELVERIFY, BOOLGADGET,
   NL,NL, &rname[1] ,NL,NL, FGAD+7,NL };
static struct Gadget og6 = {&og7, 10,30, ZWDTH,10,
   GADGHCOMP, RELVERIFY, BOOLGADGET,
   NL,NL, &rname[0] ,NL,NL, FGAD+6,NL };


/* Gadjets for requester  */

static SHORT oXY3[] = {0,0, 50,0, 50,9, 0,9, 0,0};
static SHORT oXY4[] = {2,-2, 48,-2, 52,2, 52,7, 48,11, 2,11, -2,7, -2,2, 2,-2};
static struct Border obd2 = {0,0, 2,3,JAM1, 9,oXY4, NL};
static struct Border obd1 = {0,0, 3,2,JAM1, 5,oXY3, &obd2};  /* OTAY / CANCEL box */

#if GERMAN
static struct IntuiText ot1 = {0,0,JAM1,1,1,&MyFont,(UBYTE *)"  JA  ",NL};
static struct IntuiText ot2 = {0,0,JAM1,1,1,&MyFont,(UBYTE *)" NEIN ",NL};

static struct IntuiText dtxt = {2,2,JAM1,-60,0,NL,(UBYTE *)"Ordner",NL};

#else

static struct IntuiText ot1 = {0,0,JAM1,1,1,&MyFont,(UBYTE *)"  OK  ",NL};
static struct IntuiText ot2 = {0,0,JAM1,1,1,&MyFont,(UBYTE *)"Cancel",NL};

static struct IntuiText dtxt = {2,2,JAM1,-60,0,NL,(UBYTE *)"Drawer",NL};
#endif

static struct StringInfo os5 = { NL ,ubuf, NL,DIR_SIZ,NL,NL,NL,NL,NL,NL,NL,NL,NL};
static struct Gadget og5 = { &og6, RWDTH/2-80,19,190,10,     /* Directory */
   GADGHCOMP, RELVERIFY, STRGADGET,
   NL,NL,&dtxt,NL,(APTR)&os5, FGAD+5,NL };

static struct Image   cc_img;
static struct PropInfo   cc_prop = {AUTOKNOB | FREEVERT, 0,0, 0,0x1000,0,0,0,0,0,0 };
static struct Gadget og3 = { &og5,RWDTH-39,20,20,60,      /* Scroll Bar   */
    GADGHCOMP,GADGIMMEDIATE | FOLLOWMOUSE, PROPGADGET,
    (APTR)&cc_img,NL,NL,NL,(APTR)&cc_prop,FGAD+3,NL };

static struct Gadget og2 = { &og3, RWDTH-70,RHGHT-20, 50,10,  /* CANCEL */
   GADGHCOMP,  RELVERIFY, BOOLGADGET,
   (APTR)&obd1,NL, &ot2,NL,NL, FGAD+2,NL };

static struct Gadget og1 = { &og2, 20,RHGHT-20, 50,10,      /* OTAY   */
   GADGHCOMP,      /* Flags   */
   RELVERIFY,      /* Activation   */
   BOOLGADGET,
   (APTR)&obd1,NL,   /* GadgetRender, SelectRender   */
   &ot1,NL,NL,      /* IntuiText, MutualExclude,SpecialInfo   */
   FGAD+1,NL };      /* Gadget Id, UserData   */

/* Open a requester "Window" */

static struct NewWindow NewFiles = {
   0, 15, RWDTH,RHGHT, BCOL,FCOL, NL, /* Fill in AFTER opening ... */
   SMART_REFRESH | ACTIVATE | RMBTRAP | WINDOWDRAG,
   &og1,NL,NL, NL,
   NL, RWDTH,RHGHT,RWDTH,RHGHT, WBENCHSCREEN };

IMPORT  struct Library   *IntuitionBase;

/***************************************************
*  get_fname(window,screen,hail,ddef,ddir);
*
*   Displays a window/requester that
* gets a file name for device,directory,default file, extension
*
*   Calling args:
* window:   Window making the request
* screen:   Screen, if NULL assummed workbench
* hail:   Text prompt at top of requester
* ddef:   Input default file-name. Has NO DIRECTORY OR EXTENSION.
* ddir:   Directory of file, may be null

/* Set a file-requester with prompt 'hail'   */

char *get_fname(cW,screen,hail,ddef,ddir)
   struct Window *cW;      /* Calling Window   */
   struct Screen *screen;   /* screen .... if null assumed workbench */
   UBYTE   *hail;      /* Hailing prompt   */
   char      *ddef;      /* Proable file-name   */
   char      *ddir;      /* Directory in which to search   */
   {
   FAST struct IntuiMessage *imes;   /* Wait for message in HERE   */
   FAST struct Gadget   *igad;   /* Get Gadjet Mumbo Jumbo   */
   FAST long   i,class;
   FAST TEXT   *pnam;

   FAST char   *retval;
   FAST BOOL   dir_flag;
   FAST BOOL   keepon;

   if ( ! (eW = cW) )   return(NL);

   osx.Buffer = ddef;   /* Set default file name   */
   os5.Buffer = ddir;   /* Set default device name   */

   for ( i=0; i<DENTS; i++)
      {
      rname[i].IText = "";
      rname[i].NextText = NL;
      };

   NewFiles.Title = eW->Title;
   if ((dir_info = AllocMem((long)sizeof(struct FileInfoBlock),0L)) == NULL)
      return(NL);

   if (screen)      /* User supplied a screen */
      {
      NewFiles.Type = CUSTOMSCREEN;
      NewFiles.Screen = screen;
      }

   if ( ! (FirstEntry = (struct dirent *)AllocMem((long)DBUFSIZ,0L)) ||
        ! (wRq = (struct Window *)OpenWindow( &NewFiles )) )
      {
      if ( FirstEntry )   FreeMem(FirstEntry,(long)DBUFSIZ);
      /* notify("Can't Open Requester..."); */
      FreeMem(dir_info,(long)sizeof(struct FileInfoBlock));
      return(NL);
      }

   /* Set up directory, notify any errors...   */
   if ( pnam = (char *)dinit(ddir) )   notify(pnam);


   /* This optional line will activate a string gadget   */
   if ( IntuitionBase->lib_Version > 32 )
   {
   ActivateGadget(&ogx,wRq,0L);
   }

   wRp = wRq->RPort;

   wRq->UserPort = eW->UserPort;
   ModifyIDCMP(wRq,(long)(MOUSEBUTTONS | GADGETDOWN | GADGETUP | MOUSEMOVE));

   SetAPen(wRp,1L);
   RectFill(wRp,4L,10L,(long)(RWDTH-5),(long)(RHGHT-4));

   oTxt.IText = hail;   /* Set calling arg   */
   oTxt.LeftEdge = (RWDTH - IntuiTextLength(&oTxt)) >> 1L;
   PrintIText(wRp,&oTxt,0L,0L);

   RefreshGadgets(&og1,wRq,(long)NL);
   for ( retval= NL, keepon=TRUE; keepon ; )
      {
      while ( ! (imes=(struct IntuiMessage *)GetMsg(wRq->UserPort)) )
      {
         if ( dir_flag )
            {
            i = (maxent-DENTS) * cc_prop.VertPot / MAXBODY;
            if ( i > (maxent-DENTS) )
            i = maxent-DENTS;
            if ( i <0 )   i = 0;
            curent = i;
            cxxx();
            dir_flag = FALSE;
            }
         if ( more )
            {
            if (pnam = (char *)dmore())   /* Continue to read the directory */
               notify(pnam);      /* Yucko error   */
            if ( maxent <= DENTS ) dir_flag = TRUE;
            }
         else    WaitPort(wRq->UserPort);
         }
      igad = (struct Gadget *)imes->IAddress;
      class = imes->Class;
      ReplyMsg(imes);

      switch (class)
         {
         case MOUSEMOVE:      dir_flag = TRUE;   break;
      
         case GADGETUP:
         case GADGETDOWN:
            switch ( i = igad->GadgetID)
            {
            case FGAD+6:
            case FGAD+7:
            case FGAD+8:
            case FGAD+9:
            case FGAD+10:       /* Replace file or directory name   */
               pnam = rname[i - (FGAD+6)].IText;
               if ( rname[igad->GadgetID - (FGAD+6)].NextText == NL )
                  {
                  RemoveGadget(wRq,&ogx);
                  for (i=0; i<DSIZE; i++)      ddef[i] = *pnam++;
                  AddGadget(wRq,&ogx,-1L);
                  RefreshGadgets(&ogx,wRq,(long)NL);
                  break;
                  }
                  else
                  {
                  RemoveGadget(wRq,&og5);
                  rfnam(ddir,pnam);
                  AddGadget(wRq,&og5,-1L);
                  RefreshGadgets(&og5,wRq,(long)NL);
                  }
            case FGAD+5:
               if ( pnam = (char *)dinit(ddir) )
                  notify(pnam);
            case FGAD+3:
               dir_flag = TRUE;
               break;

            case FGAD+11:      /* Name gadget, OTAY gadget   */
            case FGAD+1:
               retval = ddef;
            case FGAD+2:      /* Cancel gadget   */
               keepon = FALSE;
            }
         }
      }

      FreeMem(FirstEntry,(long)DBUFSIZ );
      FreeMem(dir_info,(long)sizeof(struct FileInfoBlock));
      free_pdir();

      CloseWindowSafely(wRq);
      return(retval);
   }

static free_pdir()
   {
   if ( pdir )
      {
      UnLock(pdir);
      pdir = NL;
      }
   }

/*****************************************************************
* dinit()
*   Initialize the fib for directory muck.  Null return
* is good, else return is a pointer to an error string      */

static char *dinit(subdir)
   char *subdir;
   {
   more = FALSE;
   curent = maxent = 0;

   NextEntry = FirstEntry;      /* Allocate from here   */
   ListHead.next = NL;          /* Clear the boogie     */

   free_pdir();   /* Unlock any old lock... */

   if (! (pdir=(struct FileLock *)Lock(subdir,(ULONG)ACCESS_READ)) )
      return("Wrong Diskette?");
   if ( ! Examine(pdir, dir_info) )   return("Wierd Disk Error");
   if ( dir_info->fib_DirEntryType < 0L )   return("Bizzare Alert!!");

   more = TRUE;
   return(dmore());
   }


static char *dmore()
   {
   FAST struct dirent   *p_D = NextEntry;
   FAST struct dirent   *ptr = (struct dirent *)&ListHead;
   FAST struct dirent   *plink;

   FAST   TEXT   *p_mung;

   FAST long    i;

   if ( ! more )   return(NL);

   if ( ExNext( pdir, dir_info ) )
      {


      if ( (ULONG)p_D >=
         ((ULONG)FirstEntry + (ULONG)DBUFSIZ - (ULONG)sizeof(struct dirent)) )
         {
         more = FALSE;
         return("Directory Truncated!");
         }


      /* Here you can add a file/directory filter   */
      /* filename text string is at &p_D->dE[0   */
      p_D->isfile = ( dir_info->fib_DirEntryType < 0L );

      p_mung = &p_D->dE[0];
      for ( i=0; i<FCHARS; i++)
         if ( ! (*p_mung++ = dir_info->fib_FileName[i]) )   break;

      i = (long)p_mung;
      NextEntry = (struct dirent *)( (i+5L) & ~3L );

      for ( i=maxent++; i>=0; i--)
         {
         if ( ! (plink = ptr->next)  )   break;
         if ( alpha_lower(p_D,plink) )   break;
         ptr = plink;
         }
      p_D->next = plink;
      ptr->next = p_D;

      return(NL);
      }
   else return ( IoErr() == ERROR_NO_MORE_ENTRIES) ?
       (char *)(more = (long)NL) : "Error Reading Directory!!!";
   }


/* dedicated alphabetizing function for dmore()   */

static alpha_lower(snew,sold)
   struct dirent *snew,*sold;
   {
   FAST struct dirent *pnew = snew;
   FAST TEXT *ps1,*ps2, c,d;

   if ( pnew->isfile == sold->isfile)
      {
      ps1 = &pnew->dE[0];
      ps2 = &sold->dE[0];
      while ( (c=*ps1++) )
         {
         if ( c > (d=*ps2++) )   return(FALSE);
         else if ( c < d )      break;
         }
      return(TRUE);
      }
   return(pnew->isfile);
   }   



/* Display directory stuff   */

static cxxx()
   {
   FAST long   i,new;
   FAST long   x,y;
   FAST struct dirent *ohboy = (struct dirent *)&ListHead;

   new = curent;
   for ( i=0; i<new; i++)   ohboy = ohboy->next;

   y = 20L;
   for (i=0; i<DENTS; i++)
      {
      y += (x=10);
      rname[i].NextText = NL;
      rname[i].IText = "";
      rname[i].LeftEdge = 0;
      if ( (new+i) < maxent )
         {
         ohboy = ohboy->next;
         rname[i].IText = &ohboy->dE[0];
         if ( ohboy->isfile )   PrintIText(wRp,&rname[i],10L,y);
         else
            {
            rname[i].LeftEdge = 48;
            PrintIText(wRp,&saydir,10L,y);
            PrintIText(wRp,&rname[i],10L,y);
            rname[i].NextText = &saydir;
            }
         x = wRp->cp_x;
         }
      if ( x < ZWDTH+10 )   RectFill(wRp,x,y,(long)(ZWDTH+10),(long)(y+8L));
      }
   }


/**************************************************
* rfnam()
*   Combines dir, plus name into dir   */

static rfnam(dir,fil_nam)
   char *dir,*fil_nam;
   {
   FAST char   *pdst = dir;
   FAST char   *psrc = fil_nam;
   FAST char   c = ':';

   while ( *pdst )
      c = *pdst++;
   if ( c != ':')   *pdst++ = '/';

   while ( *pdst++ = *psrc++ )
      ;
   }

static struct IntuiText b_txt = {0,1,JAM2, 5,20,NL,NL,    NL};
static struct IntuiText p_txt = {0,1,JAM2, 5,3,NL,"OK", NL};

/****************************************************************/
/* notify(txt)                     */
/*   Prompts for Yes/No response            */

static notify(txt)
   char *txt;
   {
   b_txt.IText = txt;
   AutoRequest(wRq,&b_txt,0L,&p_txt,0L,0L,
      (long)(IntuiTextLength(&b_txt)+50L),70L);
   }
