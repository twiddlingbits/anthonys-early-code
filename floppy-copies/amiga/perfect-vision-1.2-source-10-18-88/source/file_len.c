#include "fcntl.h"

file_len (fname)
char *fname;

{
long f,len;

f = open(fname,O_RDONLY,S_IREAD);
if (f==-1)
   return(0);
len = lseek(f,0L,2L);
close (f);
return(len);
}



