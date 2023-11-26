#include "exec/types.h"
#include "desk.h"
#include "text.h"


/*********************************************************************/
/* val = get_fixed_pt(cp);                                           */
/*                                                                   */
/* Gets a single fixed-point number given a double pointer to a      */
/* string.  Creates its on macro_text.  This routine is only for     */
/* compatibility with old stuff.  New routines should use their own  */
/* macro_text structs.                                               */
/*********************************************************************/

FIXED get_fixed_pt(cp)
char **cp;
{
struct macro_text mt;
struct macro *m, *start_macro();
FIXED val;
SFIXED evaluate_expr();
int sign;

mt.start = *cp;
mt.end = *cp + strlen(*cp);
mt.ref_count = 0;
mt.flags = 0;

m = start_macro(NULL, &mt, NULL);

val = evaluate_expr(&m, NUM_DEF_POINT, &sign);

end_macro(m);

return val;

}

