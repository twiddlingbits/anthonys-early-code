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
struct Task *FindTask(),*task=FindTask(NULL);
char *name=task->tc_Node.ln_Name;
int flags=0;

if (!strcmp(name,"gadlibtask")) 
	{
	flags=NOTICE_RETURN_NOW;
	telluser("PROGRAMMER WARNING!","You should not use show_error() from the gadlibtask",flags);
	}

switch (err) {
	case NO_ERROR:
	break;

	case CANT_OPEN_FILE:
		telluser(name,"Can't open the requested file.",flags);
	break;

	case WRITE_ERROR:
		telluser(name,"Error while writing to file.",flags);
	break;

	case READ_ERROR:
		telluser(name,"Error while reading from file",flags);
	break;

	case DISK_ERROR:
		telluser(name,"Disk error while accessing file",flags);
	break;

	case OUT_OF_MEMORY:
		telluser(name,"Not enough free memory to complete the operation!",flags);
	break;

	case NOT_8SVX:
		telluser(name,"File type is IFF, but not 8SVX (8 bit sound format)",flags);
	break;

	case BAD_IFF:
		telluser(name,"This file is not in the format I expect of IFF files.",flags);
	break;

	case DUPLICATE_NAME:
		telluser(name,"File name is already opened",flags);
	break;

	case STDERR_CANT_OPEN_SCREEN:
		telluser(name,"Can't open screen",flags);
	break;

	case STDERR_CANT_OPEN_WINDOW:
		telluser(name,"Can't open window",flags);
	break;

	case STDERR_CANT_GET_WRITE_LOCK:
		telluser(name,"Can't open file for write access",flags);
	break;

	case STDERR_UNKNOWN_FILE_FORMAT:
		telluser(name,"I don't recognize this file's format.",flags);
	break;

	case STDERR_CANT_GET_READ_LOCK:
		telluser(name,"I can't open this file for read access",flags);
	break;

	case STDERR_CANT_DO_RENAME_TO_TEMP:
		telluser(name,"I can't rename the file into the working directory.",flags);
	break;

	case STDERR_CANT_DO_RENAME_FROM_TEMP:
		telluser(name,"I can't rename the file into the destination directory.",flags);
	break;

	case STDERR_SINGLE_RESIDENT:
		telluser(name,"Cant add Module, Only one of these modules may be resident.",flags);
	break;

	case STDERR_NO_TAG:
		telluser(name,"ERROR: No Tag",flags);
	break;

	case STDERR_CANT_CREATE_SAMPLE:
		telluser(name,"Can not create sample.",flags);
	break;

	case STDERR_CANT_OPEN_SAMPLE:
		telluser(name,"Can not open sample",flags);
	break;

	case STDERR_CANT_FIND_COPYBUFFER:
		telluser(name,"You must first do a 'Destructive Copy' (Can't Find 'CopyBuffer')",flags);
	break;

	case STDERR_DATA_NOT_AVAILABLE_YET:
		telluser(name,"Data not available yet.",flags);
	break;

	case STDERR_DATA_EOF:
		telluser(name,"End of file",flags);
	break;

	case STDERR_SAMPLE_TOO_SMALL:
		telluser(name,"Can't perform edit,The resulting sample is too small.",flags);
	break;

	case STDERR_COPYBUFFER_IN_USE:
		telluser(name,"Can't create CopyBuffer. Sample in use.",flags);
	break;

	case STDERR_NO_MORE_CLIPS:
		telluser(name,"This sample has too many edits in it. Use 'Make Edits Permanent'.",flags);
	break;

	case STDERR_NO_STUDIO16_ASSIGN:
		telluser(name,"File 's:Studio16Path.config' is missing. No Studio16: Assigned.",flags);
	break;

	case STDERR_BAD_STUDIO16_ASSIGN:
		telluser(name,"File 'S:Studio16Path.config' or Studio16: points to wrong path.",flags);
	break;

	case STDERR_CANT_DELETE_FILE:
		telluser(name,"Error trying to delete requested file.",flags);
	break;

	case STDERR_CANT_FIND_REGION:
		telluser(name,"Can't find requested region.",flags);
	break;

	case STDERR_CANT_ADD_REGION:
		telluser(name,"File format doesn't support regions, or no region space left.",flags);
	break;

	case STDERR_CANT_FIND_SAMPLE:
		telluser(name,"Can't find sample.",flags);
	break;

	case STDERR_COPYBUFFER_EMPTY:
		telluser(name,"Copybuffer is empty.",flags);
	break;

/**
	case :
		telluser(name,"",flags);
	break;
***/

	default:
		sprintf(line,"undefined error #%d occured",err);
		telluser(name,line,flags);
   }
}


