/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ActiveSample *PlaySam(samp,x,num)
char *samp;
int x;
{
int ASFLAG=0;
static char str[STUDIO16_MAX_PATH_SIZE+30];

if (samp)
	{
	if (FindRegion(samp,&region[x]) != NO_ERROR)
		{
		sprintf(str,"Can't find sample/region '%s'",samp);
		telluser("Transport:",str,NOTICE_RETURN_NOW);
		return(NULL);
		}

	if (Pflags&MANUAL_RATE) region[x].parms.rate=GlobSampRate;
	else
		{
		if (!ANrate) ANrate=region[x].parms.rate;
		else region[x].parms.rate=ANrate;
		}
	if (Pflags&MANUAL_VOLUME) ASFLAG=AS_IGNORE_DEFAULT_VOLUME;

	if ((as[x]=(struct ActiveSample *)ASOpen(&region[x], AS_PLAY|AS_AUTO_DATA|ASFLAG))!=0)
		{
		as[x]->key=(struct ChanKey *)AllocChan(hand,num,CK_CHAN_PLAY);
		if (as[x]->key==NULL)
			{
			ASClose(as[x]);
			sprintf(str,"Can't Allocate play channel #%d",x);
			telluser("Transport:",str,NOTICE_RETURN_NOW);
			return(NULL);
			}
		}
	else 
		{
		sprintf(str,"Can't open disk sample '%s'",region[x].samp_name);
		telluser("Transport:",str,NOTICE_RETURN_NOW);
		return(NULL);
		}
	}
return(as[x]);
}
/*****************************************************************/
