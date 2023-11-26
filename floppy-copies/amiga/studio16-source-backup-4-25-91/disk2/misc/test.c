#include <exec/types.h>
#include <libraries/dos.h>


main()

{
int fp;
int i;

fp=Open("killme",MODE_NEWFILE);


for (i=0; i < 16; i++)
   Write(fp, i*64*1024, 64*1024);

Close(fp);
}

