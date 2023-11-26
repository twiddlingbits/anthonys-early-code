#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/** Studio 16 open comand.  Will Open studio.library and load **/
/** the requested module.  **/

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
struct Module *mod;

if (argc > 2) {
	printf("usage: StudioOpen <optional module name>\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

/** If this is the first time studio.library is opened, we **/
/** need to tell studio.library to go find all the working **/
/** samples in the audio: directory **/

InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);

if (argc==2) {
	mod=(struct Module *)LoadModule(argv[1]);
	if (mod==NULL) {
		printf("Error loading module '%s'\n",argv[1]);
		printf("usage: StudioOpen <optional module name>\n");
		CloseLibrary(StudioBase);
		exit(10);
		}
	}
CloseLibrary(StudioBase);	/** studio library is still open because of **/
}									/** utility modules loaded earlier **/
