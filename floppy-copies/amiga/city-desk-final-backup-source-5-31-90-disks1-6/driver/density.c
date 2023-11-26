/*
   Density module for City Desk Gray Scale Conversion Driver
   Anthony Wood June 1988
*/

#include <exec/types.h>
#include "include/devices/printer.h"
#include "include/devices/prtbase.h"

#include "cd_driver.h"

struct cd_port *cdp;

SetDensity(density_code)
ULONG density_code;
{
   extern struct PrinterExtendedData *PED;

   cdp = FindPort("city_desk_port");

   PED->ped_MaxXDots  = cdp->MaxXDots;
   PED->ped_MaxYDots  = cdp->MaxYDots;
   PED->ped_XDotsInch = cdp->XDotsInch;
   PED->ped_YDotsInch = cdp->YDotsInch;
}

