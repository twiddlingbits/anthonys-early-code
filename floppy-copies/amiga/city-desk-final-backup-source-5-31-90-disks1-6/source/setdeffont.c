#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"

/*****************************************************************/
/** set the default font state to the font in default_font_name **/
/*****************************************************************/


extern struct text_state defaultstate;
extern char default_font_name[40];

setdeffont(screen)
struct Screen *screen;
{
defaultstate.font=NULL;
defaultstate.lfont=NULL;
my_open_font(default_font_name,&defaultstate,NULL);

if (defaultstate.font==NULL) {
  /* telluser2(screen,"Can't Open Font:",default_font_name,FALSE); */
   defaultstate.font=screen->RastPort.Font;   /** get known font **/
   strcpy(default_font_name,defaultstate.font->tf_Message.mn_Node.ln_Name);
   }
}


