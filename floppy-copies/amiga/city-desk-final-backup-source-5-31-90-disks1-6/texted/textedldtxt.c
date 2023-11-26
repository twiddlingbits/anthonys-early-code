

#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include "desk.h"

/*   if flag = 0, this is for the page
     if flag !=0, this is for the cut      */

textedloadtext (olen,ostring,filename,flag)
int *olen,flag;
char **ostring,*filename;
{

int *f,j,j2,templen2,templen3,i;
char *string2,*string3;

f = Open(filename,1005);
if (f == NULL)
   return(FILENOTFOUND);

j = Seek(f,0L,1);
j = Seek(f,0L,-1);

if (flag == 0)
   j2 = 10000;
else
   j2 = 1;
string2 = AllocMem(j + j2,MEMF_CHIP);

if (string2 == NULL)
   return(OUTOFMEM);

Read(f,string2,j);
i = textconv(string2,j+j2-1,&string3,&templen3);
if (i != AOK) {
   FreeMem(string2,j+j2);
   return(OUTOFMEM);
   }
string2 = AllocMem(templen3 + j2,MEMF_CLEAR | MEMF_CHIP);
if (string2 == NULL)
   return(OUTOFMEM);
movmem(string3,string2,templen3);
FreeMem(string3,templen3);

if (*olen != 0 && *ostring != 0)
   FreeMem(*ostring,*olen);

*ostring = string2;
*olen = templen3 + j2;
*(*ostring + (templen3)) = 0;
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


