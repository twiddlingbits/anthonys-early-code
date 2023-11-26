
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"


/*************************/
/* pull a string from disk */
/*************************/

memstring(fp)
FILE *fp;
{
char *poin;
char string[300];
int i;
i = 0;
do {
   fscanf(fp,"%c",&string[i]);
} while (string[i] != 13 && string[i] != 10 && string[i++] != EOF);
string[i] = 0;
poin = AllocMem(strlen(string) + 1,0);
movmem(string,poin,strlen(string) + 1);

return(poin);

}



