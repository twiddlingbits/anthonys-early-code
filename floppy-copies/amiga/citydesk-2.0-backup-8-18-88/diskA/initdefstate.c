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
defaultstate.leading=88;                   /* 11 point */
defaultstate.max_space_stretch = 96;      /* 1 pica */
defaultstate.magx = 1<<16;
defaultstate.magy = 1<<16;
defaultstate.escstart = '[';
defaultstate.escend = ']';
defaultstate.softhyphen = '~';
defaultstate.tabchar = '@';
defaultstate.tiechar = '_';
defaultstate.font_size=64;   /* topaz 8 */

/** set up the default printer options **/

printerprefs.dotsperinch=75;

strcpy(default_font_name,"topaz.font");  /** default fontname **/
}

