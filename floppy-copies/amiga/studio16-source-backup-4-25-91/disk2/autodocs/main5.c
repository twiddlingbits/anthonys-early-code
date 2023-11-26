#include "exec/types.h"
#include "intuition/intuition.h"
#include <dos.h>
#include "exec/exec.h"

#define MAXIMUM_NUMBER_OF_FUNCTIONS 200
#define INLINESIZE 200
#define INTUITION_REV 33
extern struct IntuitionBase *IntuitionBase;

char *FunTxt[MAXIMUM_NUMBER_OF_FUNCTIONS];
char *FunName[MAXIMUM_NUMBER_OF_FUNCTIONS];
int LastFun = 0;

char Lin[80] = {"----------------------------------------------------------------------------\n"};
char start[] = {'/','*','#',0};
char end[] = {'#','*','/',0};
char funs[] = {'{',0};
char FormFeed[] = {12,0};
char *CurrentFileName;

void CheckFile();
void CheeseSort();
void CheckBuff();
void PrintFun();
int ChSt();
char *SaveBuff1;
char *SaveBuff2;
char *SaveBuff3;

struct Remember *RemKey=NULL;

/*============================================================*/
void main(argc,argv)
int argc;
char *argv[];
{
int errr;
struct FILEINFO dinfo;

if(!(IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",INTUITION_REV)))
		{printf("Could Not Open Intuition!");exit(0);}

if (!(SaveBuff1  = (char *) AllocRemember(&RemKey,16*1024,0)))
		{printf("Could Not Allocate Memory!");xit(0);}
if (!(SaveBuff2  = (char *) AllocRemember(&RemKey,16*1024,0)))
		{printf("Could Not Allocate Memory!");xit(0);}
if (!(SaveBuff3  = (char *) AllocRemember(&RemKey,16*1024,0)))
		{printf("Could Not Allocate Memory!");xit(0);}

errr=dfind(&dinfo,argv[1],0);
	if (stricmp(argv[2],&"NOLF") == 0)  FormFeed[0]=12;

while (errr==0)
	{
	CheckFile(dinfo.fib_FileName);
	errr=dnext(&dinfo);
	}

	CheeseSort();
	PrintFun();
xit();
}

xit()
{
FreeRemember(&RemKey,TRUE);
if (IntuitionBase) CloseLibrary(IntuitionBase);
}

/*############################################################
NAME
	CheckFile -
FUNCTION
	checks a file for autodocks
INPUTS
	filename = name of file to check.
	size = number of bytes in the file 'filename'
RESULTS
	formats and print all of the autodoc information
	contained within the file 'filename' 
##############################################################*/
void CheckFile(filename)
char *filename;
{
int fh;
char InBuff[INLINESIZE];
char *buff;

if ((fh=fopen(filename,&"r"))==-1)
	 {printf("\n\nERROR: Could Not Open File!\n");xit();}

CurrentFileName=filename;

buff=fgets(InBuff,INLINESIZE,fh);
while (buff!=NULL)
	{
	CheckBuff(buff,filename);
	buff=fgets(InBuff,INLINESIZE,fh);
	}

fclose(fh);
}

int onoff=0;
int len1=0,len2=0,len3=0,slen;
/*############################################################
NAME
	CheckBuff - Descript
FUNCTION
	checks a file for autodocks
INPUTS
	filename = name of file to check.
	size = number of bytes in the file 'filename'
RESULTS
	formats and print all of the autodoc information
	contained within the file 'filename' 
##############################################################*/
void CheckBuff(buff)
char *buff;
{
int x,y;
char temp;
static line=0;
switch (onoff)
{

case 0:
	if (InString(buff,start) != -1) {onoff=1; line=0;len1=0;len2=0;len3=0;}
break;

case 1:
	if (InString(buff,end) != -1) {onoff=2;}

	else	{
		switch(line)
			{
			case 0:
			break;

			case 1:
				len1=strlen(buff);
for(x=0;x<len1 && !((*(buff+x) > 'A' && *(buff+x) < 'Z') || (*(buff+x) > '_' && *(buff+x) < 'z') ); x++) ;
				len1=strlen(buff+x);
for(y=x;y<len1 && ((*(buff+y) > 'A' && *(buff+y) < 'Z') || (*(buff+y) > '_' && *(buff+y) < 'z') ); y++) ;
				len1=sprintf(SaveBuff1,"%s\t\t",CurrentFileName);
				temp=*(buff+y); *(buff+y)=0;
				slen=len1;
				len1=len1+sprintf(SaveBuff1+len1,"(%s)\n",buff+x);
				*(buff+y)=temp;
				len1=len1+sprintf(SaveBuff1+len1,Lin);
				len1=len1+sprintf(SaveBuff1+len1,"NAME\n%s",buff); 
				
			break;

			default:
				len3=len3+sprintf(SaveBuff3+len3,"%s",buff);
			break;
			}
		line++;
		}
break;
case 2:
	if (InString(buff,funs) != -1) {onoff=3;}
	len2=len2+sprintf(SaveBuff2+len2,"\t%s",buff);
break;
case 3:
	if (!(FunTxt[LastFun]  = (char *) AllocRemember(&RemKey,strlen(SaveBuff1)+strlen(SaveBuff2)+strlen(SaveBuff3)+strlen(Lin)+10,0)))
		{printf("Could Not Allocate Memory!");xit(0);}
	sprintf(FunTxt[LastFun],"%sSYNOPSIS\n%s%s%s",SaveBuff1,SaveBuff2,SaveBuff3,Lin);
	FunName[LastFun]=FunTxt[LastFun]+slen;
	LastFun++;
	onoff=0;
break;
}
}

/*##############################################################
NAME
	InString -
FUNCTION
	checks string1 for an occurance of string2
INPUTS
	string1 = string to search
	string2 = search string
RESULTS
	returns the number of characters matched
##############################################################*/
InString(s1,s2)
char *s1;
char *s2;
{
int len1,len2,idx=0,Found=-1;

len1 = strlen(s1);
len2 = strlen(s2);

while ((idx < len1-len2) && Found==-1)
	{
	Found=idx;
	if (ChSt(s1+idx,s2,0)!=len2) Found=-1;
	idx++;
	}

return (Found);
}

int ChSt(s1,s2,in)
char *s1;
char *s2;
int  in;
{
static top;
if (in==0) top=0;
if (in>top) top=in;
if ((*s2 != NULL) && (*s1 == *s2) ) {in++;ChSt(s1+1,s2+1,in);}
return(top);
}

/*##############################################################
NAME
	CheeseSort - Sorts data
FUNCTION
	ACTUNG! Avoid limburger
##############################################################*/

void CheeseSort()
{
int x,l,flag=1;
char *temp;

l=LastFun-1;

while(flag)
	{
	flag=0;
	l=l-1;
	for (x=0;x<l;x++)
		{
		if (strnicmp(FunName[x],FunName[x+1],15) > 0)
			{
			temp=FunName[x];FunName[x]=FunName[x+1];FunName[x+1]=temp;
			temp=FunTxt[x];FunTxt[x]=FunTxt[x+1];FunTxt[x+1]=temp;
			flag=1;
			}
		}
	}
}

/*##############################################################
NAME
	PrintThem - Prints the list
FUNCTION
	P
##############################################################*/
void PrintFun()
{
int x;

for(x=0;x<LastFun-1;x++)
printf("%s%s",FormFeed,FunTxt[x]);
}