/*###################################################################*/
/* fonts.c                                                           */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <libraries/diskfont.h>
#include "desk.h"
#include "text.h"
#include "laser.h"

#define MAX(a,b)  ((a)>(b)?(a):(b))

struct screenfontnode *firstscreenfont;

extern struct laserfont *firstlaserfont;
extern int plaser;
extern SHORT texterrors;
extern struct Window *clipboard;    /* for telluser2 */
extern struct symtab font_table;

/*********************************************************************/
/* init_fonts();                                                     */
/*                                                                   */
/* Initializes the open font chain and fills it with a list of all   */
/* available fonts.                                                  */
/*********************************************************************/

init_fonts()
{
struct AvailFonts *af;
struct AvailFontsHeader *afh;
extern int AvailFonts();
struct screenfontnode *sf;
int j;
int afsize, extrasize;
UBYTE fonttypes;
VOID add_symbol();

firstscreenfont = NULL;

afsize = 500;
fonttypes = 0xff;       /* show us all font types */
afh = (struct AvailFontsHeader *)AllocMem(afsize, MEMF_CLEAR);
if (afh == NULL)
   return(0);
extrasize = AvailFonts(afh, afsize, fonttypes);
if (extrasize) {           /* original buffer not big enough */
   FreeMem(afh, afsize);
   afsize += extrasize;
   afh = (struct AvailFontsHeader *)AllocMem(afsize, MEMF_CLEAR);
   if (afh == NULL)
      return(0);
   extrasize = AvailFonts(afh, afsize, fonttypes);
   if (extrasize)
      return(0);     /** This shouldn't happen. But, check just in case **/
}
af=(struct AvailFonts *)&afh[1]; /* bypass header to get first availfont */

for (j=0; j<afh->afh_NumEntries; j++, af++) {
   if ((af->af_Attr.ta_Flags & FPF_REMOVED) ||
       (af->af_Attr.ta_Flags & FPF_REVPATH) ||
       ((af->af_Type&AFF_MEMORY)&&(af->af_Attr.ta_Flags&FPF_DISKFONT)))
      ;     /* do nothing if font removed, reversed, or in both */
            /* disk and ram (don't list it twice) */
   else {
      sf = (struct screenfontnode *)
           AllocMem(sizeof(struct screenfontnode), MEMF_PUBLIC);
  /*    if (sf == NULL)
         printf("ERROR!! No Mem\n");  !!!*/
      if (sf) {
         sf->ta = af->af_Attr;
         sf->size = sf->ta.ta_YSize << 3;
         strncpy(sf->name, sf->ta.ta_Name, FONTNAMESIZE);
         sf->ta.ta_Name = sf->name;
         sf->font = NULL;
         sf->next = firstscreenfont;
         firstscreenfont = sf;
         sf->style = 0;
         if (sf->ta.ta_Style & FSF_ITALIC)
            sf->style |= ITALIC;
         if (sf->ta.ta_Style & FSF_BOLD)
            sf->style |= BOLD;
         if (sf->ta.ta_Style & FSF_UNDERLINED)
            sf->style |= UNDERLINE;
         add_symbol(&font_table, sf->name, 0);
      }
   }
}
FreeMem(afh, afsize);

}


/*********************************************************************/
/* close_fonts();                                                    */
/*                                                                   */
/* Closes all open fonts and deallocates the open font chain.        */
/*********************************************************************/

close_fonts()
{
struct screenfontnode *sf, *nextsf;
VOID free_symbol_table();

nextsf = NULL;
for (sf = firstscreenfont; sf != NULL; sf = nextsf) {
   if (sf->font)
      CloseFont (sf->font);
   nextsf = sf->next;
   FreeMem (sf, sizeof(struct screenfontnode));
}
free_symbol_table(&font_table);

}


/*********************************************************************/
/* my_open_font(fname, state, dum);                                  */
/*                                                                   */
/* Opens a font with name fname and with size and attributes as      */
/* specified in state.  Pointer to the font is put back in state.    */
/* Dum is no longer used (was pointer to RastPort).                  */
/*********************************************************************/

