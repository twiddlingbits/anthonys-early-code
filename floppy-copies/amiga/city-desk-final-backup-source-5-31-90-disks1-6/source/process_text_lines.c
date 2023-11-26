/*###################################################################*/
/* process_text_lines                                                */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

/*********************************************************************/
/* process_text_lines                                                */
/*                                                                   */
/* Give a text object with text lines, generate text state structure */
/*********************************************************************/

void process_text_lines(tobj)
struct text_object *tobj;
{
char ch;
struct text_ctrl tc;

tc.x = 0;
tc.y = 0;
tc.maxw = 0;
tc.newx = 0;
tc.newy = 0;
tc.startx = 0;
tc.itkern = 0;
tc.ch = 0;
tc.prevch = 0;
tc.unget_macro = NULL;
tc.unget_okay = FALSE;
tc.count = 0;

tc.state = tobj->state;            /* Copy initial text state to use */
tc.m = copy_macro_chain(tobj->macro);

ch = 'a';               /* anything non-0 */
while (tc.count < tobj->endcount && ch != '\0')
   ch = get_ch(&tc);

tobj->endstate = tc.state;
tobj->endmacro = copy_macro_chain(tc.m);
discard_text_ctrl(&tc);
}
