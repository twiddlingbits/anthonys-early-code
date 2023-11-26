/*
   City Desk 2.0 Gray Scale Conversion Driver
   Anthony Wood June 1988
*/

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include "include/devices/prtbase.h"
#include "include/devices/printer.h"

extern SetDensity();

Render(ct, x, y, status)
long ct, x, y, status;
{
   extern void *AllocMem(), FreeMem();

   extern struct PrinterData *PD;
   extern struct PrinterExtendedData *PED;

   static UWORD dataoffset=0;
   int err;


   err = PDERR_NOERR;

   switch(status) {
      case 0 : /* Master Initialization */
         /*
            ct   - pointer to IODRPReq structure (use with caution!).
            x   - width of printed picture in pixels.
            y   - height of printed picture in pixels.
         */

         PD->pd_PrintBuf = NULL;       /** I write staight to rastport **/
         break;

      case 1 : /* Scale, Dither and Render */
         /*
            ct   - pointer to PrtInfo structure.
            x   - NA
            y   - row.
         */
         Transfer(ct, y, &PD->pd_PrintBuf[dataoffset]);
         break;

      case 2 : /* Dump Buffer to Printer */
         /*
            ct   - NA.
            x   - NA.
            y   - # of rows sent (1 to PED->ped_NumRows).

            White-space strip.  Scan backwards through the buffer for
            the first non-zero data byte.  Send up to this point only.
         */
         break;

      case 3 : /* Clear and Init Buffer */
         /*
            ct   - NA.
            x   - NA.
            y   - NA.
         */
         break;

      case 4 : /* Close Down */
         /*
            ct   - error code.
            x   - io_Special flag from IODRPReq structure.
            y   - NA.
         */

        /*  flag that there is no alpha data waiting that needs
            a formfeed (since we just did one)
         */
         PED->ped_PrintMode = 0;
         break;

      case 5 : /* Pre-Master Initialization */
         /*
            ct   - NA.
            x   - io_Special flag from IODRPReq structure.
            y   - NA.
         */
         SetDensity(x & SPECIAL_DENSITYMASK); /* select density */
         break;
   }
   return(err);
}
