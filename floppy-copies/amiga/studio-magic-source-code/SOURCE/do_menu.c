#include "sed.include.h"
#include "sed.externals"
extern float get_fvalue();
extern void display();
extern void clear_voices();
void do_menu();

void do_menu()
{
	   clear_voices(); /* on ANY menu choice! */
	   if(midi_playback_flag) {
       	 	midi_playback_flag=0;
		for(k=36;k<=100;k++) draw_key(k,FALSE);
		}
           if (code != MENUNULL)
               {
               MenuNumber = MENUNUM(code);
               ItemNumber = ITEMNUM(code);
                switch(MenuNumber)
                  {
                  case 0: 
                  switch(ItemNumber)
                     {
		     case 0: /* Really new */
		     ReallyNew();
		     break;
                     case 1: /* Open (but the routine is stupidly called New) */
                     New();
                     break;
                     case 2: /* Append */
                     endsamp=nsamps;
		     begsamp=endsamp;
		     close_midi();
		     if(file_choose()>0) {
	                     load(mycurdir);
			     open_midi();			
                	     nsamps=endsamp;
	                     hist(0,nsamps);
			     if(begsamp==endsamp)
				begsamp=0; /* must have aborted or screwed up */
			     update_sliders();	
				}
                     break;
                     case 3: /* record */
		     close_midi();
		     begsamp=endsamp;	
		     fake_rate=(float)((pper-127.)/45.+.5);
		     if(fake_rate < 1) fake_rate=1;
		     pper=127+45*fake_rate;
	             midpper=pper;
	             sr=(1000000/(midpper*.279365))+.5;
                     n = stcu_d(TempStr,sr,10);
                     display(per_x,per_y,7);
		     endsamp = endsamp+digitze(&NewWindow,&buffer[endsamp],
(&buffer[endsamp]+stereo_offset),fake_rate,bufsiz-endsamp,channels);
		     nsamps=endsamp;
		     open_midi();
		     hist(0,nsamps);	
		     update_sliders();	
                     break;
                     case 6: /* Save samples */
                 /* save begsamp to endsamp only */
		     l=nsamps;
		     f=buffer;
		     buffer=&buffer[begsamp];
		     nsamps=endsamp-begsamp;			
		if(file_choose()>0) {
			close_midi();
	             n=save(mycurdir);   
		     open_midi();
		     }
		     nsamps=l;
		     buffer=f;	
                     break;
              case 7:
                 /* save everything */
			if(file_choose()<=0) break;
			close_midi();
                       save_everything(mycurdir);      
			open_midi();
                 break;
		case 9: /* save IFF */
		if(file_choose()<=0) break;
		close_midi();
		SaveIFF(mycurdir,&buffer[begsamp]);
		open_midi();
		break;
		case 10: /* create IFF Instr */
		if(section[current_section].lastsamp<=2) {
			cue("You must first create a section.");
			break;
			}
		CreateIFF();
		break;
    case 8:
/* stereo menu option */
	stereo_flag = stereo_flag ^ 1;
	if(stereo_flag) {
		DiskText[8].IText="Stereo Off";
		channels=STEREO;
		}
	else {
		DiskText[8].IText="Stereo On";
		channels=LEFT;
		}
    break;   
         case 11:
         /* quit */
         if(cue("      Are you sure you want to quit?"))
            {
            cleanUp("Done.");
	    exit(0);
            }
	break;
	case 4: /* Load Midi */
	if(file_choose()>0) {
		load_midi(mycurdir); 
		} 
         break;      
	case 5: /* Save midi */
	midi_format_flag=1; /* after version 1.5 */		
	if(file_choose() > 0) save_midi(mycurdir);	
                  break;
		} /* end of item 0 switch */
		break;
                  case 1: /* edit menunumber */
/*		if((nsamps0 != 2) || (nsamps1 != 2)) */
                  switch(ItemNumber)
                     {
                     case 1: /* mark */
			printf("nsamps=%ld",nsamps);
			l=copy_start;
			printf("nsamps=%ld",nsamps);
			m=copy_end;
			printf("nsamps=%ld",nsamps);
			if(l>1) l-=2;
			printf("nsamps=%ld",nsamps);
			if(m!=bufsiz) m+=2;
			printf("nsamps=%ld",nsamps);
                     	copy_start = begsamp;
			printf("nsamps=%ld",nsamps);
                     	copy_end = endsamp;
			printf("nsamps=%ld",nsamps);
			copy_buffer = current_buffer;
			printf("nsamps=%ld",nsamps);
			if (m>4) hist(l,m);
			printf("nsamps=%ld",nsamps);
			hist(begsamp,endsamp);
			printf("nsamps=%ld",nsamps);
                     	break;
                     case 2:
		     	close_midi();
        	     	/* paste (destructive write) */   
			preserve(begsamp,nsamps);
			sot_edit(); 
	             	paste();   
		     	open_midi();
                     	hist(begsamp,nsamps);
			begsamp=0;
			endsamp=nsamps;
			update_sliders();
                     	break;  /* end of paste case*/
                     case 3:
        		/* overlay */
           		post("Crunching");      
		     	close_midi();	
			overlay();
               		unpost();
			open_midi();
                        hist(begsamp,nsamps);
			begsamp=2;
			endsamp=nsamps;
			update_sliders();
                    	break; /* end of case 2 overlay */
                     case 5: /* clear buffer */
/*                     	for(k=0;k<bufsiz;k++)
                        	buffer[k]=0; */
			begsamp=0;
			endsamp=2;
			nsamps=2;
                  	hist(begsamp,nsamps);
			update_sliders();
                     	break;
                     case 4:
           		/* remove */   
			close_midi();	
			preserve(begsamp,nsamps);
			if(endsamp==nsamps) nsamps=begsamp+2;
           		else left_slide(endsamp,(endsamp+2-begsamp));   
			open_midi();
			begsamp=0;
                     	hist(begsamp,nsamps);
			endsamp=nsamps;
	           	update_sliders();
                     	break;
           	     case 6:
	           	/* insert */
			insert();
           		break;   
			case 7:
			/* bevel */
			bevel();
			break;      
			case 8: /* zero */
			preserve(begsamp,endsamp);
			for(k=begsamp;k<endsamp;k++) buffer[k]=0;
			hist(begsamp,endsamp);
			break;
			case 0: /* undo */
			close_midi();
			post("Undoing");
			for(k=preserve_begsamp;k<preserve_endsamp;k++) {
					tempc=buffer[k];
				buffer[k]=undo_buffer[k-preserve_begsamp];
				undo_buffer[k-preserve_begsamp]=tempc;
					} /* swap */
			begsamp=preserve_begsamp;
			endsamp=preserve_endsamp;
			nsamps=preserve_nsamps;
			update_sliders();
			hist(0,nsamps);
			unpost();
			open_midi();
			break;
                     } /* end of item number switch */
                  break; /* end of menunumber = 1 case */
                  case 2:
		if((nsamps0 != 2) || (nsamps1 != 2)) /* wont work for 0 samps*/
                  switch(ItemNumber)
                     {
                     case 1: /* interp */
     			n=get_pitch_value("Choose an interval from middle C");
			if(n==-1) break;
                     	FloatJunk=(n-60.)/12.;
                     	xmult=1./pow(2.0,FloatJunk);
		     	close_midi();
			preserve(begsamp,nsamps);
                     	endsamp=interpolate(begsamp,endsamp,xmult);
		     	open_midi();	
                     	hist(begsamp,endsamp);
                     	update_sliders();   
                     	break;
                     case 0:
                     reverse(begsamp,endsamp);
                     hist(begsamp,endsamp);
                     break;
		     case 2:
		     do_compress();
		     break;	
		     case 3:
		     preserve(begsamp,nsamps);
		     do_expand();
		     break;
     		     	case 4:
			do_gravy();
			break;	
			case 5:
			/* rescale */
			close_midi();
			l=begsamp;
			maxamp=0;
                      	while(l <= endsamp)
                       	if((k = abs(buffer[l++])) > maxamp)
                      		maxamp=k;
		k=get_ivalue("New max amp:","Amp:",2,127,maxamp);
			if(k==-1) break;
			post("Crunching");
			FloatJunk=k;
			rescale=FloatJunk/maxamp;
			i=rescale*64;
			l=begsamp;
			while(l++ <= endsamp) {
				k=buffer[l]*i;
				if(k<0) k=0-((0-k)>>6);
				else k=k>>6;
				buffer[l]=k;	
				}
			hist(begsamp,endsamp);
			unpost();
			open_midi();
			break;	
			case 6:
			/* delays */
			kk=get_ivalue("Number of delays:","Delays:",2,100,8);
			if(kk==-1) break;
		FloatJunk=(float)((endsamp-begsamp)/sr);
/*		FloatJunk=FloatJunk/(float)(k); */
FloatJunk=get_fvalue("Maximum delay (seconds):","Delay:",.005,FloatJunk,
						FloatJunk/2.);
			if(FloatJunk==-1.) break;
			frsize=FloatJunk*(float)sr;
			close_midi();
			post("Crunching");
			preserve(begsamp,nsamps);
			delays(kk);
			hist(begsamp,nsamps);
			update_sliders();
			unpost();
			open_midi();
			break;
			case 7:
 			/* flange! */
frsize=get_ivalue("Sweep period:","Period:",1000,100*MAXFILTER-1,(endsamp-begsamp));
			if(frsize==-1) break;
			frsize=frsize/100.;
		feedback=get_fvalue("Feedback:","Feedback:",.3,.99,feedback);
			if(feedback==-1) break;
			i=(feedback*64.);
			/* Let's Go!!! */
			close_midi();
			post("Crunching");
			preserve(begsamp,endsamp);
			fac=feedback*7;
			for(l=begsamp;l<endsamp;l++) buffer[l]=buffer[l]/fac;
			for(l=0;l<frsize;l++) filter[l]=buffer[l+begsamp];
			make_table(frsize,1.,100.);
			fac=0.;
			k=0;
			for(l=begsamp+frsize;l<endsamp;l++) {
				n=table[(int)fac]>>6; /* the delay */
				n=k-n;
				if(n<0) n+=frsize;
				kk=filter[n]*i;
				if(kk<0) kk=0-((0-kk)>>6);
				else kk=kk>>6;
				buffer[l]+=kk; /* fixed point */
				filter[k]=buffer[l];
				if(k++==frsize) k=0;
				fac+=.01;
				if((int)fac>=frsize) {
					fac=0.;
					}
				}
			open_midi();
			hist(begsamp,endsamp);
			unpost();
			break;
			case 8:
			/* comb!!!! */
			preserve(begsamp,endsamp);
			comb();
			break;
			case 9:
			/* AM!!!! */
			close_midi();
			preserve(begsamp,endsamp);
			a_m();
			open_midi();
			hist(begsamp,endsamp);
			break;
			case 10:
			/* Echo!!! */
		IntJunk=get_ivalue("Number of echoes:","Echoes:",1,256,4);
			if(IntJunk==-1) break;
			k=bufsiz-nsamps;
			FloatJunk=(float)(bufsiz-nsamps)/sr;
			FloatJunk=FloatJunk/(float)(IntJunk);
/* if(FloatJunk>((endsamp-begsamp)/sr)) FloatJunk=(endsamp-begsamp)/sr; */
FloatJunk=get_fvalue("Echo delay (seconds):","Delay:",.04,FloatJunk,FloatJunk/2.);
			if(FloatJunk==-1.) break;
		printf("get_fvalue returned %f\n",FloatJunk);
			frsize=FloatJunk*(float)sr;
			printf(" and fr is %ld.",frsize);
			close_midi();
			post("Crunching");
			echo(IntJunk);
			open_midi();
			unpost();
			copy_end=-1;
			copy_start=-2;
			endsamp=nsamps;
			hist(begsamp,nsamps);
			update_sliders();
			break;
			case 11:
			/* DCbias */
			close_midi();
			post("Crunching");
			k=0;
			for(l=begsamp;l<endsamp;l++) k+=buffer[l];
			k=(k/(endsamp-begsamp)+.5);
			for(l=begsamp;l<endsamp;l++) buffer[l]-=k;
			open_midi();
			unpost();
			break;
			case 12:
			/* latest!!! */
			close_midi();
			fft(1,512,&buffer[begsamp]);
			open_midi();
			break;
			case 13: /*invert (dumb) */
			post("Crunching");
			preserve(begsamp,endsamp);
			for(l=begsamp;l<endsamp;l++) buffer[l]=0-buffer[l];
			unpost();
			break;
			}
                  break; /* end of case 2: tool menu number */
                  case 3:
        switch(ItemNumber)       
         {
         case 0:
    if(midi_flag == 0)
	open_midi();
    else 
	close_midi();
         break;
         case 1:
    /* set a split point on the keyboard */
         done=0;
	if(midi_testify == 0) 
		if(cue("     Do you have a midi keyboard?") == 0) break;
	 midi_pitch = 0;
	 midi_testify=1;
	 post("Play boundary notes");
         oldpitch[done] = midi_pitch;
         midi_voice=current_section;
	 if(midi_flag == 0)
		open_midi();
         IntJunk = key_split_flag;
         key_split_flag = 0;	
         while(done < 2)
         {         
         get_midi();
         if(oldpitch[done] != midi_pitch)
            {
            oldpitch[done++]=midi_pitch;
            oldpitch[done]=midi_pitch;         
            }
	 Delay(2);
         } /* end of while done < 2 */
         if (oldpitch[1]<oldpitch[0])
            {
            k=oldpitch[0];
            oldpitch[0]=oldpitch[1];
            oldpitch[1]=k;
            }
	 unpost();
         for(k=oldpitch[0];k<oldpitch[1]+1;k++)
	    {		
            key_section[k]=midi_voice;
	    draw_key(k,TRUE);
	    }
	 key_split_flag = IntJunk;
	 Delay(20);
	 for(k=36;k<=100;k++)
		draw_key(k,FALSE);	 	
     break;
         case 2:
         key_split_flag = ((key_split_flag ^ 1) & 1);
	printf("kf = %d\n",key_split_flag);
    if((key_split_flag & 1))
      MiscText[2].IText = "Disable Key Split";
    else
      MiscText[2].IText = "Enable Key Split";
         break;
    case 3:
    /* Set Sustain point */
    if ((begsamp<section[current_section].firstsamp) ||
        (endsamp>section[current_section].lastsamp))      
        {
        post("Out of bounds!");
	Delay(50);
	unpost();
        break;
        }
   section[current_section].loopstart = begsamp;
   section[current_section].loopend = endsamp;
   section[current_section].flags = (section[current_section].flags | 
                   SUSTAIN_FLAG);
   break; /* end of set sustain case */      
	case 4:
	if(velocity_flag == 1)
		{
		velocity_flag = 0;
		   MiscText[4].IText = "Velocity On";
		}
	else
		{
		velocity_flag = 1;
		   MiscText[4].IText = "Velocity Off";
		}
	break; /* end of case 4: misc menu 4 = Velocity toggle */
	case 5: /* set midi in channel */
	n=get_ivalue("Midi Input Channel:","Channel:",1,16,in_channel+1);
	if(n==-1) break;
	in_channel=n-1;
	break; /* end of set midi inchannel */
	  } /* end of itemnumber switch */
        break; /* end of case 3: misc menu number */      
        case 4:
        switch(ItemNumber)       
	         {
	case 0:
	if(midi_record_flag == 1)
		{
		midi_record_flag = 0;
		   MidText[0].IText = "Midi Record";
		n_play_events=n_midi_events;
		}
	else
		{
		midi_record_flag = 1;
		n_midi_events=0;
		reset_clock();
		   MidText[0].IText = "Stop Recording";
		}

	break; /* end of case 0: */
	case 1: 
	if(midi_playback_flag == 1)
		{ /* this shouldnt really be possible, right? */
		midi_playback_flag = 0;
		}
	else
		{
		midi_playback_flag = 1;
		this_event=0;
		n_play_events=n_midi_events;
		reset_clock();
		midi_record_flag = 0;
                MidText[0].IText = "Midi Record";
		}
	break;
	case 2:
		if(midi_repeat_flag == 1)
		{
		midi_repeat_flag = 0;
		   MidText[2].IText = "Midi Repeat On";
		}
	else
		{
		midi_repeat_flag = 1;
		   MidText[2].IText = "Midi Repeat Off";
		}
            break; /* end of case 2 */
	case 3:
		if(midi_overdub_flag == 1)
		{
		midi_overdub_flag = 0;
		midi_record_flag=0;
		   MidText[3].IText = "Midi Overdub";
		if(cue("           Happy with that?")) {
			post("Sorting");
			for(k=n_play_events;k<n_midi_events;k++)
				event[k].when=(event[k].when)*tempo/60;
			midi_sort();
			unpost();
			}
		else n_midi_events=n_play_events;
		}
	else
		{
		if(n_midi_events == 0) {
			n=cue("Sorry, nothing in memory");
			break;
			}
		midi_overdub_flag = 1;
		   MidText[3].IText = "Stop Overdubbing";
		n_play_events = n_midi_events;
		midi_record_flag=1;
		midi_playback_flag=1;
		this_event=0;
		Delay(50); /* give the poor guy a sec to get ready */
		reset_clock();
		}
            break; /* end of case 3 */
		case 4: /* tempo */
		k=get_ivalue("New Tempo:","Tempo",4,480,tempo);
		if(k==-1) break;
		tempo=k;
		break; /* end of case 4 */
		case 5: /* ext suymnc */
		if(ext_clock_flag == 1)
			{
			ext_clock_flag = 0;
	                MidText[5].IText = "Ext Sync On";
			}
		else
			{
			ext_clock_flag = 1;
		        MidText[5].IText = "Ext Sync Off";
			ext_wait_flag=1;
		 	}
		break;
		  } /* end of itemnumber switch */
		break; /* end of case 4: MID MENU NUMBER */
        } /* end of menu number switch */
               } /* end of if menunum != menunull */
}
