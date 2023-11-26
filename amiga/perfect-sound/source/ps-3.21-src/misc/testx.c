main()
{
int vid_clock=3579546;        /** NTSC constant **/
int record_speed;
int i,old;

old = 0;
for (i=1; i < 40000; i++) {
   record_speed=(vid_clock/10)/i;
   record_speed=(vid_clock/10)/record_speed;
   if (record_speed!=old)
      printf("req %d act %d\n",i,record_speed);
   old = record_speed;
   }
}


