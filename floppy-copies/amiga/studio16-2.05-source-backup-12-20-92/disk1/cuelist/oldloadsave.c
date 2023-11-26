#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "/include/psound.h"
#include "cl.h"

extern struct Module *mod;
int DBstr2int();
BOOL KeyOk=TRUE;
extern struct StudioBase *StudioBase;
extern long preload_sigbit;
extern struct Task *task;
/**************************************************************/
extern struct list *CueL;
extern int edits;
int FindRegion();
extern BOOL LIST_EMPTY;
void MenuGhost();
int MyRead();
/**************************************************************/
#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define tinyid MakeID('T','Q','L',10)
/**************************************************************/
char file[50]="Untitled.Cue";
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
char temp[100];
char name[100];
char cpath[STUDIO16_MAX_PATH_SIZE];
int stc,vol;
void FigurePath();
char *path;

setmem(ce,sizeof(struct CueEntry),0);
/*************/
if(Read(fp,temp,11)!=11) return(CLE_EOF);
temp[11]=NULL;
stc=TCstr2int(temp);

if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
while (a!=',' && a!=10) if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
if (a==10) return(CLE_UNEXPECTED_EOL);
/*************/
if(3!=Read(fp,temp,3)) return(CLE_READ_ERROR);
temp[3]=NULL;
vol=DBstr2int(temp);

if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);
while (a!=',' && a!=10) if(1!=Read(fp,&a,1)) return(CLE_READ_ERROR);

if (a==10) return(CLE_UNEXPECTED_EOL);
/*************/
c=&name[0];
x=0;f=0;

if(1!=Read(fp,&name[x],1)) return(CLE_READ_ERROR);
while (name[x]==' ') 
	if(1!=Read(fp,&name[x],1)) return(CLE_READ_ERROR);

while(name[x]!=',' && name[x]!=10)
	{
	if (name[x]==' ')
		{
		if (f==1) c=&name[x];
		f=0;
		}
	else f=1;
	if (x<STUDIO16_MAX_PATH_SIZE) x++;
	else return(CLE_SAMPLE_NAME_TOO_LONG);
	if(1!=Read(fp,&name[x],1)) return(CLE_EOF);
	}

a=name[x];
if(name[x-1]==' ') *c=NULL;
else name[x]=NULL;

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
/*************/
if (FindRegion(name,&ce->regn))
	{
	path=(char *)FindSample(name);
	if (path)
		{
		strcpy(cpath,path);
		StrPathFileCat(cpath,name);
		if (FindRegion(cpath,&ce->regn)) {strcpy(ce->regn.samp_name,name);}
		}
	else strcpy(ce->regn.samp_name,name);
	}
FigurePath(ce);
ce->regn.parms.starttimecode=stc;
ce->regn.parms.volume=vol;
/*************/
return(NULL);
/*************/
}
/**************************************************************/
extern struct Window *wind;
/**************************************************************/
void LoadF(flg)
int flg;
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

if (edits && flg) 
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(!askuser(buff,"They will be lost. OK to continue?",0)) return;
	}

KeyOk=FALSE;
if(flg)cancel=FileRequest("Cue List Load:",((struct CueModState *)(mod->state))->path,file,FR_SINGLE_SELECT);
else cancel=FileRequest("Cue List Merge:",((struct CueModState *)(mod->state))->path,file,FR_SINGLE_SELECT);
if (cancel==0)	{KeyOk=TRUE;return;}

new_pointer(PTR_BUSY);
hide_gadget(CueL);

DumpSamples();

