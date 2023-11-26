#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "intuition/intuition.h"
#include "clib/macros.h"
#include "/include/psound.h"
#include "cl.h"
#include "Convert.h"

extern struct NewBox SampleName;
extern struct Task *task;
extern long preload_sigbit;
extern int edits;
/**************************************************************/
extern struct StudioBase *StudioBase;
extern UBYTE SG_StartTime[];
extern UBYTE SG_Volume[];
extern UBYTE SG_SampName[];
extern UBYTE SG_Remark[];
extern struct list *CueL;
/**************************************************************/
void FillInStruct();
/**************************************************************/
void ADDEntry(regn,remark)
struct Region *regn;
char *remark;
{
void Gad2Struct();
void Struct2Line();
struct CueEntry *ce;

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue List:","Memory Allocation Error",NOTICE_RETURN_NOW);return;}

ce->regn=*regn;
ce->Rstart=ce->regn.start;
strcpy(ce->Rem,remark);
FillInStruct(ce);

Struct2Line(ce);

add_list_entry(CueL,ce->line,ce);
select_list_entry_by_userdata(CueL,ce);
scroll_to_show(CueL,get_list_entry_index(CueL,find_list_entry_by_userdata(CueL,ce)));
Signal(task,1<<preload_sigbit);
edits++;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void Gad2Struct(ce)
struct CueEntry *ce;
{
ULONG x;
int q,flag;

/*==========*/
MyFindRegion(SG_SampName,ce);
/*==========*/
q=strlen(SG_StartTime);
flag=0;
for(x=0;x<q && flag==0;x++) 
	{
	if(SG_StartTime[x]>='0' && SG_StartTime[x]<='9') flag=2;
	if(SG_StartTime[x]=='+') flag=1;
	if(SG_StartTime[x]=='-') flag=-1;
	}
if (flag==2) flag=0;
if (flag==0) ce->regn.parms.starttimecode=TCstr2int(SG_StartTime);
if (flag==1) ce->regn.parms.starttimecode=AddTimeCode(ce->regn.parms.starttimecode,TCstr2int(SG_StartTime),StudioBase->defaults.smpte_mode);
if (flag==-1) ce->regn.parms.starttimecode=SubTimeCode(ce->regn.parms.starttimecode,TCstr2int(SG_StartTime),StudioBase->defaults.smpte_mode);
/*==========*/
q=0;flag=0;
for (x=0;x<3;x++)
	{
	if(SG_Volume[x]=='-') flag=1;
	if(SG_Volume[x]>='0' && SG_Volume[x]<='9') 
		q=q*10+(SG_Volume[x]-'0');
	}
if (flag==1) q=-q;

if (q>6) q=6;
if (q<-99) q=-99;
ce->regn.parms.volume=(q+100)<<5;
/*==========*/
strcpy(ce->Rem,SG_Remark);
/*==========*/
}
/**************************************************************/
char *StrFileName();
/**************************************************************/
void Struct2Line(ce)
struct CueEntry *ce;
{
struct TimeCode *StartTime=(struct TimeCode *)&ce->regn.parms.starttimecode;
static char tem[80];
int x,n,m;

for (x=0;x<100;x++) ce->line[x]=' ';

sprintf(tem,"%02d:%02d:%02d:%02d",StartTime->hours,StartTime->minutes,StartTime->seconds,StartTime->frames);
movmem(tem,&ce->line[0],11);
/*==========*/
if (ce->regn.parms.rate) /* does it exist? */
	{
	if (ce->regn.parms.smpte_sampling_rate) n=(int)ce->regn.parms.smpte_sampling_rate;
	else n=ce->regn.parms.rate;
	m=0; if(ce->regn.end) m=ce->regn.end-ce->regn.start-1;
	if (n!=0 && m!=0)
		{
		ce->end=Sample2TimeCode(m,n,StudioBase->defaults.smpte_mode);
		ce->end=AddTimeCode(ce->end,ce->regn.parms.starttimecode,StudioBase->defaults.smpte_mode);
		sprintf(tem,"%02d:%02d:%02d:%02d",(ce->end>>24)&0x0FF,(ce->end>>16)&0x0FF,(ce->end>>8)&0x0FF,(ce->end)&0x0FF);
		movmem(tem,&ce->line[13],11);
		}
	else
		{
		movmem("-E:RR:OR:--",&ce->line[13],11);
		ce->end=0;
		}
	}
else
	{
	movmem("  :  :  :  ",&ce->line[13],11);
	ce->end=0;
	}
/*==========*/
sprintf(tem,"%+3d",(ce->regn.parms.volume>>5)-100);
movmem(tem,&ce->line[26],3);
/*==========*/
if (ce->path[0]) sprintf(tem,"%-17s",StrFileName(ce->path));
else sprintf(tem,"%-17s",&" ");
movmem(tem,&ce->line[31],17);
/*==========*/
sprintf(tem,"%-40s",ce->Rem);
movmem(tem,&ce->line[50],40);
ce->line[100]=NULL;
}
/**************************************************************/
/**************************************************************/
void Struct2Gad(ce)
struct CueEntry *ce;
{
char tem[80];
struct TimeCode *StartTime=(struct TimeCode *)&ce->regn.parms.starttimecode;

sprintf(tem,"%02d:%02d:%02d:%02d",StartTime->hours,StartTime->minutes,StartTime->seconds,StartTime->frames);
movmem(tem,SG_StartTime,12);
sprintf(tem,"%+3d",(ce->regn.parms.volume>>5)-100);
movmem(tem,SG_Volume,3);
SG_SampName[0]=NULL;
set_box_text(SampleName.gad_ptr,NULL);
if (ce->path[0]) 
	{
	strcpy(SG_SampName,StrFileName(ce->path));
	set_box_text(SampleName.gad_ptr,ce->path);
	}
strcpy(SG_Remark,ce->Rem);
}
/**************************************************************/
/**************************************************************/
int GadStructCmp(ce)
struct CueEntry *ce;
{
if (strnicmp(SG_StartTime,ce->line,11)) return(1);
if (strnicmp(SG_Volume,&ce->line[26],3)) return(3);
if (stricmp(SG_SampName,StrFileName(ce->path))) return(4);
if (stricmp(SG_Remark,ce->Rem)) return(5);
return(NULL);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
int DBstr2int(str)
char *str;
{
int q,flag,x;

q=0;flag=0;
for (x=0;x<3;x++)
	{
	if(str[x]=='-') flag=1;
	if(str[x]>='0' && str[x]<='9') 
		q=q*10+(str[x]-'0');
	}
if (flag==1) q=-q;

if (q>6) q=6;
if (q<-99) q=-100;
return((q+100)<<5);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/******************void SetEndTimes()
{
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce,*ces;
int x,m,n;
char tem[100];

hide_gadget(CueL);

ces=(struct CueEntry *)CueL->active_entry->user_data;

for(x=0;x<CueL->list_size;x++)
	{
	le=find_list_entry_by_index(CueL,x);
	ce=(struct CueEntry *)le->user_data;

	if (ce->regn.parms.rate)
		{
 		if (ce->regn.parms.smpte_sampling_rate) n=ce->regn.parms.smpte_sampling_rate;
		else n=ce->regn.parms.rate;
		m=0; if(ce->regn.end) m=ce->regn.end-ce->regn.start-1;
		if (n!=0 && m!=0)
			{
			ce->end=Sample2TimeCode(m,n,StudioBase->defaults.smpte_mode);
			ce->end=AddTimeCode(ce->end,ce->regn.parms.starttimecode,StudioBase->defaults.smpte_mode);
			sprintf(tem,"%02d:%02d:%02d:%02d",(ce->end>>24)&0x0FF,(ce->end>>16)&0x0FF,(ce->end>>8)&0x0FF,(ce->end)&0x0FF);
			movmem(tem,&ce->line[13],11);
			}
		else
			{
			ce->end=0;
			movmem("-E:RR:OR:--",&ce->line[13],11);
			}
		}
	else
		{
		ce->end=0;
		movmem("  :  :  :  ",&ce->line[13],11);
		}
	delete_list_entry_by_userdata(CueL,ce);
	add_list_entry(CueL,ce->line,ce);
	}

show_gadget(CueL);
refresh_gadget(CueL);
select_list_entry_by_userdata(CueL,ces);
}
*********************/

