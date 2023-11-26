/* subsed340.c */
#include "sed.include.h"
#include "sed.externals"

void set_edit();
float get_fvalue();
void clear_voices();
extern int inabs();
/* goodbye */
cleanUp(message)
TEXT *message;
{
int k;
	ClearMenuStrip(Window);
	CloseWindow(Window); 
	flush_channel();
	CloseDevice((struct IOExtSer *)&IORser);
	CloseDevice(audcom[curcom]);
	CloseDevice(&tr);
    	DeletePort(port);
	DeletePort(allocPort);
	DeletePort(soundPort);
	FreeRemember(&RememberKey,TRUE);
	CloseLibrary(GfxBase);
	CloseLibrary(IntuitionBase);
	exit(0);
	return;
}


begplot(buffer,begsamp)
char *buffer;
long int begsamp;

   {
   int mid=128;
   int cx=20;
   int amp;
   long int nn,left,right;


   SetAPen(rastPort,WHITE);
   RectFill(rastPort,cx,102,cx+PLOT_WIDTH,157);

   SetAPen(rastPort,BLACK);
   Move(rastPort,cx,mid);
   Draw(rastPort,cx+PLOT_WIDTH,mid);
   SetAPen(rastPort,YELLOW);
   Move(rastPort,cx+PLOT_WIDTH/2,157);
   Draw(rastPort,cx+PLOT_WIDTH/2,102);
   SetAPen(rastPort,BLACK);

   left=begsamp-PLOT_WIDTH/2;
   right=begsamp+PLOT_WIDTH/2;
   if(left < 0)
      {
      left=1;
      right=PLOT_WIDTH;
      }
   if(right >nsamps)
      {
      right=nsamps;
      left=nsamps-PLOT_WIDTH;
      if(left<0) left=0;
      }
   for(nn=left;nn<right;nn++)
         {
         amp=mid-buffer[nn]/5;
         Move(rastPort,cx,mid);
         Draw(rastPort,cx,amp);
         cx++;
         }
   } /* end of begplot */

endplot(buffer,endsamp)
char *buffer;
long int endsamp;
   {
   int mid=128;
   int cx=90;
   int amp;
   long int nn,left,right;
 
   SetAPen(rastPort,WHITE);
   RectFill(rastPort,cx,102,cx+PLOT_WIDTH,157);
   SetAPen(rastPort,BLACK);
   Move(rastPort,cx,mid);
   Draw(rastPort,cx+PLOT_WIDTH,mid);
   SetAPen(rastPort,YELLOW);
   Move(rastPort,cx+PLOT_WIDTH/2,157);
   Draw(rastPort,cx+PLOT_WIDTH/2,102);

   SetAPen(rastPort,BLACK);

   left=endsamp-PLOT_WIDTH/2;
   right=endsamp+PLOT_WIDTH/2;
   if(left < 0)
      {
      left=1;
      right=PLOT_WIDTH;
      }
   if(right >nsamps)
      {
      right=nsamps;
      left=nsamps-PLOT_WIDTH;
	if(left<0) left=0;
      }
   for(nn=left;nn<right;nn++)
         {
         amp=mid-buffer[nn]/5;
         Move(rastPort,cx,mid);
         Draw(rastPort,cx,amp);
         cx++;
         }
   } /* end of endplot */

strfloat(DestStr,Value)
char *DestStr;
float Value; /* bug in < .01 case */
   {
   int ipart,fpart,n;
   char temp1[20],temp2[20];
   ipart=Value;
   fpart=Value*100000-ipart*100000.;
   n = stcu_d(temp1,ipart,10);
   n = stcu_d(temp2,fpart,10);
   n = strcat(temp1,".\0");
for(m=4;m>0;m--) 
	if(pow(10.,(float)m)>fpart) strcat(temp1,"0\0");
   n = strcat(temp1,temp2);
   n = strcpy(DestStr,temp1);
   }


display(x,y,nletters)
int x,y,nletters;
   {
   /* print the TempMeterText structure, which displays the TempStr[] */
   SetAPen(rastPort,BLACK);
   RectFill(rastPort,x,y,x+nletters*8,y+6);
   SetAPen(rastPort,YELLOW);
   Move(rastPort,x-1,y-1);
   Draw(rastPort,x+nletters*8+1,y-1);
   Draw(rastPort,x+nletters*8+1,y+7);
   Draw(rastPort,x-1,y+7);
   Draw(rastPort,x-1,y-1);
   strncpy(TempStr,TempStr,nletters); /* DANGER?!?!? */
   PrintIText(rastPort,&TempMeterText,x,y);
   }

 /* SERIAL I/O functions */


