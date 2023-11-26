

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "sed1.h"

char *bufL[2],*bufR[2];
struct IntuitionBase *IntuitionBase;

main()
{
struct Window *win;
int i;

IntuitionBase = (struct IntuitionBase *) OpenLibrary(
                "intuition.library",0);
if (IntuitionBase == NULL)   exit(10);

printf("boo1");

for (i=0;i<2;i++) {
   if ((bufL[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      printf("Can't allocate buffer memory!\n");
      exit(0);
      }
printf("boo3");

   if ((bufR[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      printf("Can't allocate buffer memory!\n");
      exit(0);
      }
   }

printf("boo2");
win = nwwin();

printf("boo4");
loadnplay("testsound",win);

}

