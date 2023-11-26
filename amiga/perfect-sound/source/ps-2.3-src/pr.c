#include "exec/types.h"

/****************************/
/** print a line to dos fp  */
/****************************/

pr(fp,string)

int fp;
char *string;
{
UBYTE newline=10;

Write(fp,string,strlen(string));
Write(fp,&newline,1);
}

