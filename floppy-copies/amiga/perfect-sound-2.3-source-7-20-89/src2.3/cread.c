#include "exec/types.h"
#include "stdio.h"

/****************************/
/* compresed data read    ***/
/****************************/
cread(fp,data,len)
int fp;
register int len;
register char *data;
{
BYTE x,d;
register LONG i,j;
BYTE buf[512];
static short delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

read(fp,&d,1);   /* skip padding */
read(fp,&x,1);   /* get inital value */

len=len-2;
if (read(fp,buf,min(512,len))==-1) return(-1);
j=0;
for (i=0;i<len;i++) {
   *data=x+delta[15&(buf[j]>>4)];
   x=*data++;
   *data=x+delta[buf[j++]&15];
   x=*data++;
   if (j==512) {
      j=0;
      if (read(fp,buf,min(512,(len-i-1)))==-1) return(-1);
      }
   }
return(0);
}