readSer(data,length)
    char *data;
    ULONG length;
{
    int error;

    IORser.IOSer.io_Data = data;
    IORser.IOSer.io_Length = length;
    IORser.IOSer.io_Command = CMD_READ;
    if (++nser == MAXSER)
   nser = 0;
    TempIORser[nser] = IORser;   
    DoIO (&TempIORser[nser]);
    return (TempIORser[nser].IOSer.io_Actual);
}
writeSer(data,length)
    char *data;
    ULONG length;
{
    int error;

    IORser.IOSer.io_Data = data;
    IORser.IOSer.io_Length = length;
    IORser.IOSer.io_Command = CMD_WRITE;
    if (++nser == MAXSER)
   nser = 0;
    TempIORser[nser] = IORser;   
    DoIO (&TempIORser[nser]);
    return (TempIORser[nser].IOSer.io_Actual);
}

querySer()
{
    int     error;

    IORser.IOSer.io_Command = SDCMD_QUERY;
    if (++nser == MAXSER)
   nser = 0;
   TempIORser[nser] = IORser;       
    if ((error = DoIO (&TempIORser[nser])) != 0) {
        CloseDevice(&IORser);
        exit(1);
    }
    return(TempIORser[nser].IOSer.io_Actual);
}


setparams(rbuf_len,rlen,wlen,brk,baud,sf,ta0,ta1)

    unsigned long rbuf_len;
    unsigned char rlen;
    unsigned char wlen;
    unsigned long brk;
    unsigned long baud;
    unsigned char sf;
    unsigned long ta0;
    unsigned long ta1;

{
    int error;


    IORser.io_ReadLen       = rlen;
    IORser.io_BrkTime       = brk;
    IORser.io_Baud          = baud;
    IORser.io_WriteLen      = wlen;
    IORser.io_StopBits      = 0x01;
    IORser.io_RBufLen       = rbuf_len;
    IORser.io_SerFlags      = sf;
    IORser.IOSer.io_Command = SDCMD_SETPARAMS;
    IORser.io_TermArray.TermArray0 = ta0;
    IORser.io_TermArray.TermArray1 = ta1;

    if ((error = DoIO (&IORser)) != 0) {
        exit (1);
    }
    return (error);
}

int cue(tit)
TEXT *tit;
{
int response;
BodyText.IText=tit;
BodyText.LeftEdge=4+20-strlen(tit)/2;
PosText.IText="Okay";
NegText.IText="Stop";
response=AutoRequest(Window,&BodyText,&PosText,&NegText,NULL,NULL,400,80);
return(response);
}

reverse(s1,s2)
long s1,s2;
   {
   int store;
   long k,l,mid;
   mid=s1+(s2-s1)/2;
   l=s2;
   for(k=s1;k<mid;k++)
      {
      l--;
      store=buffer[k];
      buffer[k]=buffer[l];
      buffer[l]=store;
      }
   }


post(tit)
TEXT *tit;
   {
   PostItText.IText = tit;
   PostItText.LeftEdge = (200-strlen(tit)*8)/2;   
   Request(&PostIt,Window);
   }

unpost()
   {
   EndRequest(&PostIt,Window);
   }   
 
left_slide(st,dist)
long st,dist;
   {
/*   for(l=(st-dist);l<nsamps-dist;l++)
      buffer[l]=buffer[l+dist]; */
movmem(&buffer[st],&buffer[st-dist],(nsamps-st));

   nsamps = nsamps - dist;
if(copy_buffer==current_buffer) {
	m=copy_start;
	l=st-dist;
	if((m>=l) && (m<=st)) copy_end=2;
	m=copy_end;
	if((m>=l) && (m<=st)) copy_end=2;
	if(copy_start>=st) copy_start-=dist;
	if(copy_end>=st) copy_end-=dist;
	}
for(k=0;k<nsects;k++)
   {
if(section[k].currbuf == current_buffer) {
m=section[k].firstsamp;
l=st-dist;
if((m>=l) && (m<=st)) remove_section();
m=section[k].lastsamp;
if((m>=l) && (m<=st)) remove_section();
   if(section[k].lastsamp != 2)
      		{
	      if(section[k].firstsamp >= st) 
        	 section[k].firstsamp=section[k].firstsamp - dist;   
	      if(section[k].lastsamp >= st)
        	 section[k].lastsamp=section[k].lastsamp - dist;   
	      if(section[k].loopstart >= st)
        	 section[k].loopstart=section[k].loopstart - dist;   
	      if(section[k].loopend >= st)
        	 section[k].loopend=section[k].loopend - dist;   
      		}
	}
	  }
  return;   
   }

