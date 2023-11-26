
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casestoreg(comcase,rk,rk2,l)
int comcase,rk,rk2,l;
{

   extern int objlist[2][100],offx[100],offy[100],slave[4][2],secnum,noun_pics[MAXLIST],varval[MAXLIST];
   extern char *extlist[MAXLIST];
   extern struct loc2 rm;
   extern struct sequence *sequences[MAXLIST];
   extern char userfile[10];
   extern UBYTE *code;
   FILE *fp;
   int i,j;

   if (comcase == 0) {
      if (userfile[0] == 0) {
         strncpy(&userfile[0],extlist[code[rk + 1]],10);
         fp = fopen(&userfile[0],"w");
         fprintf(fp,"%d,",l);
         userfile[0] = 0;
         i = 0;
         while(i<MAXLIST)
            fprintf(fp,"%d,",varval[i++]);
         i = 0;
         while(i<MAXLIST)
            fprintf(fp,"%d,",noun_pics[i++]);
         i = 0;
         while(i<rk2) {
            fprintf(fp,"%d,%d,",rm.desc[i],rm.pic[i]);
            fprintf(fp,"%d,%d,",rm.n[i],rm.s[i]);
            fprintf(fp,"%d,%d,",rm.e[i],rm.w[i]);
            fprintf(fp,"%d,%d,",rm.u[i],rm.d[i]);
            j=0;
            do {
               fprintf(fp,"%d,",rm.item[i][j]);
               } while (++j < MAXLIST);
            i++;
            }
         i=0;
         while(i<secnum) {
            fprintf(fp,"%d,",sequences[i]->room);
            fprintf(fp,"%d,",sequences[i++]->flags);
            }
         i=0;
         while(i<4)
            fprintf(fp,"%d,%d,",slave[i][0],slave[i++][1]);
         i=0;
         while(i<100)
            fprintf(fp,"%d,%d,",offx[i],offy[i++]);
         i=0;
         while(i<100)
            fprintf(fp,"%d,",objlist[0][i++]);
         i=0;
         while(i<100)
            fprintf(fp,"%d,",objlist[1][i++]);
         fclose(fp);
         }
      }

   return(2);


}

