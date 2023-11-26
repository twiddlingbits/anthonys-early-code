
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseloadg(comcase,rk,rk2,l,win1,winp1,nrflag,prev_rm)
int comcase,rk,rk2,*l,*nrflag,*prev_rm;
struct Window *win1;
struct winprint *winp1;
{

   extern int offx[100],offy[100],objnum[2],objlist[2][100];
   extern struct Image *object_images[MAXLIST];
   struct Image *ti;
   int r,f,ll,i,ii,oc;
   int noun;
   extern int slave[4][2],secnum,noun_pics[MAXLIST],varval[MAXLIST];
   extern char *extlist[MAXLIST];
   extern struct loc2 rm;
   extern struct sequence *sequences[MAXLIST];
   extern char userfile[10];
   extern UBYTE *code;
   FILE *fp;
   int j,temp_pics[MAXLIST];

   f=2;
   if (comcase == 0) {
      if (userfile[0] == 0) {
         printf("boo1");
         j=0;
         eraseroomobjects(win1);
         printf("boo2");
         i=0;
         while(i<secnum) {
            if((sequences[i]->flags & ON)&& sequences[i]->room == 0)
               endsec(i+1,win1);
            i++;
            }

         printf("boo3");
         strncpy(&userfile[0],extlist[code[rk + 1]],10);
         fp = fopen(&userfile[0],"r");
         *prev_rm = *l;
         fscanf(fp,"%d,",l);
         printf("--%d--",*l);
         userfile[0] = 0;
         printf("boo4");
         i = 0;
         while(i<MAXLIST)
            fscanf(fp,"%d,",&varval[i++]);
         printf("boo5");
         i = 0;
         while(i<MAXLIST)
            fscanf(fp,"%d,",&temp_pics[i++]);
         printf("boo6");
         i = 0;
         while(i<rk2) {
            fscanf(fp,"%d,%d,",&rm.desc[i],&rm.pic[i]);
            fscanf(fp,"%d,%d,",&rm.n[i],&rm.s[i]);
            fscanf(fp,"%d,%d,",&rm.e[i],&rm.w[i]);
            fscanf(fp,"%d,%d,",&rm.u[i],&rm.d[i]);
            j=0;
            do {
               fscanf(fp,"%d,",&rm.item[i][j]);
               } while (++j < MAXLIST);
            i++;
            }
         printf("boo7");
         i=0;
         while(i<secnum)  {
            fscanf(fp,"%d,",&j);
            sequences[i]->room = (char) j;
            fscanf(fp,"%d,",&(sequences[i++]->flags));
            }
         printf("boo8");
         i=0;
         while(i<4)
            fscanf(fp,"%d,%d,",&slave[i][0],&slave[i++][1]);
         printf("boo9");
         i=0;
         while(i<100)
            fscanf(fp,"%d,%d,",&offx[i],&offy[i++]);
         i=0;
         while(i<100)
            fprintf(fp,"%d,",&objlist[0][i++]);
         i=0;
         while(i<100)
            fprintf(fp,"%d,",&objlist[1][i++]);
         printf("boo10");
         fclose(fp);

         f=-1;
         *nrflag = 1;
         printf("boo11");
         i=0;
         while(i<secnum) {
            if((sequences[i]->flags & ON)) {
               if (sequences[i]->room == 0) {
                  if(sequences[i]->flags & ACTIVATED) {
                     sequences[i]->flags =sequences[i]->flags &(255-(ON | ACTIVATED));
                     startsec(win1,i+1,offx[i],offy[i]);
                     activate(i+1);
                     }
                  else {
                     sequences[i]->flags =sequences[i]->flags &(255-(ON | ACTIVATED));
                     startsec(win1,i+1,offx[i],offy[i]);
                     }
                  }
               }
            i++;
            }
         }
      }

   return(f);


}

