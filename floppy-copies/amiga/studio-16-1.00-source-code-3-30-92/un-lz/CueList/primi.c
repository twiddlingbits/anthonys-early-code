#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "intuition/intuition.h"
#include "clib/macros.h"
#include "/include/psound.h"
#include "cl.h"
#include "clib/macros.h"

/**************************************************************/
extern struct StudioBase *StudioBase;
extern UBYTE SG_StartTime[];
extern UBYTE SG_Volume[];
extern UBYTE SG_SampName[];
extern UBYTE SG_Remark[];
extern struct list *CueL;
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void Gad2Struct(ce)
struct CueEntry *ce;
{
struct TimeCode *StartTime=(struct TimeCode *)&ce->Start;
ULONG v,x,y,c,z[12]={0,0,0,0,0,0,0,0,0,0,0,0};
int q,flag;
char tem[80];

/*==========*/

y=0;c=0;v=0;
for (x=0;x<13;x++)
	{
	if ((SG_StartTime[x]>='0' && SG_StartTime[x]<='9')) {y=y*10; y=y+SG_StartTime[x]-'0';v++;}
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

movmem(tem,ce->StartTimeStr,12);
movmem(tem,SG_StartTime,12);

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
sprintf(tem,"%+3d",q);

movmem(tem,ce->VolStr,3);
movmem(tem,SG_Volume,3);
ce->Vol=q;

/*==========*/
movmem(SG_SampName,ce->Sample,40);
movmem(SG_Remark,ce->Rem,40);
/*==========*/
}
/**************************************************************/
#include "Convert.h"
/**************************************************************/
void Struct2Line(ce)
struct CueEntry *ce;
{
struct Disk_Samp *samp,*FindSample();
static char tem[80];
struct TimeCode *tc;
int x;

tc=(struct TimeCode *)&ce->End;

for (x=0;x<100;x++) ce->line[x]=' ';

movmem(ce->StartTimeStr,&ce->line[0],11);
/*==========*/
if (samp=FindSample(ce->Sample,-1))
	{
	ce->End=Sample2TimeCode(samp->length,samp->parms.rate,StudioBase->defaults.smpte_mode);
	ce->End=AddTimeCode(ce->End,ce->Start,StudioBase->defaults.smpte_mode);
	sprintf(tem,"%02d:%02d:%02d:%02d",tc->hours,tc->minutes,tc->seconds,tc->frames);
	movmem(tem,&ce->line[13],11);
	}
else
	{
	sprintf(tem,"  :  :  :  ");
	movmem(tem,&ce->line[13],11);
	}
/*==========*/
movmem(ce->VolStr,&ce->line[26],3);
/*==========*/
sprintf(tem,"%-17s",ce->Sample);
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
movmem(ce->StartTimeStr,SG_StartTime,12);
movmem(ce->VolStr,SG_Volume,3);
movmem(ce->Sample,SG_SampName,40);
movmem(ce->Rem,SG_Remark,40);
}
/**************************************************************/
/**************************************************************/
int GadStructCmp(ce)
struct CueEntry *ce;
{
if (strcmp(SG_StartTime,ce->StartTimeStr)) return(1);
if (strcmp(SG_Volume,ce->VolStr)) return(3);
if (strcmp(SG_SampName,ce->Sample)) return(4);
if (strcmp(SG_Remark,ce->Rem)) return(5);
return(NULL);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void SetEndTimes()
{
struct Disk_Samp *samp,*FindSample();
struct list_entry *le,*find_list_entry_by_index();
struct TimeCode *tc;
struct CueEntry *ce,*ces;
int x,m,n;
char tem[100];

hide_gadget(CueL);

ces=(struct CueEntry *)CueL->active_entry->user_data;

for(x=0;x<CueL->list_size;x++)
	{
	le=find_list_entry_by_index(CueL,x);
	ce=(struct CueEntry *)le->user_data;
	tc=(struct TimeCode *)&ce->End;

	if (samp=FindSample(ce->Sample,-1)) 
		{
		n=samp->parms.rate;
		m=samp->length;
		if (n!=0)
			{
			ce->End=Sample2TimeCode(m,n,StudioBase->defaults.smpte_mode);
			ce->End=AddTimeCode(ce->End,ce->Start,StudioBase->defaults.smpte_mode);
			sprintf(tem,"%02d:%02d:%02d:%02d",tc->hours,tc->minutes,tc->seconds,tc->frames);
			movmem(tem,&ce->line[13],11);
			}
		}
	else
		{
		sprintf(tem,"  :  :  :  ");
		movmem(tem,&ce->line[13],11);
		}
	delete_list_entry_by_userdata(CueL,ce);
	add_list_entry(CueL,ce->line,ce);
	}

show_gadget(CueL);
refresh_gadget(CueL);
select_list_entry_by_userdata(CueL,ces);
}
