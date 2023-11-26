#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"
#include "laser.h"

void setdefport()
{
extern struct printerprefs printerprefs;
UBYTE amiga_pref[2];

GetPrefs(amiga_pref,2);    /** get 1st 2 bytes of amiga preferences */
if (amiga_pref[1] == SERIAL_PRINTER)
   printerprefs.flags |= USE_SER;
else
   printerprefs.flags |= USE_PAR;
}


