#include "sed.include.h"
#include "sed.externals"
void echo();
	
void echo(nechoes)
int nechoes;
{
int i,j;
short iamp,k;
long dist;
float echoamp;
n=0;
printf("call to do %d echoes\n",nechoes);
while((pow(2.0,(float)++n)+.5<=nechoes)) {
	}
n--;
n=(int)(pow(2.0,(float)n)+.5);
nechoes=n;

dist=nechoes*frsize;
if(nsamps+dist>bufsiz) {
	l=bufsiz-nsamps;
	printf("Not enough room for those echoes.\n");
	return;
	}
printf("ehoing with frsize %ld, nechoes %ds\n",frsize,nechoes);
preserve(begsamp,nsamps);
right_slide(endsamp,dist);
copy_start=begsamp;
copy_end=endsamp;
paste_end=endsamp;
for(m=copy_end+1;m<copy_end+dist;m++) buffer[m]=0; /* zero out destination */
/*j=(copy_end-copy_start)/frsize + 1;
if(j>3) j=sqrt((double)j); */
for(m=begsamp;m<endsamp;m++) {
	if(buffer[m]<0) buffer[m]=0-((0-buffer[m])>>1); /* insure no overflow */
	else buffer[m]=buffer[m]>>1;
	}
j=0;
i=0;
while(i<=n/2) {
	printf("i=%d ",i);
	i=(int)(pow(2.0,(float)j++)+.5); /* .5 to avoid rouding error down */
	copy_end=paste_end;
	paste_start=begsamp+frsize*i;
	paste_end=paste_start+(copy_end-copy_start);
	echoamp=1.-((float)i/(float)(nechoes+1.));
	iamp=(echoamp*64+.5); 
	l=copy_end;
        m=paste_end;
	printf("%ld %ld\n",copy_start,copy_end);
        while(l > copy_start) {
		k=buffer[l]*iamp;
		if(k<0) k=0-(0-k)>>6;
		else k=k>>6;
		buffer[m]+=k; 
		m--;
		l--;
		}
	} /* end of while i loop */
}
