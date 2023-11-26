#include "sed.include.h"
#include "sed.externals"
void delays();

void delays(k)
int k;
{
printf("beg end %ld %ld\n",begsamp,endsamp);
n=frsize;
printf("%d %ld %d",n,frsize,n);
for(n=0;n<k;n++)
	{
	FloatJunk=(float)rand()/(2147385345.);
	printf("rand is %f ",FloatJunk);
	table[n]=endsamp-(frsize*FloatJunk);
	printf("delay %d is %d -- \n",n,table[n]);
	}
maxamp=0;
for(l=endsamp;l>begsamp;l--) {
	i=buffer[l];
	for(n=0;n<k;n++) {
		if(--table[n]>begsamp)
			i+=buffer[table[n]];
		}
	if(i>maxamp)
		maxamp=i;
	}
maxamp=(126*64)/maxamp;
for(n=0;n<k;n++) table[n]+=(endsamp-begsamp); /* restore the delays */
for(l=endsamp;l>begsamp;l--) {
	i=buffer[l];
	for(n=0;n<k;n++) {
		if(table[n]>begsamp) i+=buffer[table[n]--];
		}
	i*=maxamp;
	if(i<0) i=0-((0-i)>>6);
	else i=i>>6;
	buffer[l]=i;
	}
}
