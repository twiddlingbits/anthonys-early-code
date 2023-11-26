
/* ps_lib_test.c  © copyright 1990 Anthony J. Wood
**
** This program demonstrates how to call the psound.library functions.  I
** tested this program using lattice 'C' 4.01.  You must link with
** ps_lib_stub.o for it to run ok.  See ps_lib_docs for more info */


#define  PS_2   0
#define  PS_3   1
#define  RIGHT  4
#define  LEFT   2
#define  STEREO 6

struct PSBase *PSBase;

static char buf[10000];

void main()

{
void plot();
int choice;
int i;

PSBase = OpenLibrary("psound.library",1);
printf("psbase=%x\n",PSBase);
if (PSBase)
   query_digi_type();        /* Tell library which PSound calls are for */

while (PSBase) {
   printf("\nWelcome to Anthony's psound.library test program\n\n");
   printf("1. Monitor left\n");
   printf("2. Monitor right\n");
   printf("3. Record left\n");
   printf("4. Record right\n");
   printf("5. Record stereo\n");
   printf("6. Real time echo\n");
   printf("7. Real time delay\n");
   printf("8. Query digitizer\n");
   printf("9. Test round_ss()\n");
   printf("10. Inc gain\n");
   printf("11. Dec gain\n");
   printf("12. Set plot_point()\n");
   printf("13. Get a few samples (left channel)\n");
   printf("14. Get a few samples (right channel)\n");
   printf("15. Set vid_clock to PAL speed\n");
   printf("99. Exit\n");

   scanf("%d\n",&choice);
   printf("\n");

   if (choice==1) {
      Disable();
      ps_monitor(LEFT);
      Enable();
      }
   if (choice==2) {
      Disable();
      ps_monitor(RIGHT);
      Enable();
      }
   if (choice==3) {
      Disable();
      ps_record(buf,0,10000,12000,LEFT);
      Enable();
      printf("%d %d %d\n",buf[0],buf[1],buf[2]);
      }
   if (choice==4) {
      Disable();
      ps_record(buf,0,10000,12000,RIGHT);
      Enable();
      printf("%d %d %d\n",buf[0],buf[1],buf[2]);
      }
   if (choice==5) {
      Disable();
      ps_record(buf,buf+5000,5000,12000,STEREO);
      Enable();
      printf("%d %d %d\n",buf[0],buf[1],buf[2]);
      }
   if (choice==6) {
      Disable();
      real_echo(buf, 3000);
      Enable();
      }
   if (choice==7) {
      Disable();
      real_delay(buf, 3000);
      Enable();
      }
   if (choice==8) {
      i = query_digi_type();
      if (i==PS_2) printf("PSound 2.0 or 1.0\n");
      else if (i==PS_3) printf("Psound 3.0");
      else printf("Unknown type\n");
      }
   if (choice==9) {
      printf("testing round_ss() with 25,000: %d\n",round_ss(25000));
      }
   if (choice==10) {
      printf("inc gain\n");
      inc_gain();
      }
   if (choice==11) {
      printf("dec gain\n");
      dec_gain();
      }
   if (choice==12) {
      printf("setting plot point\n");
      set_plot_point(plot);
      }
   if (choice==13) {
      for (i=0; i < 10; i++)
         printf("%d\n",get_sample(LEFT));
      }
   if (choice==14) {
      for (i=0; i < 10; i++)
         printf("%d\n",get_sample(RIGHT));
      }
   if (choice==15) {
      set_vid_clock(3546895);    /* PAL C1 speed */
      }
   if (choice==99) {
      CloseLibrary(PSBase);
      PSBase = 0;
      }
   }
}

void plot(x)
int x;
{
printf("%d\n",x);
}

