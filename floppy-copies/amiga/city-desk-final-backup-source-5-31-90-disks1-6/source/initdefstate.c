#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"

initdefaultstate()

{
extern struct text_state defaultstate;
extern struct printerprefs printerprefs;
extern char default_font_name[40];

setmem (&defaultstate, sizeof(struct text_state), 0);
initdefaultstate2();

/** set up the default printer options **/

printerprefs.dotsperinch=75;
printerprefs.flags |= STRIP_PRINT;

strcpy(default_font_name,"topaz.font");  /** default fontname **/
}


initdefaultstate2()

{
extern struct text_state defaultstate;
extern UWORD xinch, yinch;

defaultstate.leading=(yinch*15)/100;                  /* 11 point */
defaultstate.max_space_stretch = (xinch*17)/100;      /* 1 pica */
defaultstate.autohyph_threshhold = xinch/2;   /* .5 inches (3 picas) */
defaultstate.magx = 1<<16;
defaultstate.magy = 1<<16;
defaultstate.escstart = '[';
defaultstate.escend = ']';
defaultstate.softhyphen = '~';
defaultstate.tabchar = '@';
defaultstate.tiechar = '_';
defaultstate.paramstart = '{';
defaultstate.paramend = '}';
defaultstate.font_size=64;   /* topaz 8 */
defaultstate.irreg_margin = xinch/16;
defaultstate.irreg_minwidth = xinch/3;
}

