#include "exec/types.h"
#include "vid.h"


void set_cs_bits(mode)

int mode;

{
static UBYTE *data,*direction;
extern short swap_cs_bits;

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;

*direction=128+64;           /* bits 7 & 6 write, rest read **/

switch (mode) {
   case RED:
      if (swap_cs_bits)
         *data = 64;
      else
         *data = 128;
      break;

   case BLUE:
      if (swap_cs_bits)
         *data = 128;
      else
         *data = 64;
      break;

   case GREEN:
      *data = 0;
      break;
   }
}



