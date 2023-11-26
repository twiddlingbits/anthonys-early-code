
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "adventure.h"
#include "sed1.h"


objectclear(secnum)
int secnum;
{
   extern struct Image *object_images[MAXLIST];
   extern struct sequence *sequences[MAXLIST];
   extern struct Samp samp[4];
   extern struct IOAudio IOB[4];
   extern struct IntuitionBase *IntuitionBase;
   extern struct GfxBase *GfxBase;
   extern struct loc2 rm;
   extern char *nounlist[MAXLIST];
   extern char *verblist[MAXLIST];
   extern char *adjlist[MAXLIST];
   extern char *extlist[MAXLIST];
   extern char *ext2list[MAXRMS];
   extern char *ext3list[MAXLIST];
   extern char *prepolist[MAXLIST];
   extern char *typelist[MAXLIST];
   extern UBYTE *code;
   extern int codekount;
   int i,ii;
   struct Image *ti;
   int k,j,l,num;
   struct sequence *a;
   struct Image *b;
   struct BitMap *c;

   FreeMem(code,codekount);

   i = 0;
   while (i < MAXLIST) {
      if (object_images[i] > 0) {
         ti = object_images[i];
         FreeMem(ti->ImageData,( ( (ti->Width+15) >>4) <<1)
                 *(ti->Height)*(ti->Depth));
         FreeMem(object_images[i],sizeof(struct Image));
         }
      i++;
      }

num = 0;
while (num < secnum)  {
   a = sequences[num];
   c = &(a->background);
   if (c->Planes[0] > 0)
      FreeMem(c->Planes[0],RASSIZE((c->BytesPerRow * 8),c->Rows)*c->Depth);
   i = 0;
   while (i < (a->framenum)) {
      if (a->graphics[i] > 0) {
         FreeMem(a->graphics[i]->ImageData,( ( (a->graphics[i]->Width+15)
         >>4) <<1)*(a->graphics[i]->Height)*(a->graphics[i]->Depth));
         FreeMem(a->graphics[i],sizeof(struct Image));
         a->graphics[i] = 0;
         }
      i++;
      }
   l = 0;
   while (l <a->framenum)
      FreeMem(a->names[l],strlen(a->names[l++])+1);
   FreeMem(a,sizeof(struct sequence));

   num++;
   }
l=0;
while(l<4)
   if(samp[l++].lmem != 0) {
      wipe_it(&samp[l-1]);
      deallocate_channel(&IOB[l-1]);
      }

l=1;
while(nounlist[l] != 0) {
   FreeMem(nounlist[l],strlen(nounlist[l])+1);
   l++;
   }

l=1;
while(verblist[l] != 0) {
   FreeMem(verblist[l],strlen(verblist[l])+1);
   l++;
   }

l=1;
while(adjlist[l] != 0) {
   FreeMem(adjlist[l],strlen(adjlist[l])+1);
   l++;
   }

l=1;
while(prepolist[l] != 0) {
   FreeMem(prepolist[l],strlen(prepolist[l])+1);
   l++;
   }

l=1;
while(extlist[l] != 0) {
   FreeMem(extlist[l],strlen(extlist[l])+1);
   l++;
   }

l=1;
while(ext2list[l] != 0) {
   FreeMem(ext2list[l],strlen(ext2list[l])+1);
   l++;
   }

l=1;
while(ext3list[l] != 0) {
   FreeMem(ext3list[l],strlen(ext3list[l])+1);
   l++;
   }

l=1;
while(typelist[l] != 0) {
   FreeMem(typelist[l],strlen(typelist[l])+1);
   l++;
   }

CloseLibrary(GfxBase);   
CloseLibrary(IntuitionBase);

}




