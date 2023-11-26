

#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include "desk.h"


loadtext (olen,ostring,filename)
int *olen;
char **ostring,*filename;
{

int *f,j,templen2,templen3,i;
char *string2,*string3;


f = Open(filename,1005);
if (f == NULL)
   return(FILENOTFOUND);

j = Seek(f,0L,1);
j = Seek(f,0L,-1);

string2 = AllocMem(j+1,MEMF_CLEAR);    /** textconv frees len + 1 **/
if (string2 == NULL)
   return(OUTOFMEM);

Read(f,string2,j);
i = textconv(string2,j,&string3,&templen3);
if (i != AOK) {
   FreeMem(string2,j+1);
   return(OUTOFMEM);
   }
string2 = AllocMem(templen3 + 10000,MEMF_CLEAR | MEMF_CHIP);
if (string2 == NULL)
   return(OUTOFMEM);
movmem(string3,string2,templen3);
FreeMem(string3,templen3);
FreeMem(*ostring,*olen);
*ostring = string2;
*olen = templen3 + 10000;
*(*ostring + templen3) = 0;
Close(f);
return(AOK);

}



savetext (len,string,filename)
int len;
char *string,*filename;
{

int *f,j;
char *string2;

f = Open(filename,1006);
if (f == NULL)
   return(FILENOTFOUND);

Close(f);
return(AOK);

}


