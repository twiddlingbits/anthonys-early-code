#include "exec/types.h"

UWORD *clxdat, *clxcon;

hit()
{
static UWORD oldval=0,x;


if ((x=*clxdat) != oldval) ;
/*   printf("%x\n",x); */

oldval = x;
if (oldval & 4)
   return(TRUE);
return(FALSE);
}

hit_setup()
{
clxdat = 0xdff00e;
clxcon = 0xdff098;
*clxcon = 65;           /** Notice sprite 2 collisions **/
}

