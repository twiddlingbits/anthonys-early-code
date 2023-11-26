	/* subsed40.c */
#include "sed.include.h"
void midi_sort();
int midi_parse();
long playtime();

typedef LONG Fixed;

extern char *get_fname();
extern float get_fvalue();

#include "sed.externals"



/*get_string(tit)
TEXT *tit;
   {
                 ReqText.IText=tit;
                 Request(&StringRequest,Window);
                 reqflag=0;
                 while(reqflag == 0)
                    {
                    Wait(1 << Window->UserPort->mp_SigBit);
                    while(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
                       {
                       class = message->Class;
                       ReplyMsg(message);
                       if (class == REQCLEAR)
                       reqflag=1;
                       }
                    }
    }

*/

play()
{
if(comlength<8) return;
audcom[curcom]->ioa_Request.io_Unit = (1<<nvoice);
audcom[curcom]->ioa_Period = pper;
audcom[curcom]->ioa_Cycles = ncycles;
audcom[curcom]->ioa_Data = comdata;
audcom[curcom]->ioa_Length = comlength;
audcom[curcom]->ioa_Volume = comvolume;
audcom[curcom]->ioa_Request.io_Command = command;

BeginIO(audcom[curcom]);
if(find_com() == 0)
	{
        audcom[curcom]->ioa_Request.io_Command = CMD_FLUSH;
        audcom[curcom]->ioa_Request.io_Unit = 15;
        BeginIO(audcom[curcom]);   
	for(m=0;m<4;m++)
		used[m]=0;
	find_com();
	}
if(stereo_flag) {
next_channel();
next_channel(); /* attempt stereo?*/
audcom[curcom]->ioa_Request.io_Unit = (1<<nvoice);
audcom[curcom]->ioa_Period = pper;
audcom[curcom]->ioa_Cycles = ncycles;
audcom[curcom]->ioa_Data = comdata+stereo_offset;
audcom[curcom]->ioa_Length = comlength;
audcom[curcom]->ioa_Volume = comvolume;
audcom[curcom]->ioa_Request.io_Command = command;

BeginIO(audcom[curcom]);
if(find_com() == 0)
	{
        audcom[curcom]->ioa_Request.io_Command = CMD_FLUSH;
        audcom[curcom]->ioa_Request.io_Unit = 15;
        BeginIO(audcom[curcom]);   
	for(m=0;m<4;m++)
		used[m]=0;
	find_com();
	}
   }/* end of stereo condition */
}

change_section()
{
      if(current_buffer != section[current_section].currbuf)
      buffer_toggle();
      begsamp=section[current_section].firstsamp;
      comdata=&buffer[begsamp];   
      endsamp=section[current_section].lastsamp;
      comlength=endsamp-begsamp;   
      if(section[current_section].flags & STEREO_ON_FLAG) {
		stereo_flag=1;
		DiskText[5].IText="Stereo Off";
		channels=STEREO;
		}
      else if(stereo_flag) {
		stereo_flag=0;
		DiskText[5].IText="Stereo On";
		channels=LEFT;
		}	

/*      if(section[current_section].flags & SPLIT_FLAG) {
		key_split_flag=1;
		MiscText[2].IText="Key Split Disable";
		}
		else if(key_split_flag) {
		key_split_flag=0;
		MiscText[2].IText = "Key Split Enable";
		} */
}

get_midi()
   {

      while((tlength=querySer()) != 0)
        {
         actual = readSer (midi,tlength);
	midi_parse(actual);
	} /* end of while tlength */
} /* end of get_midi routine */

