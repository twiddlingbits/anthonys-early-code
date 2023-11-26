#include <exec/types.h>
#include <exec/exec.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

/************************************************************/

void dprintf(str)
char *str;
{
int fp;

fp = Open("*",MODE_NEWFILE);
if (fp) {
	Write(fp,str,strlen(str));
	Close(fp);
	}
}