right_slide(st,dist)
   long st,dist;
   {
   if(nsamps+dist > bufsiz)
   {
   nsamps=bufsiz-dist; /* this means we lose data at end of buffer ! */
   if(nsamps<=st)
	return;
   }
/*   for(l=nsamps;l>st;l--)
	   buffer[l+dist]=buffer[l]; */
movmem(&buffer[st],&buffer[st+dist],(nsamps-st));
   nsamps=nsamps+dist;      
if(copy_buffer==current_buffer)
	if(copy_start>st) {
		copy_start+=dist;
		copy_end+=dist;
		}

   for(k=0;k<nsects;k++)
   {
if(section[k].currbuf == current_buffer)
   if(section[k].lastsamp != 2)
      {
      if(section[k].firstsamp >= st)
         section[k].firstsamp=section[k].firstsamp + dist;   
      if(section[k].lastsamp >= st)
         section[k].lastsamp=section[k].lastsamp + dist;   
      if(section[k].loopstart >= st)
         section[k].loopstart=section[k].loopstart + dist;   
      if(section[k].loopend >= st)
         section[k].loopend=section[k].loopend + dist;   
      }
   }
   return;
   }

paste()
{
   /* assumes that copy_start/end were set by "MARK" and puts a
      copy of those samples starting at lastmoved slider */
	if(lastbuffer != current_buffer) paste_start=nsamps;
else paste_start = ((long)LimitInfo[lastmoved].HorizPot*(long)nsamps/65535);
	FloatJunk=(float)LimitInfo[lastmoved].HorizPot/65535.;
	paste_start=nsamps*FloatJunk;
       paste_end = paste_start+(copy_end-copy_start);
       if(paste_end > bufsiz)
                {
                paste_end=bufsiz;
                copy_end=copy_start+(paste_end-paste_start);
                }
       if(paste_end>nsamps)
      {
      nsamps=paste_end;
      }
       dif=copy_start - paste_start;
       if (dif < 0)
                {
                l=copy_end;
                while(l > copy_start)
	                dest[paste_end--]=orig[l--];
                }
       else
               {
               l=copy_start;
               while(l < copy_end)
	               dest[paste_start++]=orig[l++];
               }
}

draw_key(number,color)
	short number,color;
	/* color is Boolean -- 1=TRUE=YELLOW   0=FALSE=NORMAL */
	{
	int m,n,x,y;	
	m=number-36;
	x=KEY_X + m*(KEY_WIDTH+2);
	y=KEY_Y + 2*KEY_HEIGHT/3;
	n=(m+12)%12;
	SetAPen(rastPort,YELLOW);
	switch(n)
		{
		case 0: case 2: case 4: case 5: case 7: case 9: case 11:
		if(color == 0)
			SetAPen(rastPort,WHITE);
		break;
		default:
		/* black keys!*/
		if(color == 0)
			SetAPen(rastPort,BLACK);
		break;
		}
	RectFill(rastPort,x,KEY_Y,x+KEY_WIDTH,y);
	if(color == 0)
		SetAPen(rastPort,WHITE);
	switch(n)
		{
		case 0: case 5:
		RectFill(rastPort,x,y+1,x+2*KEY_WIDTH-1,y+KEY_HEIGHT/3);
		break;
		case 4: case 11:
		RectFill(rastPort,x-KEY_WIDTH+1,y+1,x+KEY_WIDTH,y+KEY_HEIGHT/3);
		break;
		case 2: case 7: case 9:
		RectFill(rastPort,x-KEY_WIDTH+1,y+1,x+2*KEY_WIDTH-1,y+KEY_HEIGHT/3);
		break;
		}
	}	

flush_channel()
	{
        audcom[curcom]->ioa_Request.io_Command = CMD_FLUSH;
        audcom[curcom]->ioa_Request.io_Unit = (1<<nvoice);
        BeginIO(audcom[curcom]);   
	find_com();
	}

