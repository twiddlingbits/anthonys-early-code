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
char line[60];

switch (err) {
   case NO_ERROR:
      break;
   case CANT_OPEN_FILE:
      telluser("Can't open the requested file.",0,0);
      break;
   case WRITE_ERROR:
      telluser("Error while writing to file.",0,0);
      break;
   case READ_ERROR:
      telluser("Error while reading from file",0,0);
      break;
   case DISK_ERROR:
      telluser("Disk error while accessing file",0,0);
      break;
   case OUT_OF_MEMORY:
      telluser("Not enough free memory to","complete the operation!",0);
      break;
   case NOT_8SVX:
      telluser("File type is IFF, but not","8SVX (8 bit sound format)",0);
      break;
   case BAD_IFF:
      telluser("This file is not in the format","I expect of IFF files.",0);
      break;

	case DUPLICATE_NAME:
		telluser("File name is already opened",0,0);
   	break;
		

	case STDERR_CANT_OPEN_SCREEN:
		telluser("Can't open screen",0,0);
		break;
	
	case STDERR_CANT_OPEN_WINDOW:
		telluser("Can't open window",0,0);
		break;

	case STDERR_CANT_GET_WRITE_LOCK:
		telluser("Can't open file for write access",0,0);
		break;
	
	case STDERR_UNKNOWN_FILE_FORMAT:
		telluser("I don't recognize this file's format.",0,0);
		break;

	case STDERR_CANT_GET_READ_LOCK:
		telluser("I can't open this file","for read access",0);
		break;

	case STDERR_CANT_DO_RENAME_TO_TEMP:
		telluser("I can't rename the file into the","working directory.",0);
		break;

	case STDERR_CANT_DO_RENAME_FROM_TEMP:
		telluser("I can't rename the file into the","destination directory.",0);
		break;

	case STDERR_CANT_FIND_COPYBUFFER:
		telluser("You must first do a 'Destructive Copy'",
			"(Can't Find 'CopyBuffer')",0);
		break;

	case STDERR_SAMPLE_TOO_SMALL:
		telluser("Can't perform edit:","The resulting sample is too small.",0);
		break;

	case STDERR_COPYBUFFER_IN_USE:
		telluser("Can't create CopyBuffer.","Sample in use.",0);
		break;

/**
	case :
		telluser("","",0);
		break;
***/

   default:
		sprintf(line,"undefined error #%d occured",err);
      telluser("ERROR!",line,0);
   }
}


