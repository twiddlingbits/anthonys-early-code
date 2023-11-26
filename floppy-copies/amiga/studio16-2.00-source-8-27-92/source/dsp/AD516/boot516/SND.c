#include "exec/types.h"
#include "exec/exec.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
/********************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
if (argc==3) LoadCode(argv[1],argv[2]);
else printf("USAGE:%s  SndRawData (16 bits)\n",argv[0]);
}
/********************************************************************/
int LoadCode(s,vs)
char *s;
char *vs;
{
struct FileHandle *fp2;
extern struct FileHandle *Open();
short *x;
void Split();

if(!(x=(short *)AllocMem(0x0401<<2,NULL))) return(1);

if (!(fp2=Open(s,MODE_OLDFILE)))
	{
	FreeMem(x,0x0401<<2);
	printf("COULD NOT OPEN FILE\n\n");
	return(2);
	}

if(Read(fp2,x,(256<<2))!=(256<<2))
	{
	FreeMem(x,0x03f4<<2);
	Close(fp2);
	printf("UNEXPECTED END OF FILE\n\n");
	return(3);
	}
printf("\n{****************** DSP SND  ******************}\n");
Split(x);
printf("\n{**********************************************}");

FreeMem(x,0x0401<<2);
Close(fp2);
return(0);
}
/********************************************************************/
void Split(file)
short *file;
{
short x;
for (x=0;x<256;x+=8)
	printf("{ %03d }  %6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,\n",x,file[x],file[x+1],file[x+2],file[x+3],file[x+4],file[x+5],file[x+6],file[x+7]);
}
/********************************************************************/
