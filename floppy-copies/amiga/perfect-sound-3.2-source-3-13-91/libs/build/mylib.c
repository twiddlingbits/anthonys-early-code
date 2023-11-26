/* this is a test library */
#include <exec/types.h>

#define LIBNAME mylib.library
#define BASE MyBase
#define VERSION 2
#define REVISION 1
#define LIBID This is mylib.library 2.1

/* The lines above are only to allow the user to change the name of the
   library and change the library's base name.  The base name is only used
   if the #pragma is used.  Unfortunately, the #pragma is limited to only
   6 parameters.  To use all 12, you will have to link your program with
   the stub file. */

LONG GetDown()
{
	 return (77);
}

ULONG Double(arg)
ULONG arg;
{
	 return (2 * arg);
}

LONG Triple(arg)
LONG arg;
{
    arg *= 3;

    return ((LONG)arg);
}

LONG Add(apples,oranges)
LONG apples,oranges;
{
    return(apples+oranges);
}

char *StrCat(a,b)
  char *a,*b;
{
  strcat(a,b);
  return(a);
}

