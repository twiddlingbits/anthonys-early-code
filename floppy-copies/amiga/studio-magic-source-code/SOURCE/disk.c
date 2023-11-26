/* subsed340.c */
#include "sed.include.h"
#include "sed.externals"

extern long ReadIFF();
extern UWORD SizeIFF();
extern int SaveIFF();
extern void midi_sort();
void load();
void load_sec();
int save();
int next_midi_line();
void save_everything();
void put_samples();
void New();
int load_midi();

void New()			
{
if(nsamps > 2)	
	if(cue("    This will wipe out the buffer, okay?") == 0)
		return;
Delay(2); /* bug? */
if(file_choose() <= 0) return;
for(k=0;k<nsects;k++)
	if(section[k].currbuf == current_buffer)
        	section[k]=section_defaults;
for(k=0;k<MAXSECTGAD;k++) 
	SectionText[k].FrontPen=WHITE;
if(copy_buffer==current_buffer) copy_end=2;
/* draw_sect(); */
LongJunk=endsamp;
endsamp=0;
strcpy(UrString,mycurdir);
strcat(mycurdir,".buf0");
if((m=Lock(mycurdir,ACCESS_READ)) == 0) { /* no strings attached */
	UnLock(m);
	load(UrString);
	nsamps=endsamp;	
	hist(begsamp,nsamps);
	update_sliders();
	return;
	}
UnLock(m);
buffer_toggle();
if(nsamps>2)
	if(cue("This will erase EVERYTHING, okay?")==0)
		return;
if(current_buffer !=0)
	buffer_toggle();
endsamp=0;
if(copy_buffer==current_buffer) copy_end=2;
load(mycurdir);
strcpy(mycurdir,UrString);
strcat(mycurdir,".buf1");			
buffer_toggle();
endsamp=0;
load(mycurdir);
if(endsamp == 0) endsamp=LongJunk;
nsamps=endsamp;
load_sec(UrString);
strcpy(mycurdir,UrString);
strcat(mycurdir,".midi");
if((m=Lock(mycurdir,ACCESS_READ)) != 0) load_midi(mycurdir);
UnLock(m);
RefreshGadgets(&LimitGadget[0],Window,NULL);
begsamp=0;
hist(begsamp,nsamps);		
update_sliders();
return;
}

void load(what)
TEXT *what;
   {
unsigned char a[2];
strcpy(RString,what);
LongJunk=SizeIFF(RString);
if(LongJunk != 0) { /*IFF Load! */
	if(LongJunk+begsamp>bufsiz) LongJunk=bufsiz-begsamp;
	post("IFF Load");
	endsamp=ReadIFF(RString,&buffer[0],begsamp);
	unpost();
	}
else {
     if((fp = fopen(RString,"r")) == NULL)
        {
	fclose(fp);
        return;
        }
     filenum = fileno(fp);
     n=read(filenum,&a[0],2);
     n=pper;
     pper=a[0]+256*a[1];
     if(pper<124 || pper>8000) pper=n;
     midpper=pper;
     sr=(1000000/(pper*.279365))+.5;
     n = stcu_d(TempStr,sr,10);
     display(per_x,per_y,7);
/*     update_sliders(); */
     n=8192;
     i=n;
     k=0;
     post("Loading");
     while((n == i) && (k == 0))
      {
      if ((endsamp+i)>bufsiz)
         {
         i=bufsiz-endsamp;
	 cue("File truncated -- too long.");
         k=1;
         }
      n=read(filenum,&buffer[endsamp],i);
      endsamp=endsamp+n;
      }
    fclose(fp);
    unpost();
} /* end of not IFF load */
if(endsamp&1) endsamp=endsamp-1;
nsamps=endsamp;
n = stcu_d(TempStr,begsamp,10);
display(begsamp_x,begsamp_y,7);
n = stcu_d(TempStr,endsamp,7);
display(endsamp_x,endsamp_y,7);
}