int midi_parse(actual)
int actual;
{
         k=0;
	 if(midi_flag==0) return;
         while (k < actual)
            {
            midi_data=midi[k];
	if(midi_data >239) { /* System message! */
		if((midi_data)== EXT_START) {
			ext_clock=0;
			ext_wait_flag=0;
			this_event=0;
			}
		if((midi_data) == EXT_CLOCK) ext_clock+=60;
		if((midi_data) == EXT_STOP) ext_wait_flag=1;
		if((midi_data) == EXT_CONT) ext_wait_flag=0;
		k++;
		}
   else
            switch (midi_read)
               {
               case 0:
                  if ((n=(midi_data & 128)) == 128)
                     {
                   	midi_status = (midi_data & 244); 
	                midi_channel = (midi_data & 15); 
			if(in_channel==midi_channel) midi_read++;
			k++;
                     }
		  else midi_read++; /* must be running status */
			/* dont incr k, so we reprocess this byte */
                  break;
               case 1:
                  if (midi_status == NOTE_ON)
                        {
                        midi_pitch = midi_data;
                        midi_read++;
                        }
                  if (midi_status == VOICE_CHANGE)
                        {
                midi_voice=(midi_data&15);   
	
      if((section[midi_voice].lastsamp != 2) && (midi_voice < MAXSECT))
         {
                      current_section = midi_voice;
                    change_section();   
         }
                     midi_read=0;
                     }   
                  k++;
                  break;
               case 2:
                  if (midi_status == NOTE_ON)
                     {
                     midi_volume = midi_data;
		   if(midi_volume != 0 && velocity_flag == 1)
		     comvolume = vel_vol[midi_volume];
                     midi_read=0;
                     k++;
                    if (key_split_flag == 1)
                      {   
                           current_section = key_section[midi_pitch];
                           change_section();
                      }      
                       pper=period[current_section][midi_pitch];

                     if (midi_volume == 0)
                          {
/*			Delay(1); give time for other comms*/
				draw_key(midi_pitch,FALSE);
		/* note that we draw it FALSE before actually fliushing
			this record is not always accurate */
              			tempvoice=nvoice;
   			            nvoice=note_channel[current_section][midi_pitch];
		if(used[nvoice]==midi_pitch)
   	            if(CheckIO(loopcom[nvoice]) != 0 || loopcom[nvoice] == 0)
						{
				             command = ADCMD_FINISH;
						kk=nvoice; /*play will
		automatically advance nvoice, so must preserve */
						play();
						used[kk]=0;
						loopcom[kk]=0;
						}
				else
				{
			/* flush because havent even gotten to loop yet */
			flush_channel();
			flush_channel();
			used[nvoice]=0;
				}
		/*	used[nvoice]=0;*/ /* this is a dumb kludge, see ? */
				nvoice = tempvoice; 
	                } /* end of if midi volume == 0 */
                     else
                        {
			/* sound a note */
			draw_key(midi_pitch,TRUE);
	                next_channel();
			 used[nvoice]=midi_pitch;
        if((section[current_section].flags & SUSTAIN_FLAG) == SUSTAIN_FLAG)
                      {
                comdata = &buffer[section[current_section].firstsamp];
               comlength = section[current_section].loopstart-
                        section[current_section].firstsamp;
               ncycles = 1;
               command = CMD_WRITE;
	       loopcom[nvoice] = audcom[curcom];	
               play();
               comdata = &buffer[section[current_section].loopstart];
               comlength = section[current_section].loopend-
                        section[current_section].loopstart;
               ncycles = 0;
               play();
               comdata = &buffer[section[current_section].loopend];
               comlength = section[current_section].lastsamp-
                         section[current_section].loopend;
               ncycles = 1;
               play();
   
                     }
               else
                      {
                  /* normal play */
                           command = CMD_WRITE;
                           play();
			   loopcom[nvoice]=0;
                     }   
                       note_channel[current_section][midi_pitch] = nvoice;
			used[nvoice]=midi_pitch;
                       }
		if(midi_record_flag) 
			{
			now=playtime();
			event[n_midi_events].what1=midi_status+midi_channel; /* + add ch ! */
			event[n_midi_events].when=now;
			event[n_midi_events].what2=midi_pitch; 
			event[n_midi_events].what3=midi_volume; 
			event[n_midi_events].voice=current_section;
			if(++n_midi_events>MAXMIDIEVENTS) {
				midi_record_flag=0;
				MidText[0].IText = "Midi Record";
				cue("             Sorry, out of MIDI memory");
				if(midi_overdub_flag) {
					midi_overdub_flag=0;
				        MidText[3].IText = "Midi Overdub";
					if(cue("           Happy with that?")) {
						post("Sorting");
			for(n=n_play_events;n<n_midi_events;k++)
				event[n].when=(event[n].when)*tempo/60;
						midi_sort();
						unpost();
						}
					else n_midi_events=n_play_events;
					} /* end of overdub */
				} /* max midi */
			} /* end if midi_record_flag */
                     }  /* end of if midi_status == NOTE_ON */
			else { /* must be a fuckup */
				k++;
				midi_read=0;
				}
                  break;
               default:
/*                  printf("midi data error"); */
			k++;
                  midi_read=0;
                  break;
               } /* end of switch midi_read */
       } /* end of while k<actual */     
   }/* end of midi_parse routine */

next_channel()
   {
   for(n=0;n<5;n++)	
	{
     	if(++nvoice == 4)
         	nvoice=0;
	if(used[nvoice]==0)
		break;
	}	
   flush_channel();
/*   flush_channel();	*/
   }


