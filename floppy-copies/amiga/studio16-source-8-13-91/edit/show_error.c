#include "/include/studio16.h"


/*############################################################
NAME  - show_error

FUNCTION
      given a standard studio16 error code, this calls telluser()
      with a description of the error
INPUTS
      error code as defined in studio16.h
RESULTS
   NONE
##############################################################*/

void show_error(err)
int err;
{
switch (err) {
   case NO_ERROR:
      break;
   case CANT_OPEN_FILE:
      telluser("Can't open the requested file.",0);
      break;
   case WRITE_ERROR:
      telluser("Error while writing to file.",0);
      break;
   case READ_ERROR:
      telluser("Error while reading from file","");
      break;
   case DISK_ERROR:
      telluser("Disk error while accessing file",0);
      break;
   case OUT_OF_MEMORY:
      telluser("Not enough free memory to","complete the operation!");
      break;
   case NOT_8SVX:
      telluser("File type is IFF, but not","8SVX (8 bit sound format)");
      break;
   case BAD_IFF:
      telluser("This file is not in the format","I expect of IFF files.");
      break;
   
   default:
      telluser("SHOW_ERROR:","undefined error occured");
   }
}


