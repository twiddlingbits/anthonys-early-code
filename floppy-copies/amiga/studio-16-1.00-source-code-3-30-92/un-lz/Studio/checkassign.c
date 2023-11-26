#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/filehandler.h>

/*****************************************************************/

extern struct DosLibrary *DOSBase;

/*** Returns TRUE if string is a valid assign statment ***/

int CheckAssign(string)
char *string;
{
LONG blen;
struct DeviceNode *dn;
struct DosInfo *di;
struct RootNode *root;
UBYTE *bstring;
int Flag=0;

root = (struct RootNode *)DOSBase->dl_Root;
di = (struct DosInfo *)BADDR(root->rn_Info);
dn = (struct DeviceNode *) BADDR(di->di_DevInfo);

Forbid();
for (; dn != 0 && !Flag ; dn = (struct DeviceNode *)BADDR(dn->dn_Next))
	{
	bstring = (UBYTE *)BADDR(dn->dn_Name);
	blen = (LONG)bstring[0];
	if((stricmp(&bstring[1],string)==0) && (!Flag)) 
		{
		Flag++;
		}
 	}
Permit();

return(Flag);
}

