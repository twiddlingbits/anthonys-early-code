#include <exec/types.h>
#include <stdio.h>

long MyBase;
/*   ^^^^^^ should be the same as the #define'd BASE in mylib.c */

#include "mylibpragma.h"
/*        ^^^^^^^^^^^^^ created by fd2pragma of mylib.fd */

#define RTC printf("return to continue - ");fflush(stdout);\
getchar();

#define DOUBARG 19

char text[100] = "hello";

main()
{
	 LONG	 retval;

	 printf("here we go\n");
	 MyBase = (APTR) OpenLibrary("mylib.library", 0L);
	 printf("openlib returns base: %lx\n", MyBase);

	 RTC;

	 if (MyBase)
	 {
		 /* test function GetDown()      */
		 retval = GetDown();
		 printf("called getdown, %ld returned\n", retval);
		 RTC;

		 /* test function Double()       */
		 printf("double of %d = %ld\n", DOUBARG, Double((LONG)DOUBARG));
		 RTC;

		 /* test function Triple()       */
		 printf("Here is three times %d: %ld\n",DOUBARG,
			 Triple((LONG)DOUBARG));
		 RTC;

		 printf("Here is 12+13: %ld\n",Add((LONG)12,(LONG)13));

		 printf("Here is hello+there: %s\n",StrCat(text,"there"));

		 CloseLibrary(MyBase);
	 }
}