my_open_font(fname, state, dum)
char *fname;
struct text_state *state;
char *dum;
{
struct laserfont *laser_openfont();
struct TextFont *font, *screen_openfont();
extern short this_page;
char err[25];

/* Try to open a laser font first */

state->lfont = laser_openfont(fname, state->font_size, state->style);

if (state->lfont) {           /* found one */
   state->font = state->lfont->screenfont;
}
else {
   font = screen_openfont(fname, state->font_size, state->style);
   if (font) {
      state->font = font;           /* valid font */
      }
   else {
      if (texterrors) {
         sprintf(err,"%s (Pg %d)",fname,this_page);
         if (!telluser(clipboard->WScreen, "Invalid Font Name or Command",
                       err))
            texterrors = FALSE;        /* user hit cancel  */
      }
   }
}

/* set the fonts to the screen and printer */

set_font (state);
}


/*********************************************************************/
/* set_font (state);                                                 */
/*                                                                   */
/* If the current font is a laser font, set up the printer to use    */
/* that font.  Nothing needs to be done for amiga fonts.             */
/*********************************************************************/
set_font(state)
struct text_state *state;
{

/* Set the laser font to the printer */

if (plaser && state->lfont)
   laser_setfont(state->lfont);

}

/*********************************************************************/
/* lfont = laser_openfont (fname, size, style);                      */
/*                                                                   */
/* Attempt to open a laser font.  If successful, return a pointer to */
/* the laserfont structure.  If not, return NULL.                    */
/*********************************************************************/

struct laserfont *laser_openfont(fname, size, style)
char *fname;
COORD size;
UBYTE style;
{
struct laserfont *p, *closematch;
COORD closesize;
UBYTE closestyle;

if (firstlaserfont==NULL)
   return(NULL);

closematch = NULL;

for (p=firstlaserfont; p!=NULL; p=p->next) {

   if (strcmp(fname,p->fontname) == 0) {
      if (closematch == NULL) {              /* no prev name match */
         closematch = p;
         closesize = p->size;
         closestyle = p->style;
      }
      else {
         if (iabs(p->size-size) < iabs(closesize-size)) {
            closematch = p;
            closesize = p->size;
            closestyle = p->style;
         }
         else if (iabs(p->size-size) == iabs(closesize-size)) {
            if ((style == p->style) ||       /* exact match */
                ((p->style^style) < (closestyle^style))) { /* or closer match */
               closematch = p;
               closesize = p->size;
               closestyle = p->style;
            }
         }
      }
   }
}
return(closematch);
}

/*********************************************************************/
/* font = screen_openfont (name, size, style);                       */
/*                                                                   */
/* Attempt to open a screen font.  If successful, return a pointer   */
/* to the TextFont structure.  If not, return NULL.  A .font is      */
/* added to the name if there isn't one already.                     */
/*********************************************************************/

struct TextFont *screen_openfont(name, size, style)
char *name;
COORD size;
UBYTE style;
{
char fname[FONTNAMESIZE];
char *dum;
struct TextFont *font, *OpenFont(), *OpenDiskFont();
struct screenfontnode *p, *closematch;
COORD closesize;
UBYTE closestyle;

strncpy(fname, name, FONTNAMESIZE);
if (stcpm(fname, ".font", &dum) == 0)
   strncat(fname, ".font", FONTNAMESIZE-strlen(fname)-1);

closematch = NULL;
for (p=firstscreenfont; p!=NULL; p=p->next) {

   if (stricmp(fname,p->name) == 0) {
      if (closematch == NULL) {              /* no prev name match */
         closematch = p;
         closesize = p->size;
         closestyle = p->style;
      }
      else {
         if (iabs(p->size-size) < iabs(closesize-size)) {
            closematch = p;
            closesize = p->size;
            closestyle = p->style;
         }
         else if (iabs(p->size-size) == iabs(closesize-size)) {
            if ((style == p->style) ||       /* exact match */
                ((p->style^style) < (closestyle^style))) {    /* or closer match */
               closematch = p;
               closesize = p->size;
               closestyle = p->style;
            }
         }
      }
   }
}
if (closematch == NULL)
   return(NULL);              /* invalid font name */

if (closematch->font) {
   return(closematch->font);  /* font already open */
   }

if (closematch->ta.ta_Flags & FPF_DISKFONT) {
   font = OpenDiskFont(&closematch->ta);
   }
else  {
   font = OpenFont(&closematch->ta);
   if (font==0) {    /** This shouldn't happen. But does on Zuma fonts **/
      font = OpenDiskFont(&closematch->ta);
      }
   }
if (font == NULL)
   return(NULL);              /* whoops!  something strange is going on */

closematch->font = font;      /* font successfully opened */
return(font);

}

