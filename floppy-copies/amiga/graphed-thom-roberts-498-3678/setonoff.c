#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"


/*************************************/
/*** set printer submenu checkmark ***/
/*************************************/
void settypeflag()
{
extern struct printerprefs printerprefs;
extern struct MenuItem SubItems[];
extern FIXED cpertextpixx,cpertextpixy;
extern UWORD xinch,yinch;

cpertextpixx=(576<<16)/xinch;
cpertextpixy=(576<<16)/yinch;
SubItems[4].Flags &=(~CHECKED);
SubItems[5].Flags &=(~CHECKED);
SubItems[6].Flags &=(~CHECKED);

if (printerprefs.printertype==PREFERENCES) {
   SubItems[4].Flags |=CHECKED;
   cpertextpixx=(576<<16)/xinch;
   cpertextpixy=(576<<16)/yinch;
   }

if (printerprefs.printertype==HPLASER) {
   SubItems[5].Flags |=CHECKED;
   cpertextpixx=(576<<16)/150;
   cpertextpixy=(576<<16)/150;
   }

if (printerprefs.printertype==POSTSCRIPT) {
   SubItems[6].Flags |=CHECKED;
   cpertextpixx=(576<<16)/300;
   cpertextpixy=(576<<16)/300;
   }
}

/**************************************************************/
/*** set all the submenus that are tied to a global variable **/
/**************************************************************/

setonoff()
{

extern SHORT screensize;
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT snaptogrid;
extern SHORT displaygrid;
extern SHORT snaptotop;
extern SHORT autoflow;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern struct MenuItem SubItems[];

SubItems[0].Flags &= (~CHECKED);    /* 640 X 200 */
SubItems[1].Flags &= (~CHECKED);    /* 640 X 400 */
SubItems[11].Flags &= (~CHECKED);   /* 1 CR */
SubItems[12].Flags &= (~CHECKED);   /* 2 CR */
SubItems[13].Flags &= (~CHECKED);   /* strip space on */
SubItems[14].Flags &= (~CHECKED);   /* stip spave off */
SubItems[15].Flags &= (~CHECKED);   /* Amiga */
SubItems[16].Flags &= (~CHECKED);   /* ASCII */
SubItems[17].Flags &= (~CHECKED);   /* Scrible */
SubItems[18].Flags &= (~CHECKED);   /* grid snap On */
SubItems[19].Flags &= (~CHECKED);   /* grid snap Off */
SubItems[20].Flags &= (~CHECKED);   /* grid display on */
SubItems[21].Flags &= (~CHECKED);   /* grid off */
SubItems[28].Flags &= (~CHECKED);   /* Snap to top On */
SubItems[29].Flags &= (~CHECKED);   /* snap off */
SubItems[30].Flags &= (~CHECKED);   /* Auto Flow on */
SubItems[31].Flags &= (~CHECKED);   /* Auto Flow off */

if (screensize==200)
   SubItems[0].Flags |= CHECKED;    /* 640 X 200 */
else
   SubItems[1].Flags |= CHECKED;    /* 640 X 400 */

if (!double_newlines)
   SubItems[11].Flags |= CHECKED;   /* 1 CR */
else 
   SubItems[12].Flags |= CHECKED;   /* 2 CR */

if (strip_spaces)
   SubItems[13].Flags |= CHECKED;   /* strip space on */
else 
   SubItems[14].Flags |= CHECKED;   /* stip spave off */

if (ansi_escapes) {
   SubItems[15].Flags |= CHECKED;   /* Amiga notepad */
   }

if (real_ascii) {
   SubItems[16].Flags |= CHECKED;   /* ASCII */
   }

if (scribble_style) {
   SubItems[17].Flags |= CHECKED;   /* Scrible */
   }

if (snaptogrid)
   SubItems[18].Flags |= CHECKED;   /* grid snap On */
else
   SubItems[19].Flags |= CHECKED;   /* grid snap Off */

if (displaygrid)
   SubItems[20].Flags |= CHECKED;   /* grid display on */
else
   SubItems[21].Flags |= CHECKED;   /* grid off */

if (snaptotop) 
   SubItems[28].Flags |= CHECKED;   /* Snap to top On */
else
   SubItems[29].Flags |= CHECKED;   /* snap off */

if (autoflow)
   SubItems[30].Flags |= CHECKED;   /* Auto Flow on */
else
   SubItems[31].Flags |= CHECKED;   /* Auto Flow off */
}


/** Ghost Save... **/

ghost_save()

{
extern struct MenuItem Items[];

Items[2].Flags &= (~ITEMENABLED);
}

unghost_save()

{
extern struct MenuItem Items[];

Items[2].Flags |= ITEMENABLED;
}