strcpy(temp,((struct CueModState *)(mod->state))->path);strcat(temp,file);
if((fp=Open(temp,MODE_OLDFILE))==NULL) {telluser("Cue List:","Could not open the file",NOTICE_RETURN_NOW);KeyOk=TRUE;new_pointer(PTR_NORMAL);return;}
Read(fp,&id,4);
if(id==tinyid)
{
if (flg!=0)
	{
	y=CueL->list_size;
	for(x=0;x<y;x++)
		{
		le=find_list_entry_by_index(CueL,0);
		ce=(struct CueEntry *)le->user_data;
		FreeMem(ce,sizeof(struct CueEntry));
		delete_list_entry_by_index(CueL,0);
		}
	}
MenuGhost(TRUE);

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue LIST:","Memory Allocation Error (loadF)",NOTICE_RETURN_NOW);}
while ((err=ReadLn(fp,ce))==0)
	{
	line++;
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
MenuGhost(FALSE);
}
else 
	{
	Close(fp);
	telluser("Cue List:","This file does not contain a CueList",NOTICE_RETURN_NOW);
	}

le=find_list_entry_by_index(CueL,0);
ce=(struct CueEntry *)le->user_data;
select_list_entry_by_userdata(CueL,ce);
show_gadget(CueL);
refresh_gadget(CueL);
Signal(task,1<<preload_sigbit);
if(flg) edits=0;
else edits++;
KeyOk=TRUE;
new_pointer(PTR_NORMAL);
}
/**************************************************************/
/**************************************************************/
void SaveF(flg)
int flg;
{
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
int x,id,num;
LONG fp;
char temp[200];
char temp2[200];
int cancel;

if (CueL->list_size==0) {telluser("Cue List:","No cue List to Save",NULL);return;}

KeyOk=FALSE;
if (flg==0)
	{
	cancel=FileRequest("Cue List Save:",((struct CueModState *)(mod->state))->path,file,FR_SINGLE_SELECT);
	x=strlen(file);
	if (stricmp(&file[x-4],".Cue")) strcat(file,".Cue"); 
	if (cancel==0)	{KeyOk=TRUE;return;}
	}

WindTitleChange(wind,file);

strcpy(temp,((struct CueModState *)(mod->state))->path);strcat(temp,file);
if (flg==0)
	{
	fp=Open(temp,MODE_OLDFILE);
	if (fp) if (!askuser("Cuelist:","File Exists, Do you want to replace?",NULL)) {Close(fp);return;}
	if (fp) Close(fp);
	}
new_pointer(PTR_BUSY);
fp=Open(temp,MODE_NEWFILE);
if (fp)	{
	id=tinyid;
	Write(fp,&id,4);

	for(x=0;x<CueL->list_size;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		sprintf(temp2,"%02d:%02d:%02d:%02d",(ce->regn.parms.starttimecode>>24)&0x000FF,(ce->regn.parms.starttimecode>>16)&0x000FF,(ce->regn.parms.starttimecode>>8)&0x000FF,(ce->regn.parms.starttimecode)&0x000FF);
		sprintf(temp,"%-11s ,%+3d ,%s,%s \n",temp2,(ce->regn.parms.volume>>5)-100,ce->path,ce->Rem);
		num=strlen(temp);
		if (Write(fp,temp,num)!=num){telluser("Cue List:","Error Saving Cue List (write)",NOTICE_RETURN_NOW);x=CueL->list_size;}
		}
	Close(fp);
	edits=0;
	}
else telluser("Cue List:","Error Saving Cue List (open)",NOTICE_RETURN_NOW);
KeyOk=TRUE;
new_pointer(PTR_NORMAL);
}
/**************************************************************/
/**************************************************************/
int MyRead(fp,str,num)
LONG fp;
char *str;
LONG num;
{
static int len=0;
static bufr[1025];
int rtn;
if (len<num) 
	{
	rtn=0;
	if (len!=0)
		{
		for (rtn=0; rtn<len; rtn++) bufr[rtn]=bufr[1024-rtn];
		}
	len=len+Read(fp,&bufr[rtn],1024-rtn);
	}
if (len==0) return(0);
if (len<num) 
	{
	rtn=strncpy(str,&bufr[1024-len],len);
	len=len-rtn;
	return(rtn);
	}
else
	{
	rtn=strncpy(str,&bufr[1024-len],num);
	len=len-rtn;
	return(rtn);
	}
}
/**************************************************************/
/**************************************************************/
