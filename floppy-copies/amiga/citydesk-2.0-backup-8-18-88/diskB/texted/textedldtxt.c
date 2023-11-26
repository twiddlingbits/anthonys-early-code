

#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include "desk.h"


textedloadtext (olen,ostring,filename)
int *olen;
char **ostring,*filename;
{

int *f,j;
char *string2;


f = Open(filename,1005);
if (f == NULL)
   return(FILENOTFOUND);

j = Seek(f,0L,1);
j = Seek(f,0L,-1);

string2 = AllocMem(j + 10000,MEMF_CHIP);
if (string2 == NULL)
   return(OUTOFMEM);

FreeMem(*ostring,*olen);
*ostring = string2;
*olen = j + 10000;
Read(f,*ostring,j);
*(*ostring + j) = 0;
Close(f);
return(AOK);

}



textedsavetext (len,string,filename)
int len;
char *string,*filename;
{

int *f,j;
char *string2;

f = Open(filename,1006);
if (f == NULL)
   return(FILENOTFOUND);

printf("written = %d\n",Write(f,string,len));
Close(f);
return(AOK);

}