int find_com()
	{
	n = 0;
	if(++curcom == MAXCOM)
		curcom = 0;
	while((CheckIO(audcom[curcom]) == 0) && (++n <MAXCOM+1))
		if(++curcom == MAXCOM)
	 		curcom = 0;
	if(n >= MAXCOM)
		n=0;
	else
		n=1;
	return(n);
	}


int file_choose()
{
char *n;
n=get_fname(&NewWindow,NULL,"Take your pick:",FileNameStr,thedir);
if(!n) return(-1);
strcpy(mycurdir,thedir);
if(mycurdir[strlen(mycurdir)-1] != ':') strcat(mycurdir,"/");
strcat(mycurdir,FileNameStr);
return(1);
}
	

/* record from the digitizer */
int record (lp,rate,maxnum)
char *lp;
int rate,maxnum;
{
register char *reg1;
register ULONG k;
static int i;
static UBYTE *data,*direction,*pa;

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
pa=(UBYTE *)0xBFE001;

reg1=lp;
*direction=0;           /* all bits are read */
*data=0;

post("Left to stop");
if ((*pa&64)==0) while((*pa&64)==0) ;
Disable();
k=0;
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);   /* delay loop */
      *reg1++=(*data)-128;
      }
unpost();
Enable();
k=(--k)&0xFFFFFFF8;    /* make length an even multible of 8 */
return((int)k);
}


long playtime()
{
long l,m,n;
if(ext_clock_flag == 0) {
	tr.tr_node.io_Command = TR_GETSYSTIME;
	DoIO(&tr);
	l=tr.tr_time.tv_secs;
	m=tr.tr_time.tv_micro;
	if(n_midi_events == 0) {
		start_sec=l;
		start_mic=m;
		}
	n=(l-start_sec)*1500+(m-start_mic)/666; /* 1500ths of a second */
	}
else n=ext_clock;
return(n);
}

midi_playback()
{
int temp_midi_read,tf,temp_sec;
long now;
actual=0;
if(ext_clock_flag && ext_wait_flag) return;
if(!n_play_events) return;
tf=midi_record_flag;
temp_sec=current_section;
midi_record_flag=0; /* avoid overdub feedback */
while((now=(playtime()*(long)tempo)/(long)60) >=event[this_event].when) {
	midi[actual++]=event[this_event].what1;
	midi[actual++]=event[this_event].what2;
	midi[actual++]=event[this_event].what3;
	if(current_section != event[this_event].voice) {
		current_section=event[this_event].voice;
		change_section();
		}
	temp_midi_read=midi_read;
	midi_read=0;
	midi_parse(actual);
	writeSer(midi,actual);
	actual=0;
	midi_read=temp_midi_read;
	if(++this_event>=n_play_events) 
		if(midi_repeat_flag==0) {
			this_event=0;
			midi_playback_flag=0;
	                MidText[1].IText = "Midi Playback";
			break;
			}
		else { /* play it again, Sam */
			this_event=0;
			n=n_midi_events;
			n_midi_events=0;/* dumb reset */
			now=playtime();
			n_midi_events=n;
			}
	}
midi_record_flag=tf;
if(current_section != temp_sec) {
	current_section = temp_sec;
	change_section();
	}
}
	

preserve(a,b)
int a,b;
{
	buffer_toggle();
	buffer_toggle();
	OnMenu(Window,UNDO_MENU_NUMBER);
	preserve_begsamp=a;
	preserve_endsamp=b;
	preserve_nsamps=nsamps;
	switch(current_buffer) {
		case 0:
		if((bufsiz-nsamps1+2)>=(b-a)) {
			undo_buffer=&buffer1[nsamps1];
			}
		else {
			OffMenu(Window,UNDO_MENU_NUMBER);
			return;
			}
		break;
		case 1:
		if((bufsiz-nsamps0+2)>=(b-a)) {
			undo_buffer=&buffer0[nsamps0];
			}
		else {
			OffMenu(Window,UNDO_MENU_NUMBER);
			return;
			}
		break;
		}	
	movmem(&buffer[a],undo_buffer,(b-a));
/*	for(k=a;k<b;k++) undo_buffer[k-a]=buffer[k]; */
}

void midi_sort()
{
long mmax;
int nmax;
struct event tempe;
for(k=(n_midi_events - 1);k>=0;k--) {
	mmax=-1;
	nmax=0;
	for(n=k;n>=0;n--) 
		if(event[n].when >mmax) {
			mmax=event[n].when;
			nmax=n;
			}
	tempe=event[k];
	event[k]=event[nmax];
	event[nmax]=tempe;
	}
}	

void reset_clock()
{
int n;
n=n_midi_events;
n_midi_events=0;
playtime();
n_midi_events=n;
if(ext_clock_flag) ext_clock=0;
}

