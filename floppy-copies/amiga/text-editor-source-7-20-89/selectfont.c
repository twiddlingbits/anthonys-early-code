#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"
#include "stdio.h"

#define SIZELEN 15


struct fontlist {
   char  *name;
   COORD size[SIZELEN];
   char  type[SIZELEN][6];    /** BIbi  **/
   };

extern struct printerprefs printerprefs;
extern struct laserfont *firstlaserfont;
extern int frac[];         /** in newtextformat.c **/

SHORT listlen;

selectfont(screen,fname,size)

char *fname;
COORD *size;
struct Screen *screen;

{

static USHORT BorderVectors1[] = {0,0,58,0,58,10,0,10,0,0};
static struct Border Border1 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors1,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText1 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,1,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "CANCEL",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget cancel = {
     NULL,     /* next gadget */
     194,62,     /* origin XY of hit box relative to window TopLeft */
     55,9,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     RELVERIFY,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border1,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText1,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     9,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors3[] = {0,0,71,0,71,10,0,10,0,0};
static struct Border Border3 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors3,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText3 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     2,1,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     " LASER ",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget laser = {
     &cancel,     /* next gadget */
     76,42,     /* origin XY of hit box relative to window TopLeft */
     68,9,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border3,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText3,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     6,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors4[] = {0,0,50,0,50,10,0,10,0,0};
static struct Border Border4 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors4,     /* pointer to XY vectors */
     NULL     /* next border in list */
     };

static struct IntuiText IText4 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,1,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "AMIGA",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget amiga = {
     &laser,     /* next gadget */
     15,42,     /* origin XY of hit box relative to window TopLeft */
     47,9,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border4,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText4,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     6,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors5[] = {0,0,19,0,19,9,0,9,0,0};
static struct Border Border5 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors5,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText5 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,0,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "+",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget size_plus = {
     &amiga,     /* next gadget */
     221,25,     /* origin XY of hit box relative to window TopLeft */
     16,8,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border5,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText5,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     5,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors6[] = {0,0,19,0,19,9,0,9,0,0};
static struct Border Border6 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors6,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText6 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,0,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "-",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget size_minus = {
     &size_plus,     /* next gadget */
     196,25,     /* origin XY of hit box relative to window TopLeft */
     16,8,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border6,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText6,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     4,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors7[] = {0,0,19,0,19,9,0,9,0,0};
static struct Border Border7 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors7,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText7 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,0,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "-",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget font_minus = {
     &size_minus,     /* next gadget */
     13,25,     /* origin XY of hit box relative to window TopLeft */
     16,8,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border7,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText7,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     3,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors8[] = {0,0,28,0,28,10,0,10,0,0};
static struct Border Border8 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors8,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText8 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,1,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "OK",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget ok = {
     &font_minus,     /* next gadget */
     25,62,     /* origin XY of hit box relative to window TopLeft */
     25,9,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     RELVERIFY,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border8,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText8,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     2,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

static USHORT BorderVectors9[] = {0,0,19,0,19,9,0,9,0,0};
static struct Border Border9 = {
     -2,-1,     /* border XY origin relative to container TopLeft */
     3,0,JAM1,     /* front pen, back pen and drawmode */
     5,     /* number of XY vectors */
     BorderVectors9,     /* pointer to XY vectors */
     NULL     /* next border in list */
};

static struct IntuiText IText9 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     4,0,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "+",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct Gadget font_plus = {
     &ok,     /* next gadget */
     38,25,     /* origin XY of hit box relative to window TopLeft */
     16,8,     /* hit box width and height */
     GADGHCOMP,     /* gadget flags */
     GADGIMMEDIATE,     /* activation flags */
     BOOLGADGET,     /* gadget type */
     (APTR)&Border9,     /* gadget border or image to be rendered */
     NULL,     /* alternate imagery for selection */
     &IText9,     /* first IntuiText structure */
     0,     /* gadget mutual-exclude long word */
     NULL,     /* SpecialInfo structure for string gadgets */
     1,     /* user-definable data (ordinal gadget number) */
     NULL     /* pointer to user-definable data */
};

/* Gadget list */

static struct IntuiText IText11 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     185,14,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "Size:",     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct IntuiText IText10 = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     11,14,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     "Font:",     /* pointer to text */
     &IText11     /* next IntuiText structure */
};

#define ITextList IText10

static struct NewWindow NewWindowStructure = {
     174,47,     /* window XY origin relative to TopLeft of screen */
     274,76,     /* window width and height */
     0,1,     /* detail and block pens */
     GADGETDOWN|GADGETUP,     /* IDCMP flags */
     ACTIVATE|WINDOWDRAG|NOCAREREFRESH,     /* other window flags */
     &font_plus,     /* first gadget in gadget list */
     NULL,     /* custom CHECKMARK imagery */
     "Select a Font",     /* window title */
     NULL,     /* custom screen */
     NULL,     /* custom bitmap */
     5,5,     /* minimum width and height */
     640,200,     /* maximum width and height */
     CUSTOMSCREEN     /* destination screen type */
};


struct Window *w;
ULONG class;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();
SHORT k;                               /** font names kount **/
struct fontlist *fl;
SHORT f_index=0,s_index=0;       /** font and size index **/
SHORT makelaserlist(),makeamigalist();

amiga.Flags &= (~SELECTED);
laser.Flags &= (~SELECTED);

getflmem(&fl);
if (fl == NULL) {
   showstat(screen,OUTOFMEM);
   return(CANCEL);
   }

k=makeamigalist(fl);
if (getfontpos(fl,k,fname,*size,&f_index,&s_index)) {
   amiga.Flags |= SELECTED;
   }

else if (firstlaserfont == NULL) {
   amiga.Flags |= SELECTED;
   f_index = 0;
   s_index = 0;
   }

else {
   laser.Flags |= SELECTED;
   k=makelaserlist(fl);
   getfontpos(fl,k,fname,*size,&f_index,&s_index);
   }


NewWindowStructure.Screen=screen;
w=(struct Window *)OpenWindow(&NewWindowStructure);
if (w==NULL) {
   freeflmem(fl);
   return(OUTOFMEM);
   }
PrintIText(w->RPort,&ITextList,0,0);

showfont(w,fl,f_index,s_index);

loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   if (class==GADGETDOWN && address==&font_plus) {
      if (f_index < k-1) {
         f_index++;
         s_index=0;
         }
      }

