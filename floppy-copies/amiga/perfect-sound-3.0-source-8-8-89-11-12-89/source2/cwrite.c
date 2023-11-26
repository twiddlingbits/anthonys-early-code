#include "exec/types.h"

/*******************************/
/** compressed write          **/
/*******************************/

cwrite(fp,data,len)
int fp,len;
BYTE *data;
{
WORD prev=0;
register int pererr=0;
register int i;
register int k,j;
int bpos;
char val;
BYTE buf[512];
static short delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

if (write(fp,&prev,1)==-1) return(-1);   /* padding */
prev=*data;
if (write(fp,data,1)==-1) return(-1);    /* 1st value */
i=len/2;
bpos=0;

for (k=0;k<i;k++) {
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=(val<<4)|j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;

   buf[bpos++]=val;
   if (bpos==512) {
      bpos=0;
      if (write(fp,buf,512)==-1) return(-1);
      }
   }
   if (bpos!=0)
      if (write(fp,buf,bpos)==-1) return(-1);
return(pererr*100/len);
}

