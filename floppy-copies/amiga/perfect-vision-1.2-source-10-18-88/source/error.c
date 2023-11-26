#include "exec/types.h"
#include "vid.h"

show_error(stat)
int stat;
{
extern struct Screen *menu_screen;

pop_up();

switch(stat) {

   case OUTOFMEM:
      telluser2(menu_screen,"Out of Memory","",FALSE);
      break;

   case DISKERROR:
      telluser2(menu_screen,"Disk I/O ERROR!","",FALSE);
      break;

   case CANTOPENFILE:
      telluser2(menu_screen,"Sorry:","Can't Open File",FALSE);
      break;

   case WRONGFILETYPE:
      telluser2(menu_screen,"Warning: File not","correct format",FALSE);
      break;

   case BADIFF:
      telluser2(menu_screen,"ERROR:","Not a good IFF file",FALSE);
      break;

   case NOFRAMES:
      telluser2(menu_screen,"No Frames Grabbed!","",FALSE);
      break;
   case ONLY_ONE_FRAME:
      telluser2(menu_screen,"You only have one frame!","",FALSE);
      break;
   }
pop_down();
}

