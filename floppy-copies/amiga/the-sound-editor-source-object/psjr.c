/* ps jr. test */


#include "stdio.h"

main()
{
unsigned char *pb,*dir;
int clk,i;
unsigned char bit,val;

dir=0xBFE301;
 pb=0xBFE101;

*pb=4;
*dir=0x0F6;

xxx:

val=0;
*pb=128+64+32+4;   /* minimum gain, cs* not active */
for (i=0;i<5000;i++);  /* delay */
*pb=128+64+32;   /* minimum gain, cs* active */
for (i=0;i<200;i++);  /* delay */

for (clk=0;clk<8;clk++) {
   *pb=*pb|2;
   *pb=(*pb)&(~2);
   bit=(*pb)&1;
   val=val|(bit<<(7-clk));
   }

*pb=*pb|4;  /* deselect chip */

printf("read %d %x\n\n",val,val);

for (clk=0;clk<50000;clk++) ;

goto xxx;
}
