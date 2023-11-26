#include "exec/types.h"
#include "desk.h"
#include "text.h"


/*********************************************************************/
/* val = get_fixed_pt(cp, units, def_units);                         */
/*                                                                   */
/* Gets a single fixed-point number given a double pointer to a      */
/* string.  Creates its on macro_text.  This routine is only for     */
/* compatibility with old stuff.  New routines should use their own  */
/* macro_text structs.  Units indicates whether coordinates are X    */
/* coords, Y coords, or old coords.                                  */
/*********************************************************************/

FIXED get_fixed_pt(cp, units,def_units)
char **cp;
int units;                    /* NUM_COORD_X, NUM_COORD_Y, or NUM_COORD */
int def_units;    /** NUM_DEF_INCH, NUM_DEF_POINT **/
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

val = evaluate_expr(&m, def_units, &sign, NULL, units);

end_macro(m);

return val;

}

#if 0
new_get_num(cp, def_units)
char **cp;
int def_units;
{
struct macro_text mt;
struct macro *m, *start_macro();
FIXED val;
SFIXED evaluate_expr();
int sign, pts;

mt.start = *cp;
mt.end = *cp + strlen(*cp);
mt.ref_count = 0;
mt.flags = 0;

m = start_macro(NULL, &mt, NULL);

val = evaluate_expr(&m, def_units, &sign);

end_macro(m);

pts = val>>16;

/* printf("IN GET_FIXED_PT: pts=%d\n",pts); */

return pts;

}
#endif

