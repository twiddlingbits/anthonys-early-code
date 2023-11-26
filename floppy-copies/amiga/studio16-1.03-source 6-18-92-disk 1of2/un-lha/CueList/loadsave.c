#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "/include/psound.h"
#include "cl.h"

BOOL KeyOk=TRUE;
extern struct StudioBase *StudioBase;
extern long preload_sigbit;
extern struct Task *task;
/**************************************************************/
extern struct list *CueL;
extern int edits;
/**************************************************************/
#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define tinyid MakeID('T','Q','L',10)
/**************************************************************/
char path[34]="Studio16:CueLists";
char file[33]="Demo.List";
/**************************************************************/
#define CLE_EOF				1
#define CLE_UNEXPECTED_EOL		-2
#define CLE_SAMPLE_NAME_TOO_LONG	-3
#define CLE_READ_ERROR			-4
/**************************************************************/
int ReadLn(fp,ce)
LONG fp;
struct CueEntry *ce;
{
char a,*c;
byte f;
int x;
int TCstr2int();

/*************/
if(Read(fp,ce->StartTimeStr,11)!=11) return(CLE_EOF);
if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
while (a!=',' && a!=10) if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
if (a==10) return(CLE_UNEXPECTED_EOL);

ce->Start=TCstr2int(ce->StartTimeStr);
/*************/
if(3!=Read(fp,ce->VolStr,3)) return(CLE_READ_ERROR);

if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
while (a!=',' && a!=10) if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);

if (a==10) return(CLE_UNEXPECTED_EOL);
/*************/

c=&ce->Sample[0];
x=0;f=0;

if(1!=Read(fp,&ce->Sample[x],1)) return(CLE_READ_ERROR);
while (ce->Sample[x]==' ') 
	if(1!=Read(fp,&ce->Sample[x],1)) return(CLE_READ_ERROR);

while(ce->Sample[x]!=',' && ce->Sample[x]!=10)
	{
	if (ce->Sample[x]==' ')
		{
		if (f==1) c=&ce->Sample[x];
		f=0;
		}
	else f=1;
	if (x<40) x++;
	else return(CLE_SAMPLE_NAME_TOO_LONG);
	if(1!=Read(fp,&ce->Sample[x],1)) return(CLE_EOF);
	}

a=ce->Sample[x];
if(ce->Sample[x-1]==' ') *c=NULL;
else ce->Sample[x]=NULL;

if (a==10) return(NULL);
/*************/

c=&ce->Rem[0];
x=0;f=0;
if (1!=Read(fp,&ce->Rem[x],1)) return(CLE_EOF);
while(ce->Rem[x]!=10)
	{
	if (ce->Rem[x]==' ') 
		{
		if (f==1) c=&ce->Rem[x];
		f=0;
		}
	else f=1;
	if (x<40) x++;
	if (1!=Read(fp,&ce->Rem[x],1)) return(CLE_EOF);
	}
if(ce->Rem[x-1]==' ')*c=NULL;
else ce->Rem[x]=NULL;

return(NULL);
/*************/

}
/**************************************************************/
extern struct Window *wind;
/**************************************************************/
void LoadF()
{
LONG fp;
char temp[200];
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce=NULL;
void Gad2Struct();
void Struct2Gad();
void Struct2Line();
int x,y,id,line=1,err;
char buff[80];
int cancel;
void DumpSamples();

if (edits) 
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(!askuser(buff,"They will be lost. OK to continue?",0)) return;
	}

KeyOk=FALSE;
cancel=FileRequest("Cue List Load:",path,file,FR_SINGLE_SELECT);
if (cancel==0)	{KeyOk=TRUE;return;}


hide_gadget(CueL);

DumpSamples();

