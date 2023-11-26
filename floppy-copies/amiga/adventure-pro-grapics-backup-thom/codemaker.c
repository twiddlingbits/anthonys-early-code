
#include <exec/types.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>


main () {

char words[120];
char t1,t2;
int a,i,j;
FILE *fp;

printf("Enter number of words to be coded:");
scanf("%d\n",&i);
j = 0;
while (j <i) {
   printf("\nword:");
   scanf("%c%c%c%c",&words[j*4],&words[j*4+1],&words[j*4+2],&words[j*4+3]);
   j++;
   }

fp = fopen("codedata.c","w");

fprintf(fp,"\n\n /* coded words file */\n\n");
fprintf(fp,"char words[120] = {\n");
   a = 0;
   do {
      if ((a & 7) == 7)
         fprintf(fp,"\n");
      if (a>0)
         fprintf(fp,",");
      t1 = (words[a]>>4) + (words[a]<<4);
      fprintf(fp,"%d",t1);
      } while(a++ < 119);
fprintf(fp,"};\n\n\n");

fclose(fp);

}


