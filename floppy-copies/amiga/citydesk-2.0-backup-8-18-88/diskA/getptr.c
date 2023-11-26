#include <exec/types.h>
#include <intuition/intuition.h>

struct IntuitionBase *IntuitionBase;

main()
{
struct Preferences pref;
int i;

IntuitionBase = OpenLibrary("intuition.library", 0);

GetPrefs (&pref, sizeof(struct Preferences));

printf("USHORT pointer[] = {\n");
printf("   0x0000, 0x0000,\n");
for (i=0; i<16; i++) {
   printf("   0x%x, 0x%x,\n", pref.PointerMatrix[i*2], pref.PointerMatrix[i*2+1]);
}
printf("   0x0000, 0x0000,\n");
printf("   0x0000, 0x0000,\n");
printf("};\n");
printf(" WIDTH = 16, HEIGHT = 16\n");
printf(" XOFFSET = %d, YOFFSET = %d\n\n", (int)pref.XOffset, (int)pref.YOffset);

CloseLibrary(IntuitionBase);

}

