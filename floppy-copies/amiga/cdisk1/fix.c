#include <stdio.h>
#define MODE_NEWFILE 1006
#define MODE_OLDFILE 1005


main(argc,argv)
int argc;
char *argv[];

{
static char buff[1024*100];
char x;
int i;
FILE *out,*in,*Open();
in=Open(*++argv,MODE_OLDFILE);
out=Open(*++argv,MODE_NEWFILE);
if (in==0 ||out==0) {
   printf("cant open a file.");
   exit(10);
   }
i=Read(in,buff,999999);
Close(in);

printf("length of %d",i);
Write(out,&i,4);
Write(out,buff,i);
Close(out);
printf("Fiin\n\n");
}


