
#include <exec/types.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>


main () {

char t1,t2;
int a,i,j;
   a = 0;
   do{
      t1 = words[j][a];
      t2 = words[j][a];
      t1 = (t1<<4) + (t2<<4);
      fprintf(fp,"%d,",t1);
      } while(words[j][a++] != 0);
   fprintf(fp,"},\n");
   j++;
   }
fprintf(fp,"};\n\n\n");

fclose(fp);

}


