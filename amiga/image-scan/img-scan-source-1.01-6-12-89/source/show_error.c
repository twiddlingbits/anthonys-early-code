#include "exec/types.h"
#include "scan.h"

void show_error(stat)
int stat;
{
extern struct Screen *is_screen;

switch(stat) {

   case OUTOFMEM:
      telluser2(is_screen,"Out of Memory","",FALSE);
      break;

   case DISKERROR:
      telluser2(is_screen,"Disk I/O ERROR!","",FALSE);
      break;

   case CANTOPENFILE:
      telluser2(is_screen,"Sorry:","Can't Open File",FALSE);
      break;

   case WRONGFILETYPE:
      telluser2(is_screen,"Warning: File not","correct format",FALSE);
      break;

   case BADIFF:
      telluser2(is_screen,"Can't load","this IFF file",FALSE);
      break;

   }
}

