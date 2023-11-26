#include "exec/types.h"
#include "/include/studio16.h"

/******************************************/
/*  FFT Save for Crystal IBM FFT software */
/******************************************/

SaveFFT(wfp,rsp)

long wfp;
struct SampleFilePointer *rsp;
{
static short mydata[1024];
long error,i;
char str[80];

PutLine(wfp,"   CFFT5.11          FORMAT1");
PutLine(wfp,"   1024               36          48000");
PutLine(wfp,"   xxx                      16");
PutLine(wfp,"   CDATA");
if (SampleRead(rsp,mydata,1024)!=1024)
	error=READ_ERROR;
else {
	error=NO_ERROR;
	for (i=0; i <1024; i=i+8) {
		sprintf(str,"%9d%9d%9d%9d%9d%9d%9d%9d",mydata[i],mydata[i+1],mydata[i+2],mydata[i+3],mydata[i+4],mydata[i+5],mydata[i+6],mydata[i+7]);
		if (PutLine(wfp,str)) {
			error=WRITE_ERROR;
			break;
			}
		}
	}
return(error);
}

PutLine(fp, str)
long fp;
char *str;
{
Write(fp,str,strlen(str));
return(Write(fp,"\015\n",2)!=2);
}