   if (class==GADGETDOWN && address==&font_minus) {
      if (f_index !=0) {
         f_index--;
         s_index=0;
         }
      }

   if (class==GADGETDOWN && address==&size_plus) {
      s_index++;
      if ( fl[f_index].size[s_index] == 0)
         s_index--;
       }

   if (class==GADGETDOWN && address==&size_minus) {
      if (s_index != 0)
         s_index--;
      }

   showfont(w,fl,f_index,s_index);

   if (class==GADGETDOWN && address==&laser) {
      if (firstlaserfont == NULL) {
         telluser2(screen,"No laser fonts","loaded!",FALSE);
         continue;
         }
      else {
         f_index=0;
         s_index=0;
         k=makelaserlist(fl);
         showfont(w,fl,f_index,s_index);
         }
      }

   if (class==GADGETDOWN && address==&amiga) {
      if (printerprefs.printertype == POSTSCRIPT) {
         telluser2(screen,"Can't Use Amiga","Fonts With PostScript.",FALSE);
         continue;
         }
      f_index=0;
      s_index=0;
      k=makeamigalist(fl);
      showfont(w,fl,f_index,s_index);
      }

   if (class==GADGETDOWN && address->GadgetID==6)  {
      mutexc(w,address);    /** my own mutual exclude **/
      }

   if (class==GADGETUP && address==&cancel) {
      NewWindowStructure.FirstGadget=w->FirstGadget->NextGadget;
      CloseWindow(w);
      freeflmem(fl);
      return(CANCEL);
      }