buffer_toggle()
	{
	if (current_buffer == 0)
		{
		current_buffer = 1;
		buffer = buffer1;
		stereo_offset=buffer0-buffer1;
		nsamps0 = nsamps;
		nsamps = nsamps1;
		bs0 = begsamp;
		begsamp = bs1;
		es0 = endsamp;
		endsamp = es1;
		}
	else
		{
		current_buffer = 0;
		buffer = buffer0;
		stereo_offset=buffer1-buffer0;
		nsamps1 = nsamps;
		nsamps = nsamps0;
		bs1 = begsamp;
		begsamp = bs0;
		es1 = endsamp;
		endsamp = es0;
		}
	}		

open_midi()
	{
	if(midi_flag == 1)
		return;
    	midi_flag = 1;
    	if ((error = OpenDevice (SERIALNAME, 0, &IORser, 0)) != 0) 
        	cleanUp("bad news %ld on Open \n", error);
 	if ((error = setparams (rbl,rwl,wwl,brk,baud,sf,t0,t1)) != 0) 
        	cleanUp("Bad setparam.",error);
        MiscText[0].IText = "Disable MIDI";
	}

close_midi()
	{
	if(midi_flag == 0)
		return;
	CloseDevice(&IORser);
	midi_flag = 0;
        MiscText[0].IText = "Enable MIDI";
	}

SPrint(rp,t,x,y,le)
struct RastPort *rp;
TEXT *t;
int x,y,le;
	{
strncpy(SPrintStr,t,le);
PrintIText(rp,&FNText,x,y);
	}	

void set_edit()
	{
	Delay(40);
	orig=buffer;
	dest=buffer;
	if(current_buffer != copy_buffer)
		{
		buffer_toggle();
		orig=buffer;
		buffer_toggle();
		}
	}


overlay()
	{
                    	paste_start = (LimitInfo[lastmoved].HorizPot*nsamps/65535.);
                     	paste_end = paste_start+(copy_end-copy_start);
                    	if(paste_end > bufsiz)
	                       	{
                        	paste_end=bufsiz;
                        	copy_end=copy_start+(paste_end-paste_start);
                        	}
          	 	if(paste_end > nsamps)
         			{
	 			for(l=nsamps;l<paste_end;l++)
				buffer[l]=0;
         			nsamps=paste_end;
         			}
    	                if (copy_start < paste_start)
                        	{
                        	l=copy_end;
                        	m=paste_end;
				sot_edit();
				preserve(copy_start,paste_end);
                        	maxamp=0;
                        	while(l > copy_start)
                           		{
    	                  		if((k = inabs(dest[m--]+orig[l--])) > maxamp)
                              		maxamp=k;
                           		}
	/* make this fixed point instead ! */
                        	if(maxamp>126)
                           		maxamp = (64*126/maxamp);
                        	else
                           		maxamp = 64;
                        	l=copy_end;
                        	m=paste_end;
                        	while(l > copy_start)
                           		{
				k=(orig[l]+dest[m])*maxamp;
				if(k<0) dest[m] = 0-((0-k)>>6);
				else dest[m] = k>>6;
					m--;
					l--;
                           		}
                        	}
                     	else
                        	{
                        	l=copy_start;
                        	m=paste_start;
				sot_edit();
                        	maxamp=0;
				preserve(paste_start,copy_end);
                        	while(l < copy_end)
                           	if((k = inabs(dest[m++]+orig[l++])) > maxamp)
                              	maxamp=k;
                        	if(maxamp > 126)
                           		rescale = 126./maxamp;
                        	else
                           		rescale = 1;
                        	l=copy_start;
                        	m=paste_start;
                        	while(l < copy_end)
                           		{
                           	dest[m] = (orig[l++]+dest[m++])*rescale;
                           		}
                        	}
	}

