#include "exec/types.h"

/**********************/
/**    skip a form    */
/**********************/

skip(in)
int in;

{
ULONG x;
read(in,&x,4);  /* get lenth */
lseek(in,x,1);  /* seek past it */
if ((x&1)==1)
   lseek(in,1L,1);  /* skip padding if odd */
}

