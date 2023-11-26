#include <stdio.h>

main()
{
FILE *in,*out;
unsigned char c,p;

in = fopen("in","r");

while ((c=fgetc(in))!=EOF)
   if (c == 10 || c==0xc7 || (c >31 && c<128)) {
   if (c==0xc7) c=10;
   if (c==10 && p==10) ;
   else fputc(c,stdout);
      if (c==';') {fputc(10,stdout);c=10;}
      p = c;
      }

fclose(in);
}
