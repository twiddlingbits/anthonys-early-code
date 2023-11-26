#include <exec/types.h>
#include <intuition/intuition.h>

struct IntuitionBase *IntuitionBase;

USHORT pointer[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x4000, 0x2,
   0x2000, 0x4,
   0x1000, 0x8,
   0x800, 0x10,
   0x400, 0x20,
   0x200, 0x40,
   0x100, 0x80,
   0x80, 0x100,
   0x40, 0x200,
   0x20, 0x400,
   0x10, 0x800,
   0x8, 0x1000,
   0x4, 0x2000,
   0x2, 0x4000,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -8 */

struct NewWindow nw = {
   0, 0, 100, 100, 1, 2, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 100, 100,
   WBENCHSCREEN
};

main()
{
int i;
struct Window *win;
char x[10];

IntuitionBase = OpenLibrary("intuition.library", 0);

win = OpenWindow(&nw);

SetPointer(win, pointer, 16, 16, -9, -8);

scanf("%s", &x);

CloseLibrary(IntuitionBase);

}

