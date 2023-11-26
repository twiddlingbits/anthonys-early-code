#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "/include/psound.h"
#include "cl.h"

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
/**************************************************************/
int ReadLn(fp,ce)
LONG fp;
struct CueEntry *ce;
{
char a,*c;
byte f;
int x;
int TCstr2int();

if(Read(fp,ce->StartTimeStr,11)!=11) return(-1);
Read(fp,&a,1); while (a!=',' && a!=10) Read(fp,&a,1);

ce->Start=TCstr2int(ce->StartTimeStr);

if (a==10) return(-2);
Read(fp,ce->VolStr,3);
Read(fp,&a,1); while (a!=',' && a!=10) Read(fp,&a,1);
if (a==10) return(-2);

c=&ce->Sample[0];
x=0;f=0;

Read(fp,&ce->Sample[x],1); while (ce->Sample[x]==' ') Read(fp,&ce->Sample[x],1);

while(ce->Sample[x]!=',' && ce->Sample[x]!=10)
	{
	if (ce->Sample[x]==' ')
		{if (f==1) c=&ce->Sample[x];f=0;}
	 else f=1;
	if (x<40) x++;
	Read(fp,&ce->Sample[x],1);
	}
a=ce->Sample[x];
if(ce->Sample[x-1]==' ') *c=NULL;
else ce->Sample[x]=NULL;
if (a==10) return(NULL);

c=&ce->Rem[0];
x=0;f=0;
Read(fp,&ce->Rem[x],1);
while(ce->Rem[x]!=10)
	{
	if (ce->Rem[x]==' ') 
		{if (f==1) c=&ce->Rem[x];f=0;}
	else f=1;
	if (x<40) x++;
	Read(fp,&ce->Rem[x],1);
	}
if(ce->Rem[x-1]==' ')*c=NULL;
else ce->Rem[x]=NULL;

return(NULL);
}
/**************************************************************/
/**************************************************************/
void LoadF()
{
LONG fp,num;
char temp[200];
struct CueEntry *ce=NULL;
void Gad2Struct();
void Struct2Gad();
void Struct2Line();
int x,y,id;
char buff[80];
int cancel;
void DumpSamples();
extern struct Window *wind;

if (edits) 
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(!askuser(buff,"They will be lost. OK to continue?",0)) return;
	}

cancel=FileRequest("Cue List Load:",path,file,FR_SINGLE_SELECT);
if (cancel==0)	return;

hide_gadget(CueL);

DumpSamples();

strcpy(temp,path);strcat(temp,file);
if((fp=Open(temp,MODE_OLDFILE))==NULL) {telluser("Cue List:","Could not find the cue list file",NOTICE_RETURN_NOW);return;}
num=Read(fp,&id,4);
if(id==tinyid)
{
y=CueL->list_size;
for(x=0;x<y;x++)
	{
	delete_list_entry_by_index(CueL,0);
	}

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue LIST:","Memory Allocation Error (loadF)",NOTICE_RETURN_NOW);}
while (ReadLn(fp,ce)==0)
	{

	Struct2Gad(ce);
	Gad2Struct(ce);
	Struct2Line(ce);
	add_list_entry(CueL,ce->line,ce);
	select_list_entry_by_userdata(CueL,ce);
	if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue List:","Memory Allocation Error (load)",NOTICE_RETURN_NOW);break;}
	}
Close(fp);

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

cancel=FileRequest("Cue List Save:",path,file,FR_SINGLE_SELECT);
if (cancel==0)	return;

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
}
/**************************************************************/
/**************************************************************/
int TCstr2int(st)
char st[];
{
LONG itc;
struct TimeCode *StartTime=(struct TimeCode *)&itc;
ULONG v,x,y,c,z[12]={0,0,0,0,0,0,0,0,0,0,0,0};
char tem[80];
/*==========*/

y=0;c=0;v=0;
for (x=0;x<13;x++)
	{
	if ((st[x]>='0' && st[x]<='9')) {y=y*10; y=y+st[x]-'0';v++;}
	else {c++; z[c]=y; y=0;v=0;}
/*	if (v==2) {c++; z[c]=y; y=0;v=0;}*/
	}

if (z[1]>24) z[1]=00;
if (z[2]>59) z[2]=00;
if (z[3]>59) z[3]=00;
if (z[4]>30) z[4]=00;

StartTime->hours=z[1];
StartTime->minutes=z[2];
StartTime->seconds=z[3];
StartTime->frames=z[4];

sprintf(tem,"%02d:%02d:%02d:%02d",StartTime->hours,StartTime->minutes,StartTime->seconds,StartTime->frames);

movmem(tem,st,12);
return(itc);
}