strcpy(temp,path);strcat(temp,file);
if((fp=Open(temp,MODE_OLDFILE))==NULL) {telluser("Cue List:","Could not open the file",NOTICE_RETURN_NOW);KeyOk=TRUE;return;}
Read(fp,&id,4);
if(id==tinyid)
{
y=CueL->list_size;
for(x=0;x<y;x++)
	{
	le=find_list_entry_by_index(CueL,0);
	ce=(struct CueEntry *)le->user_data;
	FreeMem(ce,sizeof(struct CueEntry));
	delete_list_entry_by_index(CueL,0);
	}

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue LIST:","Memory Allocation Error (loadF)",NOTICE_RETURN_NOW);}
while ((err=ReadLn(fp,ce))==0)
	{
	line++;
	Struct2Gad(ce);
	Gad2Struct(ce);
	Struct2Line(ce);
	add_list_entry(CueL,ce->line,ce);
	select_list_entry_by_userdata(CueL,ce);
	if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue List:","Memory Allocation Error (load)",NOTICE_RETURN_NOW);break;}
	}
Close(fp);
FreeMem(ce,sizeof(struct CueEntry));

if (err<0) 
	{
	sprintf(temp,"Cue List Load Error:  Line:%d",line);
	if (err==-2) telluser(temp,"Unexpected End-Of-Line",NULL);
	else if (err==-3) telluser(temp,"Sample Name Too Long",NULL);
	else if (err==-4) telluser(temp,"Read Error",NULL);
	else telluser(temp,"Undefined Error",NULL);
	}
WindTitleChange(wind,file);
}
else 
	{
	Close(fp);
	telluser("Cue List:","This file does not contain a CueList",NOTICE_RETURN_NOW);
	}

show_gadget(CueL);
refresh_gadget(CueL);
Signal(task,1<<preload_sigbit);
edits=0;
KeyOk=TRUE;
}
/**************************************************************/
/**************************************************************/
void SaveF()
{
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
int x,id,num;
LONG fp;
char temp[200];
int cancel;

if (CueL->list_size==0) {telluser("Cue List:","No cue List to Save",NULL);return;}

KeyOk=FALSE;
cancel=FileRequest("Cue List Save:",path,file,FR_SINGLE_SELECT);
if (cancel==0)	{KeyOk=TRUE;return;}

WindTitleChange(wind,file);

strcpy(temp,path);strcat(temp,file);
fp=Open(temp,MODE_NEWFILE);
if (fp)	{
	id=tinyid;
	Write(fp,&id,4);

	for(x=0;x<CueL->list_size;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		sprintf(temp,"%-11s ,%-3s ,%-40s,%s \n",ce->StartTimeStr,ce->VolStr,ce->Sample,ce->Rem);
		num=strlen(temp);
		if (Write(fp,temp,num)!=num){telluser("Cue List:","Error Saving Cue List (write)",NOTICE_RETURN_NOW);x=CueL->list_size;}
		}
	Close(fp);
	edits=0;
	}
else telluser("Cue List:","Error Saving Cue List (open)",NOTICE_RETURN_NOW);
KeyOk=TRUE;
}
/**************************************************************/
/**************************************************************/
int TCstr2int(st)
char st[];
{
LONG itc;
struct TimeCode *StartTime=(struct TimeCode *)&itc;
ULONG x,y,c,z[12]={0,0,0,0,0,0,0,0,0,0,0,0};
char tem[80];
/*==========*/

y=0;c=0;
for (x=0;x<13 ;x++)
	{
	if ((st[x]>='0' && st[x]<='9')) {y=y*10; y=y+st[x]-'0';}
	else {c++; z[c]=y; y=0;}
	}

if (z[1]>23) z[1]=23;
if (z[2]>59) z[2]=59;
if (z[3]>59) z[3]=59;
if (z[4]>StudioBase->defaults.smpte_mode-1) z[4]=StudioBase->defaults.smpte_mode-1;

StartTime->hours=z[1];
StartTime->minutes=z[2];
StartTime->seconds=z[3];
StartTime->frames=z[4];

sprintf(tem,"%02d:%02d:%02d:%02d",StartTime->hours,StartTime->minutes,StartTime->seconds,StartTime->frames);

movmem(tem,st,12);
return(itc);
}
