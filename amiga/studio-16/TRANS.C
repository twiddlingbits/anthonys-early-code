/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/


void PlaySamples()
{
char  *samp;
int x,y;
void OpenSmpteWaitWindow();
void CloseSmpteWaitWindow();
long mask=0;

GlobSampRate=hand->current_sampling_rate;
LastPlayRate=hand->current_sampling_rate;
ANrate=0;

for(x=0;x<NumTracks;x++) {if (as[x]!=NULL) {telluser("Transport:","Channels Active, Aborting",NULL);return;}}

hide_gadget(PlayBut.gad_ptr);

new_pointer(PTR_BUSY);
x=0;
for(y=0;y<NumTracks;y++)
	{
	samp=((struct box *)(PlayList[y].gad_ptr))->text;
	if (samp)
		{
		as[x]=PlaySam(samp,x,y);
		if (as[x]) 
			{
			x++;
			Playing++;
			}
		}
	}

if (RecBut.gad_ptr->flags&BUTN_HIT)
	{
	if(Rec1But.gad_ptr->flags&BUTN_HIT)
		{
		if (NumRecTracks==1) as[x]=RecSam(RecFileName,x,NULL);
		else as[x]=RecSam(RecFileName,x,CK_CHAN_LEFT);
		if (as[x]) {x++;Playing++;Recording++;}
		}
	if (NumRecTracks>1)
		if(Rec2But.gad_ptr->flags&BUTN_HIT)
			{
			as[x]=RecSam(RecFileName2,x,CK_CHAN_RIGHT);
			if (as[x]) {x++;Playing++;Recording++;}
			}
	}

if (x)
	{
	if (Titles[1].NextMenu) Titles[1].NextMenu->Flags&=~MENUENABLED;
	if (!(Pflags&MANUAL_RATE) && ANrate)
		{
		BroadcastEventParmsLessOne(GetEventID("SE_CMD_SET_SAMPLING_RATE"),mod,hand,ANrate,0, 0, 0,0);
		}

	BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);
	ASTriggerList(as,TRIGGER_LIST_MANUAL_SYNC);

	new_pointer(PTR_NORMAL);

	if (Pflags&SMPTE_START)
		{
		OpenSmpteWaitWindow(StartTC,EndTC);
		SetSignal(0,(1<<sw_abort_sigbit)|(1<<sw_trigger_sigbit));
		mask=Wait((1<<sw_abort_sigbit)|(1<<sw_trigger_sigbit));

		if (mask&(1<<sw_abort_sigbit)) 
			{
			for(x=0;x<NumTracks;x++) 
				if (as[x]) 
					{
					if(!(as[x]->key->flags&CK_CHAN_RECORD))
						ASStop(as[x]);
					}
			}
		}

	BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,0,0,0,0);

	if (Pflags&SMPTE_START) CloseSmpteWaitWindow();

	if (mask&(1<<sw_abort_sigbit)) 
		{
		for(x=0;x<NumTracks;x++) 
			if (as[x]) 
				{
				if(as[x]->key->flags&CK_CHAN_RECORD)
					{
					ASStop(as[x]);
					as[x]->region.parms.filter3db=-2;
					}
				}
		RecSize=0;
		}
	else
		{
		if (Pflags&SMPTE_STOP) NotifyMeOnEvent(se_info_smpte_timeout,mod);
		}

	show_gadget(PauseBut.gad_ptr);
	StopBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);

	if (!(as[Playing-1]->key->flags&CK_CHAN_RECORD))
		{
		RecBut.gad_ptr->flags&=~BUTN_HIT;
		refresh_gadget(RecBut.gad_ptr);
		hide_gadget(RecBut.gad_ptr);
		}
	}
else
	{
	new_pointer(PTR_NORMAL);
	show_gadget(PlayBut.gad_ptr);
	PlayBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PlayBut.gad_ptr);

	StopBut.gad_ptr->flags|=BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);

	PauseBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PauseBut.gad_ptr);
	}

}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/*****************************************************************/
char sn[200];
/*****************************************************************/
struct ActiveSample *RecSam(name,x,flags)
char *name;
int x,flags;
{
int y=1;
static char str[STUDIO16_MAX_PATH_SIZE+40];

if (DiskSpaceAvailable<1) 
	{
	telluser("Transport:","Not enough disk space available to record");
	return(NULL);
	}

name=(char *)StrFileName(name);
sprintf(sn,"%s",name);
while(FindSample(sn,-1))
	{
	y++;
	sprintf(sn,"%s_%d",name,y);
	}

strcpy(region[x].samp_name,StudioBase->defaults.samp_paths);
add(region[x].samp_name,sn);
region[x].start=0;
region[x].end=0;
region[x].user_data=NULL;
region[x].parms.filter3db=hand->current_filter;
region[x].parms.volume=3200;
region[x].parms.pan=ZERO_DB;
region[x].parms.flags=0;
if (flags&CK_CHAN_LEFT) region[x].parms.pan=0;
if (flags&CK_CHAN_RIGHT) region[x].parms.pan=200<<5;
if (Pflags&MANUAL_RATE) region[x].parms.rate=GlobSampRate;
else region[x].parms.rate=LastPlayRate;

if ((as[x]=(struct ActiveSample *)ASOpen(&region[x], AS_RECORD|AS_AUTO_DATA))!=0)
	{
	if (Pflags&RECORD_OUTPUT)
		{as[x]->region.parms.flags|=SI_TAP_OUTPUT;as[x]->key=(struct ChanKey *)AllocChan(hand,-1,CK_CHAN_RECORD|CK_TAP_OUTPUT|flags);}
	else 	{as[x]->region.parms.flags|=SI_TAP_INPUT;as[x]->key=(struct ChanKey *)AllocChan(hand,-1,CK_CHAN_RECORD|CK_TAP_INPUT|flags);}
	if (as[x]->key==0)
		{
		ASClose(as[x]);
		telluser("Transport:","Can't Allocate a record channel!",NOTICE_RETURN_NOW);
		DeleteSample(region[x].samp_name);
		return(NULL);
		}
	}
else {sprintf(str,"Can't open record sample '%s'",region[x].samp_name);telluser("Transport:",str,NOTICE_RETURN_NOW);return(NULL);}


return(as[x]);
}
