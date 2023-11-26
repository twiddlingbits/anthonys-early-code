
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"



/*****************/
/* print a list  */
/*****************/

printlist(list)
char list[MAXLIST][ITEMSPACE];
{
int i;
i = 1;
printf("\nNew list:\n");
while  (list[i][0]!=0) {
   printf("%s\n",list[i]);
   i++;  }
}

/***********************/
/* dump a list to disk */
/***********************/

filelist(fp,list)
FILE *fp;
char list[MAXLIST][ITEMSPACE];
{
int i;
i = 1;
while  (list[i][0]!=0) {
   fprintf(fp,"%s\n",list[i]);
   i++;  }
fprintf(fp,"zyxwvut\n");

}


