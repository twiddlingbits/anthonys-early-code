#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/** Close the requested module **/

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
if (argc > 1) {
	printf("usage: StudioClose\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't find studio.library\n");
	exit(10);
	}

CloseAllModules(FALSE);
CloseLibrary(StudioBase);
}
