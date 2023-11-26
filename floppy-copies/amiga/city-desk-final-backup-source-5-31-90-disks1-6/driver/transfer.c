/*
   Transfer routine for city desk driver
   Anthony Wood june 1988
*/

#include "exec/types.h"
#include "exec/exec.h"
#include "include/devices/prtgfx.h"
#include "cd_driver.h"

extern struct cd_port *cdp;

Transfer(PInfo, y, ptr)
struct PrtInfo *PInfo;
UWORD y;   /* row # */
UBYTE *ptr;   /* ptr to buffer */
{
   UBYTE *dmatrix, Black, dvalue, threshold;
   union colorEntry *ColorInt;
   UWORD x, width, sx, *sxptr;

   /* pre-compute */
   /* printer non-specific, MUST DO FOR EVERY PRINTER */
   x = PInfo->pi_xpos; /* get starting x position */
   ColorInt = PInfo->pi_ColorInt; /* get ptr to color intensities */
   sxptr = PInfo->pi_ScaleX;
   width = PInfo->pi_width; /* get # of source pixels */

   /* pre-compute threshold; are we thresholding? */
   if (threshold = PInfo->pi_threshold) { /* thresholding */
      dvalue = threshold ^ 15; /* yes, so pre-compute dither value */
      do { /* for all source pixels */
         /* pre-compute intensity value for Black */
         Black = ColorInt->colorByte[PCMBLACK];
         ColorInt++; /* bump ptr for next time */

         sx = *sxptr++;

         /* dither and render pixel */
         do { /* use this pixel 'sx' times */
            if (Black > dvalue) { /* if we should render Black */
               set_bit(x,y);
/*               *(ptr + (x >> 3)) |= bit_table[x & 7]; */ /* set bit */
            }
            ++x; /* done 1 more printer pixel */
         } while (--sx);
      } while (--width);
   }
   else { /* not thresholding, pre-compute ptr to dither matrix */
      dmatrix = PInfo->pi_dmatrix + ((y & 3) << 2);
      do { /* for all source pixels */
         /* pre-compute intensity value for Black */
         Black = ColorInt->colorByte[PCMBLACK];
         ColorInt++; /* bump ptr for next time */

         sx = *sxptr++;

         /* dither and render pixel */
         do { /* use this pixel 'sx' times */
            if (Black > dmatrix[x & 3]) { /* if we should render Black */
               set_bit(x,y);
/*               *(ptr + (x >> 3)) |= bit_table[x & 7];*/ /* set bit */
            }
            ++x; /* done 1 more printer pixel */
         } while (--sx);
      } while (--width);
   }
}

set_bit(x,y)
int x,y;
{
UBYTE *ptr;   /* ptr to buffer */
static UBYTE bit_table[] = {128, 64, 32, 16, 8, 4, 2, 1};

ptr = cdp->buf_start + ((unsigned short)y * (unsigned short)cdp->modulo);
x += cdp->x_off;
*(ptr + (x >> 3)) |= bit_table[x & 7]; /* set bit */
}

