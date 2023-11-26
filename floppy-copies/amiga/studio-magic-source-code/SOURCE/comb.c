#include "sed.include.h"
#include "sed.externals"

extern float get_fvalue();
void comb();

void comb()
{
int oldn;
			n=get_pitch_value("Filter pitch:");
			if(n==-1) return;
			n=n-12; /* down 2 octs???? */
			FloatJunk=pow(2.0,(float)((105.-(float)n)/12.));
			FloatJunk=(float)7040./FloatJunk;
			frsize=(float)sr/FloatJunk;
			if(frsize>MAXFILTER) return;
		feedback=get_fvalue("Feedback:","Feedback:",.8,.99,.99);
			if(feedback==-1) return;
	/* DCBias */
			k=0;
			for(l=begsamp;l<endsamp;l++) k+=buffer[l];
			k=(k/(endsamp-begsamp)+.5);
			printf("%d DCBias found\n",k);
			for(l=begsamp;l<endsamp;l++) buffer[l]-=k;

		printf("frsize %d feedback %f\n",frsize,feedback);
			num=feedback*64.+.5;/*shouyld be 64, not 256 */
			num2=(126*64)/128;
			close_midi();
			post("Crunching");
			LongJunk=endsamp-begsamp;
			k=0; /* points to oldest member of filter array */
			for(l=0;l<frsize;l++) filter[l]=buffer[begsamp+l];
			n=0;
			oldn=126;
			for(l=(begsamp+frsize);l<(begsamp+LongJunk);l++) {
				n=(filter[k]*num);
				if(n<0) n=(0-n)>>6;
				else n=n>>6;
				n+=buffer[l];
				if(iabs(n)>oldn) {
					num2=iabs((126*64)/n);
					oldn=n;
					} 
				filter[k]=n; /* before the rescale ! */
				n*=num2; 
				if(n<0) n=(0-n)>>6;
				else n=n>>6;
				buffer[l]=n;
				if(++k==frsize) k=0;
				}
			unpost();
			open_midi();
			hist(begsamp,endsamp);
return;
}
