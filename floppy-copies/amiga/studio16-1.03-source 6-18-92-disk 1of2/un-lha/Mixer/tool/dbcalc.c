#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "float.h"
#include "mieeedoub.h"

main()
{
double x,y;
int x2=0;
/*for(x=6.0;x>-97.1;x=x-0.25)
	{
	y=pow(10.0,x/20.0)*32768.0;
	printf("%4.2f %10.3f\n",x,y);
	}
printf("----------------------------------------------\n");*/
for(x=-100.0;x<=6.00;x=x+0.03125)
	{
	if (x2==0) printf("\n/* %3.0f db */ ",x);
	x2=(x2+1) & 31;
	y=pow(10.0,x/20.0)*32768.0;
	printf("%5.0f,",y);
	}
printf("\n");
}