   if (class==GADGETUP && address==&ok) {
      NewWindowStructure.FirstGadget=w->FirstGadget->NextGadget;
      CloseWindow(w);                           /** skip drag bar **/
      *size=fl[f_index].size[s_index];
      strcpy(fname,fl[f_index].name);
      freeflmem(fl);
      return(AOK);
      }
   }

goto loop;
}



showfont(w,fl,f_index,s_index)

struct Window *w;
struct fontlist fl[];
SHORT f_index,s_index;

{

static char t[20];
static struct IntuiText fname = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   60,14,
   NULL,
   t,
   0,
   };

static struct IntuiText fsize = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     230,14,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     t,     /* pointer to text */
     NULL     /* next IntuiText structure */
};

static struct IntuiText atr = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     185,42,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     t,     /* pointer to text */
     NULL     /* next IntuiText structure */
};


sprintf(t,"%-15s",fl[f_index].name);
PrintIText(w->RPort,&fname,0,0);

strcpy(t,"    ");                   /** clear previous **/
PrintIText(w->RPort,&fsize,0,0);

sprintf(t,"%d.%d",fl[f_index].size[s_index]>>3,
             frac[fl[f_index].size[s_index]&7]);
PrintIText(w->RPort,&fsize,0,0);

if (fl[f_index].type[s_index][0]==0) {
   strcpy(t,"          ");                   /** clear previous **/
   }
else {
   sprintf(t,"Attr: %-4s",fl[f_index].type[s_index]);
   }

PrintIText(w->RPort,&atr,0,0);
}


SHORT makelaserlist(names)

struct fontlist names[];

{
struct laserfont *lf;
SHORT k;
SHORT i,j;

for (i=0; i< listlen; i++) {
   for (k=0; k< SIZELEN; k++) {
      names[i].type[k][0]=0;
      names[i].size[k]=0;
      }
   }

k=0;

for (lf=firstlaserfont; lf!=NULL; lf=lf->next) {
   for (i=0; i<k; i++) {
      if (!strcmp(lf->fontname, names[i].name))
         goto exists;
      }

   k++;
   names[i].name=lf->fontname;

exists:

   j=0;
   while (names[i].size[j]!=0 && names[i].size[j]!=lf->size &&
          j < SIZELEN-1 ) j++;
   names[i].size[j]=lf->size;
   if (strlen(names[i].type[j]) < 4) {
      if (lf->style==BOLD)
         strcat(names[i].type[j],"B");
      if (lf->style==ITALIC)
         strcat(names[i].type[j],"I");
      if (lf->style==(ITALIC|BOLD))
         strcat(names[i].type[j],"bi");
      }
   if (k >= listlen) break;
   }

sortfontlist(names,k);
return(k);
}


SHORT makeamigalist(names)
struct fontlist names[];
{
extern struct screenfontnode *firstscreenfont;
struct screenfontnode *sf;
SHORT k;
SHORT i,j;

for (i=0; i< listlen; i++) {
   for (k=0; k< SIZELEN; k++) {
      strcpy(names[i].type[k],"BIbi");
      names[i].size[k]=0;
      }
   }

k=0;

for (sf=firstscreenfont; sf!=NULL; sf=sf->next) {
   for (i=0; i<k; i++) {
      if (!strcmp(sf->name, names[i].name))
         goto exists;
      }

   k++;
   names[i].name=sf->name;

exists:

   j=0;
   while (names[i].size[j]!=0 && names[i].size[j]!=sf->size &&
          j < SIZELEN-1 ) j++;
   names[i].size[j]=sf->size;
   if (k >= listlen) break;
   }

sortfontlist(names,k);
return(k);
}


sortfontlist(fl,k)

struct fontlist fl[];
int k;

{
SHORT i,j,a,swaped;

for (i=0; i < k; i++) {
   j=0;
   while (fl[i].size[j]!=0 && j < SIZELEN) j++;

   /** bubble sort **/

   swaped=TRUE;
   while (swaped) {
      swaped=FALSE;
      for (a=0; a < j-1; a++) {
         if (fl[i].size[a] > fl[i].size[a+1]) {
            swap (&fl[i].size[a], &fl[i].size[a+1]);
            strswap (fl[i].type[a], fl[i].type[a+1]);
            swaped=TRUE;
            }
         }
      }

   }
}

swap(x,y)

SHORT *x,*y;

{
SHORT temp;

temp=*x;
*x=*y;
*y=temp;
}

strswap (s1,s2)

char s1[6],s2[6];
{
short i;
char temp[6];

for (i=0; i<6; i++)
   temp[i] = s1[i];

for (i=0; i<6; i++)
   s1[i] = s2[i];

for (i=0; i<6; i++)
   s2[i] = temp[i];
}

getfontpos(fl,k,fname,size,fpos,spos)

struct fontlist fl[];
SHORT k;
char *fname;
COORD size;
SHORT *fpos,*spos;

{
SHORT i,j;

for (i=0; i < k; i++) {
   if (!strcmp(fl[i].name,fname)) {    /** got name **/
      j=0;
      while (fl[i].size[j]!=0) {
         if (fl[i].size[j]==size) {
            *fpos=i;
            *spos=j;
            return(TRUE);
            }
         j++;
         }
      return(FALSE);
      }
   }
return(FALSE);
}

/*********************************************/
/** Allocate memory for the font list array **/
/*********************************************/

getflmem(fl)
struct fontlist **fl;
{
listlen = findmaxsize();
*fl = (struct fontlist *)AllocMem(sizeof(struct fontlist)*listlen, 0);
}

/*********************************************/
/** Free the fontlist array's memory        **/
/*********************************************/

freeflmem(fl)
struct fontlist *fl;
{
FreeMem(fl,sizeof(struct fontlist)*listlen);
}

/*********************************************/
/** figure out what the maximum number of   **/
/** font entrys in fontlist we will need    **/
/*********************************************/

findmaxsize()
{

extern struct screenfontnode *firstscreenfont;
struct screenfontnode *sf;
struct laserfont *lf;
SHORT k,j;

k=j=0;

for (sf=firstscreenfont; sf!=NULL; sf=sf->next)
   k++;

for (lf=firstlaserfont; lf!=NULL; lf=lf->next)
   j++;

return(max(k,j)+2); /** +2 just to make me feel happy. **/
}

