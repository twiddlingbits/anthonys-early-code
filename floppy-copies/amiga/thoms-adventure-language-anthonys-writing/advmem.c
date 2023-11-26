
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"


/*************************/
/* pull a list from disk */
/*************************/

memlist(fp,list)
FILE *fp;
char *list[MAXLIST];
{
int i;
char string[300];
i = 0;
jumpa:
   fscanf(fp,"%s\n",string);
   if (string != "zyxwvut") {
      list[i] = allocmem(strlen(string) + 1,MEM_FAST);
      movemem(string,list[i],strlen(string) + 1);
      ++i;
      goto jumpa;
      }

}


/*************************/
/* pull a string from disk */
/*************************/

memstring(fp,poin)
FILE *fp;
char *poin;
{
char string[300];
fscanf(fp,"%s\n",string);
poin = allocmem(strlen(string) + 1,MEM_FAST);
movemem(string,poin,strlen(string) + 1);

}



