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
else printf("USAGE:%s  DSP_CODE_FILE_NAME  ARRAY_VARABLE_NAME\n",argv[0]);
}
/********************************************************************/
int LoadCode(s,vs)
char *s;
char *vs;
{
struct FileHandle *fp2;
extern struct FileHandle *Open();
USHORT *x;
void Split();

if(!(x=(USHORT *)AllocMem(0x0401<<2,NULL))) return(1);

if (!(fp2=Open(s,MODE_OLDFILE)))
	{
	FreeMem(x,0x03f4<<2);
	printf("COULD NOT OPEN FILE\n\n");
	return(2);
	}

if(Read(fp2,x,(0x0400<<2))!=(0x0400<<2))
	{
	FreeMem(x,0x03f4<<2);
	Close(fp2);
	printf("UNEXPECTED END OF FILE\n\n");
	return(3);
	}
printf("#include <exec/types.h>\n");
printf("#include <exec/exec.h>\n");
printf("\n/****************** DSP CODE - %s ******************/\nUSHORT %s[] = {\n",s,vs);
Split(x);
printf("};\n/***********************************************************/\n\n",s);

FreeMem(x,0x0401<<2);
Close(fp2);
return(0);
}
/********************************************************************/
void Split(file)
USHORT *file;
{
int x;
for (x=0;x<(0x0400<<1);x+=8)
	printf("/* 0x%04x */  0x%04x,0x%04x,0x%04x,0x%04x,0x%04x,0x%04x,0x%04x,0x%04x,\n",x,file[x],file[x+1],file[x+2],file[x+3],file[x+4],file[x+5],file[x+6],file[x+7]);
}
/********************************************************************/
