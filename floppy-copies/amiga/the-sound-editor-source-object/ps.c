/* ps jr. test */


#include "stdio.h"

main()
{
unsigned char *pa,*dir;
int clk;
unsigned char bit,val;

dir=0xBFE301;
 pa=0xBFE101;

*pa=0;
*dir=0xF6;
printf("dir is %x in hex\n\n",*dir);

xxx:

printf("enter *pa");
scanf("%d",&clk);
val=clk;
*pa=val;
printf("*pa is %x in hex\n",*pa);
goto xxx;
}
