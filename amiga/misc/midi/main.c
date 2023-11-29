#include "libraries/dos.h"


main()
{
int dummy,i,j,kount;
unsigned char midi_data[100];

dummy = Open("SER:",MODE_OLDFILE);
if (dummy==0) {
   printf("can't alloc ser:\n");
   exit(0);
   }

midi_init();               /** This links in my RBF interupt handler **/

for (i=0; i<100; i++) {    /** Grab 100 events **/

   while (midi_check()==0) ;            /*** Wait for data to arrive ***/
   kount = midi_grab(midi_data);        /*** xfer data to our buffer ***/
   printf("%d: ",kount);
   for (j=0; j<kount; j++)              /*** Print it for fun ***/
      printf("%2x",midi_data[j]);
   printf("\n");

   }

midi_end();          /*** This restores the old interupt handler **/

Close(dummy);     /*** Free the SER: device ***/
}
