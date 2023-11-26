/* ps jr. test */


#include "stdio.h"

main()
{
unsigned char *pb,*dir;
int clk;
unsigned char bit,val;

dir=0xBFE301;
 pb=0xBFE101;

*pb=4;
*dir=128+64+32+2+4;

xxx:

val=0;
*pb=128+64+32;   /* minimum gain, cs* active */

   bit=0;bit=1;bit=0;bit=1;   /* delay */
for (clk=0;clk<8;clk++) {
   *pb=*pb|2;
   bit=(*pb)&1;
   *pb=(*pb)&(~2);
   val=val|(bit<<clk);
   }

*pb=*pb|4;  /* deselect chip */

printf("read %d %x\n",val,val);

for (clk=0;clk<10000;clk++) ;

goto xxx;
}
