/*###################################################################*/
/* set_default_state.c                                                 */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

/*********************************************************************/
/* set_default_state(&state, text);                                  */
/*                                                                   */
/* Given a starting state and a string, this routine interprets the  */
/* string and returns the state changed to reflect any commands in   */
/* the string.                                                       */
/*********************************************************************/

void set_default_state(state, text)
struct text_state *state;
char *text;
{
char ch;
struct text_ctrl tc;
struct macro_text mt;
struct macro *start_macro();

tc.state = *state;            /* Copy initial text state to use */

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
/* tc.unget_active = FALSE; bobby took this out and added below */
tc.unget_okay = FALSE;
tc.count = 0;

mt.start = text;
mt.end = text + strlen(text);
mt.ref_count = 0;
mt.flags = 0;

tc.m = start_macro(NULL, &mt, NULL);

ch = 'a';               /* anything non-0 */
while (ch != '\0')
   ch = get_ch(&tc);

*state = tc.state;                     /* copy state back to caller */
discard_text_ctrl(&tc);
}