void load_sec(what)
TEXT *what;
   {
    if((m=Lock(what,ACCESS_READ)) == 0)
	{
	UnLock(m);
	return;
	}
    if((fp = fopen(what,"r")) == NULL)
        {
	fclose(fp);
        return;
        }
    post("Loading sections");   
      for(k=0;k<nsects;k++)
   {
   fscanf(fp," %d ! %d ! %d ! %d ! %d ! %d !",&section[k].firstsamp,
        &section[k].lastsamp,&section[k].loopstart,&section[k].loopend,
      &section[k].flags,&section[k].currbuf);
   for(m=0;m<MAXNAME;m++)
	fscanf(fp,"%c",&section[k].name[m]);
   }
        for(k=0;k<nsects;k++)
	{
       for(m=0;m<100;m++)
	{
       fscanf(fp,"%h! ",&period[k][m]);
	}
	}
       for(m=0;m<100;m++)
   	{
   	fscanf(fp,"%h! ",&key_section[m]);   
   	}
   fscanf(fp,"%h! ",&key_split_flag);
if(key_split_flag !=1) key_split_flag=0;
    if(key_split_flag)
  MiscText[2].IText = "Disable Key Split";
	else
  MiscText[2].IText = "Enable Key Split";

  fclose(fp); 
   unpost();	
   change_section();
   }

int save(what)
TEXT *what;
   {
    if((m=Lock(what,ACCESS_READ)) != 0)
   if(cue("Replace existing file?") == 0)
      {
      return(0);
      }
      UnLock(m);
     /* put sample file into buffer */
     if((fp = fopen(what,"w")) == NULL)
        return(0);
   put_samples();   
   return(1);
   }

void save_everything(what)
TEXT *what;
   {
kk=current_buffer;
  if(current_buffer == 1)
	buffer_toggle();	
   strcpy(UrString,what);
   strcat(what,".buf0");
   if ((n=save(what)) == 0)
	   return;
   if((fp = fopen(UrString,"w")) == NULL)
        return;
   post("Saving Sections");
   for(k=0;k<nsects;k++)
   {
   fprintf(fp," %d ! %d ! %d ! %d ! %d ! %d !",section[k].firstsamp,
        section[k].lastsamp,section[k].loopstart,section[k].loopend,
      section[k].flags,section[k].currbuf);

   for(m=0;m<MAXNAME;m++)
	fprintf(fp,"%c",section[k].name[m]);
   }
   for(k=0;k<nsects;k++)
	{
   for(m=0;m<100;m++)
	{
       fprintf(fp,"%d! ",period[k][m]);
	}
	}
   for(m=0;m<100;m++)
   {
   fprintf(fp,"%d! ",key_section[m]);   
   }
   fprintf(fp,"%d! ",key_split_flag);
   fclose(fp);      
   unpost();
if(nsamps1 > 2)
   {
   buffer_toggle();
   strcpy(what,UrString);
   strcat(what,".buf1");
   if((fp = fopen(what,"w")) == NULL)
        return;
     put_samples();
    }
if(n_midi_events) {
	strcpy(what,UrString);
	strcat(what,".midi");
	post("Saving MIDI");
	save_midi(what);
	unpost();
	}
if(kk != current_buffer) buffer_toggle();
return;
}


void put_samples()
{
unsigned char a[2];
     filenum = fileno(fp);
     a[0]=(pper&255);
     if(pper>255) a[1]=(pper&(64738-255))>>8;
     else a[1]=0;
     n=write(filenum,&a[0],2);
     n=8192;
     i=n;   
     LongJunk = 0;
     post("Saving buffer");   
     while((LongJunk+n) < nsamps)
      {
      if((n=write(filenum,&buffer[LongJunk],i)) < 1)
	{
	fclose(fp);
	unpost();

	return;
	}
      LongJunk=LongJunk+n;
      }
     i=nsamps-LongJunk;   
     n=write(filenum,&buffer[LongJunk],i);
     fclose(fp);
     unpost();
     return;
}

