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
extern FIXED cpertextpixx,cpertextpixy;
extern UWORD xinch,yinch;

if (printerprefs.printertype==PREFERENCES) {
/*   SubItems[4].Flags |=CHECKED; */
   }

if (printerprefs.printertype==HPLASER) {
/*   SubItems[5].Flags |=CHECKED;     */
   xinch = 300;
   yinch = 300;
   }

if (printerprefs.printertype==POSTSCRIPT) {
/*   SubItems[6].Flags |=CHECKED; */
   printerprefs.dotsperinch=300;
   xinch = 300;
   yinch = 300;
   }

if (printerprefs.printertype==VPAGE) {
   printerprefs.dotsperinch=300;
   xinch = 300;
   yinch = 300;
   }

/* cpertextpixx=(576<<16)/xinch;
cpertextpixy=(576<<16)/yinch; */

cpertextpixx=1<<16;
cpertextpixy=1<<16;
}

/**************************************************************/
/*** set all the submenus that are tied to a global variable **/
/**************************************************************/

setonoff()
{
#if 0
extern struct printerprefs printerprefs;
extern SHORT screensize;
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT wp_style;
extern SHORT displaygrid;
extern SHORT snaptogrid;
extern SHORT snaptotop;
extern SHORT autoflow;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT flow_warnings;
extern SHORT usedefault;
extern SHORT displayguides;
extern struct MenuItem SubItems[];

SubItems[0].Flags &= (~CHECKED);    /* 640 X 200 */
SubItems[2].Flags &= (~CHECKED);    /* guides visible */
SubItems[3].Flags &= (~CHECKED);    /* invisable */
SubItems[1].Flags &= (~CHECKED);    /* 640 X 400 */
SubItems[7].Flags &= (~CHECKED);    /* 75 dpi */
SubItems[8].Flags &= (~CHECKED);    /* 100 dpi */
SubItems[9].Flags &= (~CHECKED);    /* 150 dpi */
SubItems[10].Flags &= (~CHECKED);   /* 300 dpi */
SubItems[11].Flags &= (~CHECKED);   /* 1 CR */
SubItems[12].Flags &= (~CHECKED);   /* 2 CR */
SubItems[13].Flags &= (~CHECKED);   /* strip space on */
SubItems[14].Flags &= (~CHECKED);   /* stip spave off */
SubItems[15].Flags &= (~CHECKED);   /* Amiga */
SubItems[16].Flags &= (~CHECKED);   /* ASCII */
SubItems[17].Flags &= (~CHECKED);   /* Scrible */
SubItems[18].Flags &= (~CHECKED);   /* wordperfect */
SubItems[19].Flags &= (~CHECKED);   /* grid snap On */
SubItems[20].Flags &= (~CHECKED);   /* grid snap Off */
SubItems[21].Flags &= (~CHECKED);   /* grid display on */
SubItems[22].Flags &= (~CHECKED);   /* grid size */
SubItems[23].Flags &= (~CHECKED);   /* grid size */
SubItems[24].Flags &= (~CHECKED);   /* grid size */
SubItems[25].Flags &= (~CHECKED);   /* grid size */
SubItems[26].Flags &= (~CHECKED);   /* grid size */
SubItems[27].Flags &= (~CHECKED);   /* grid size */
SubItems[28].Flags &= (~CHECKED);   /* grid size */
SubItems[29].Flags &= (~CHECKED);   /* Snap to top On */
SubItems[30].Flags &= (~CHECKED);   /* snap off */
SubItems[31].Flags &= (~CHECKED);   /* Auto Flow on */
SubItems[32].Flags &= (~CHECKED);   /* Auto Flow off */
SubItems[33].Flags &= (~CHECKED);   /* Reflow warnings on */
SubItems[34].Flags &= (~CHECKED);   /* Reflow warnings off */
SubItems[35].Flags &= (~CHECKED);   /* Reflow uses default state */
SubItems[36].Flags &= (~CHECKED);   /* Reflow uses same start state */

if (screensize==200)
   SubItems[0].Flags |= CHECKED;    /* 640 X 200 */
else
   SubItems[1].Flags |= CHECKED;    /* 640 X 400 */

if (displayguides)
   SubItems[2].Flags |= CHECKED;    /* visable */
else
   SubItems[3].Flags |= CHECKED;    /* invisable */

if (printerprefs.dotsperinch == 75)
   SubItems[7].Flags |= CHECKED;    /* HP 75 dpi */
else if (printerprefs.dotsperinch == 100)
   SubItems[8].Flags |= CHECKED;    /* HP 100 dpi */
else if (printerprefs.dotsperinch == 150)
   SubItems[9].Flags |= CHECKED;    /* HP 150 dpi */
else
   SubItems[10].Flags |= CHECKED;    /* HP 300 dpi */

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

if (wp_style) {
   SubItems[18].Flags |= CHECKED;   /* wordperfect */
   }

if (snaptogrid)
   SubItems[19].Flags |= CHECKED;   /* grid snap On */
else
   SubItems[20].Flags |= CHECKED;   /* grid snap Off */

if (displaygrid)
   SubItems[21].Flags |= CHECKED;   /* grid display on */
else
   SubItems[22].Flags |= CHECKED;   /* grid off */

if (snaptotop) 
   SubItems[29].Flags |= CHECKED;   /* Snap to top On */
else
   SubItems[30].Flags |= CHECKED;   /* snap off */

/* if (gridsize == 8)
   SubItems[23].Flags |= CHECKED;
else if (gridsize == 48)
   SubItems[24].Flags |= CHECKED;
else if (gridsize == 96)
   SubItems[25].Flags |= CHECKED;
else if (gridsize == 144)
   SubItems[26].Flags |= CHECKED;
else if (gridsize == 288)
   SubItems[27].Flags |= CHECKED;
else
   SubItems[28].Flags |= CHECKED; */

if (autoflow)
   SubItems[31].Flags |= CHECKED;   /* Auto Flow on */
else
   SubItems[32].Flags |= CHECKED;   /* Auto Flow off */

if (flow_warnings)
   SubItems[33].Flags |= CHECKED;   /*  on */
else
   SubItems[34].Flags |= CHECKED;   /* off */

if (usedefault)
   SubItems[35].Flags |= CHECKED;   /* use default state */
else
   SubItems[36].Flags |= CHECKED;   /* use same starting state */
#endif
}


/** Ghost Save... **/

ghost_save()

{
extern struct MenuItem Items[];

Items[3].Flags &= (~ITEMENABLED);
}

unghost_save()

{
extern struct MenuItem Items[];

Items[3].Flags |= ITEMENABLED;
}

