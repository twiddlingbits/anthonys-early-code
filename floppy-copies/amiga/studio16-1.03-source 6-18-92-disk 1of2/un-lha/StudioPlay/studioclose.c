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
struct Module *mod;

if (argc!=2) {
	printf("usage: StudioClose <module name>\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't find studio.library\n");
	exit(10);
	}

mod=(struct Module *)FindModule(argv[1]);
if (mod) {
	SendMsg(mod, GetEventID("SE_CMD_KILL_MODULE"),mod,0,0,0,0,0);
	}
else {
	printf("Module '%s'is not loaded\n",argv[1]);
	}

CloseLibrary(StudioBase);	/** Close our open **/
}