int load_midi(what)
TEXT *what;
   {
short ah,bh,ch,dh;
     if((fp = fopen(what,"r")) == NULL) return(0);
	filenum=fileno(fp);
	post("Loading MIDI");
	if(n=next_midi_line()) {
		k=0;
		fclose(fp);
		fp=fopen(what,"r");
		filenum=fileno(fp);
		while(n=next_midi_line()) {
			switch(c) {
			case 'N':
			n=fscanf(fp,"%f%f%h%h%h",&FloatJunk,&durmult,&ah,&bh,
				&ch);
			event[k].what2=ah;
			event[k].what3=bh;
			event[k].voice=ch;
			event[k].what1=NOTE_ON;
			event[k].when=FloatJunk*1000.;
			k++;
			event[k].what2=ah;
			event[k].what3=0;
			event[k].voice=ch;
			event[k].what1=NOTE_ON;
			event[k].when=(durmult+FloatJunk)*1000.;
			k++;
			break;
			case 'V':
			n=fscanf(fp,"%f%h",&FloatJunk,&ah);
			event[k].what1=VOICE_CHANGE;
			event[k].when=FloatJunk*1000.;			
			k++;
			break;
			case 'T':
			n=fscanf(fp,"%h",&tempo);
			break;
			case 'X':
			n=fscanf(fp,"%f%h%h%h%h",&FloatJunk,&ah,&bh,&ch,&dh);
			event[k].what1=ah;
			event[k].what2=bh;
			event[k].what3=ch;
			event[k].voice=dh;
			event[k].when=FloatJunk*1000.;
			k++;
			break;
			}
		}
	n_midi_events=k;
	n_play_events=k;
	midi_sort();
	}
	else {	
	fclose(fp);
	fp=fopen(what,"r");
	filenum=fileno(fp);
	n=read(filenum,&tempo,2);
	if(n != 2) return(0);
	i=0;
	n=8;	
	while(n==8) {
		n=read(filenum,&event[i].what1,1);
		if(n!=1) break;
		n=read(filenum,&event[i].what2,1);
		n=read(filenum,&event[i].what3,1);
		n=read(filenum,&event[i].voice,1);
		n=read(filenum,&event[i].when,8); 
		if(n!=8) break;
		i++;
		}
n_midi_events=i;
n_play_events=i;
}
unpost();
fclose(fp);
  return(1);
   }


int save_midi(what)
TEXT *what;
   {
     if((fp = fopen(what,"w")) == NULL) return(0);
	filenum=fileno(fp);
	if(midi_format_flag == 1) {
	fprintf(fp,"STUDIO MAGIC -- This is an edittable midi data file:\n\n");
	fprintf(fp,"----------------------------------------------------\n");
	fprintf(fp,"N   st      dur     pitch  volume  voice         \n");
	fprintf(fp,"----------------------------------------------------\n\n");
	fprintf(fp,"T %d\n",tempo);
		for(k=0;k<n_midi_events;k++) {
			n=0;
			FloatJunk=event[k].when / 1000.;
			switch(event[k].what1) {
			case NOTE_ON:
			if(event[k].what3 != 0) {
				while((event[k].what2 != event[k+n].what2) ||
(event[k].voice != event[k+n].voice) ||
		((event[k+n].what3 != 0) && (event[k+n].what1 != NOTE_OFF))) {
				if(++n > n_midi_events) {
					break;
					}
				}
			durmult=(event[k+n].when - event[k].when) / 1000.;
	fprintf(fp,"N %3.5f %3.5f %d %d %d\n",FloatJunk,durmult,
				event[k].what2,event[k].what3,event[k].voice);
			}
			break;
			case VOICE_CHANGE:
			fprintf(fp,"V %3.3f %d\n",FloatJunk,event[k].voice);
			break;
			default:
			fprintf("X %3.3f %d %d %d %d\n",FloatJunk,
				event[k].what1,event[k].what2,event[k].what3,
				event[k].voice);
			break;
			}/* end of switch */			
			}

		}
	else {
	n=write(filenum,&tempo,2);
	for(k=0;k<n_midi_events;k++) {
		n=write(filenum,&event[k].what1,1);
		n=write(filenum,&event[k].what2,1);
		n=write(filenum,&event[k].what3,1);
		n=write(filenum,&event[k].voice,1);
		n=write(filenum,&event[k].when,8);
		if(n!=8) {
			break;
			}
	}
	}
	fclose(fp);
  return(1);
   }

int next_midi_line()
{
c='0';
i=0;
while((c != 'N') && (c != 'V') && (c != 'X') && (c != 'T' )) {
	n=fscanf(fp,"%c",&c);
	if((n != 1) || (i++ >40)) {
		return(0);
		}
	}
return(1);
}
