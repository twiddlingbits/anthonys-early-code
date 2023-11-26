#include "sed.include.h"
#include "sed.externals"
void check_sections();
void remove_section();

hist(start,nlong)
long int start;
long int nlong;
{
int k,l,nframs,frsize;
float rms,tot;
int sum;
int scwidth=594;
int mid=62;
int cx=23;
int amp;
int inc;
long int istart,iend;

current_hist=current_buffer;
nframs=(scwidth/(3));

frsize=nsamps/nframs+.5;
if(frsize==0) frsize=1;
istart=(float)start*nframs/(float)nsamps;
FloatJunk=(float)nlong/(float)nsamps;
iend=FloatJunk*nframs;
if(istart<0) istart=0;
if(iend>(scwidth/3)) {
	iend=scwidth/3;
	}
start=istart*frsize;
nframs--;

cx=23+istart*3;
nframs=(nlong-start)/frsize;
if (frsize>4) inc=frsize/4;
    else inc=1;
SetAPen(rastPort,WHITE);
RectFill(rastPort,istart*3+23,11,iend*3+23,66);

SetAPen(rastPort,BLACK);
Move(rastPort,20,11);
Draw(rastPort,620,11);
Draw(rastPort,620,66);
Draw(rastPort,20,66);
Draw(rastPort,20,11);
if(nlong<=2) return;
for(k=0;k<=nframs;k++)
   {
   sum=0;
   l=start;
   m=start+frsize;
   if(current_buffer==copy_buffer) {
	if(copy_end>2) {
 	 	if(m>=copy_start) SetAPen(rastPort,YELLOW);
		if(l>=copy_end) SetAPen(rastPort,BLACK); /* show marked region */
		}
	 }
   for(l=start;l < m ;l=l+inc)
      {
      sum+=iabs(buffer[l]); /* *buffer[l]; */
      }
   start+=frsize;
   rms=sum/(frsize/inc); 
/*   rms = sqrt(tot);  */
   /* rms should now be the amp, from 0 to 128 */
   amp=mid-(rms/1.4);
	if(amp<13) amp=13;
   Move(rastPort,cx,mid); 
   Draw(rastPort,cx,amp);
   Move(rastPort,cx+1,mid);
   Draw(rastPort,cx+1,amp);
/*   Move(rastPort,cx+2,mid);
   Draw(rastPort,cx+2,amp);   */
   cx=cx+3+rastPort->PenWidth;
   if(cx>(scwidth+23)) {
	break;
	}
   FloatJunk=nsamps;   
   }
if (show_sect)
	draw_sect();
}

draw_sect()
{
int scwidth=594;
int mid=60;
int cx=23;
int amp;
int inc;
FloatJunk = nsamps;
check_sections();
   for(k=0;k<nsects;k++)
	{
/*
 if((section[k].lastsamp >nsamps) && (section[k].currbuf == current_buffer)) {
	section[k]=section_defaults;
	SPrint(rastPort,SpaceStr,212,90+14*k,10);
        RefreshGadgets(&SectionGadget[0],Window,NULL);
	}
*/
      if((section[k].lastsamp > 2) && (section[k].currbuf == current_buffer))
      {
      SetAPen(rastPort,BLACK);
      cx=(section[k].firstsamp/FloatJunk)*scwidth+23;
      Move(rastPort,cx,mid+5);
      Draw(rastPort,cx,mid+6);
      amp=(section[k].lastsamp/FloatJunk)*scwidth+23;
      if(amp>scwidth+23) amp=scwidth+23;
      Move(rastPort,amp,mid+1);
      Draw(rastPort,amp,66);
      SetAPen(rastPort,YELLOW);
      RectFill(rastPort,cx+1,62,amp-1,64);
      }
   }
}


void check_sections()
{
buffer_toggle();
buffer_toggle();
for(k=0;k<nsects;k++) {
	 if(section[k].currbuf == 1) l=nsamps1;
	         else l=nsamps0;
	 if(section[k].lastsamp > l)
		remove_section(k);
	}
RefreshGadgets(&SectionGadget[0],Window,NULL);
}

void remove_section() /* assumes k-th section to remove ! */
{
section[k]=section_defaults;
i=k;
if(i>8) i=i-8;
if(section[k].currbuf == current_buffer) 
	SPrint(rastPort,SpaceStr,212,90+14*i,10);
}

update_sliders()
   {
	if(begsamp>nsamps) begsamp=nsamps-2;
	if(endsamp>nsamps) endsamp=nsamps;
        FloatJunk = (begsamp*65535.)/nsamps;

        LimitInfo[lastbeg].HorizPot = FloatJunk;
        FloatJunk = (endsamp*65535.)/nsamps;
        LimitInfo[lastbeg^1].HorizPot = FloatJunk;
                     begplot(buffer,begsamp);
                     endplot(buffer,endsamp);
                     duration=pper*tic*(endsamp-begsamp)/1000000.;
                     strfloat(TempStr,duration);
                     display(duration_x,duration_y,8);
                     n = stcu_d(TempStr,begsamp,10);
     	             display(begsamp_x,begsamp_y,7);
                     n = stcu_d(TempStr,endsamp,10);
                     display(endsamp_x,endsamp_y,7);
           comdata = &buffer[begsamp];
           comlength = endsamp-begsamp;   
	FloatJunk=log((double)midpper/127.)/log(2.);
	FloatJunk=(65535.*FloatJunk)/8.4;
	PerInfo.HorizPot = FloatJunk;
       RefreshGadgets(&LimitGadget[0],Window,NULL);
	if(current_hist != current_buffer)
		hist(begsamp,nsamps); 
SetAPen(rastPort,BLUE);
Move(rastPort,lastlastmoved,67);
Draw(rastPort,lastlastmoved,69);
Move(rastPort,lastlastmoved-1,67);
Draw(rastPort,lastlastmoved-1,69);
FloatJunk=LimitInfo[lastmoved].HorizPot/65535.;
n=FloatJunk*594+23;
SetAPen(rastPort,YELLOW);
Move(rastPort,n,67);
Draw(rastPort,n,69);
Move(rastPort,n-1,67);
Draw(rastPort,n-1,69);
lastlastmoved=n;
	   }