float get_fvalue(tit,tot,hi,lo,def)
TEXT *tit,*tot;
float hi,lo,def;
{
float value; 
		value=def;
		strfloat(TempStr,value);
		display(410,143,10);
		SPrint(rastPort,SpaceStr,310,143,10);
		SPrint(rastPort,tot,310,143,10);
                ValueText.IText=tit;
		LimitInfo[5].HorizPot=65535*(1.-((value-lo)/(hi-lo)));
                 Request(&SliderRequest,Window);
                 reqflag=0;
                 while(reqflag == 0)
  	             {
              Wait(1 << Window->UserPort->mp_SigBit);   
              while(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
                       {
                       class = message->Class;
		   	address = message->IAddress;
                       ReplyMsg(message);
			if (class == GADGETDOWN) {
				if (address == (APTR)&DoneGadget) {
					reqflag=1;
					}
				if (address == (APTR)&QuitGadget) {
					reqflag=1;
					value=-1.;
					}
				} /* GADGETDOWN */
			if (class == GADGETUP) {
				if (address == (APTR)&LimitGadget[5]) {
value=lo+(double)(1.-((float)(LimitInfo[5].HorizPot)/65535.))*(hi-lo);
	n=value;
					strfloat(TempStr,value);
					display(410,143,10);
					SPrint(rastPort,SpaceStr,310,143,10);
					SPrint(rastPort,tot,310,143,10);
				}
                       } /* GADGETUP */
                    } /* mesg */
		} /* reqflag == 0 */
	return(value);
    }


int get_ivalue(tit,tot,hi,lo,def)
TEXT *tit,*tot;
int hi,lo,def;
	{
	int value;
		value=def;
                ValueText.IText=tit;
		n=stcu_d(TempStr,value,10);
		SPrint(rastPort,SpaceStr,310,158,10);
		SPrint(rastPort,tot,310,158,10);
		display(410,158,10);
	LimitInfo[5].HorizPot=65535*(1.-((float)(value-lo)/(float)(hi-lo)));
                Request(&SliderRequest,Window);
                 reqflag=0;
                 while(reqflag == 0)
  	             {
              Wait(1 << Window->UserPort->mp_SigBit);   
              while(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
                       {
                       class = message->Class;
		   	address = message->IAddress;
                       ReplyMsg(message);
			if (class == GADGETDOWN) {
				if (address == (APTR)&DoneGadget) {
					reqflag=1;
					}
				if (address == (APTR)&QuitGadget) {
					reqflag=1;
					value=-1;
					}
				} /* GADGETDOWN */
			if (class == GADGETUP) {
				if (address == (APTR)&LimitGadget[5]) {
		value=(int)(lo+(1.-(LimitInfo[5].HorizPot/65535.))*(hi-lo));
				n=stcu_d(TempStr,value,10);
				SPrint(rastPort,SpaceStr,310,158,10);
				SPrint(rastPort,tot,310,158,10);
					display(410,158,10);
				}
                       } /* GADGETUP */
                    } /* mesg */
		} /* reqflag == 0 */
	return(value);
    }

int get_pitch_value(tit)
TEXT *tit;
	{
	int value;
	value=67;
                ValueText.IText=tit;
		LimitInfo[5].HorizPot=65535./2.;
                Request(&SliderRequest,Window);
		draw_key(60,TRUE);
		SPrint(rastPort,"Freq. Mult.:",310,143,10);

                 reqflag=0;
                 while(reqflag == 0)
  	             {
              Wait(1 << Window->UserPort->mp_SigBit);   
              while(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
                       {
                       class = message->Class;
		   	address = message->IAddress;
                       ReplyMsg(message);
			if (class == GADGETDOWN) {
				if (address == (APTR)&DoneGadget) {
					reqflag=1;
					}
				if (address == (APTR)&QuitGadget) {
					reqflag=1;
					value=-1.;
					}
				} /* GADGETDOWN */
			if (class == GADGETUP) {
				if (address == (APTR)&LimitGadget[5]) {
                     		value=(LimitInfo[5].HorizPot/65535.)*(64.)+36;
		     		for(k=36;k<=100;k++) draw_key(k,FALSE);
			     draw_key(60,TRUE);
			     draw_key(value,TRUE);	
			FloatJunk=pow(2.0,(value-60.)/12.);
					strfloat(TempStr,FloatJunk);
					display(410,143,10);
				}
                       } /* GADGETUP */
                    } /* mesg */
		} /* reqflag == 0 */
	for(k=36;k<=100;k++) draw_key(k,FALSE);
	return(value);
    }

void clear_voices()
	{
       audcom[curcom]->ioa_Request.io_Command = CMD_FLUSH;
       audcom[curcom]->ioa_Request.io_Unit = 15;
        BeginIO(audcom[curcom]);   
	for(k=0;k<4;k++)
		used[k]=0;
}

