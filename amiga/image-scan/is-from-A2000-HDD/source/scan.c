#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <fcntl.h>
#include "scan.h"

#define DEMO FALSE

extern struct Window *is_window;
extern struct Screen *is_screen;

int open_config()

{
extern struct config config;
FILE *fp, *fopen();
char line[80];
int i;

#if !DEMO
fp = fopen("config.is","r");
if (fp==0) {
   telluser(0,"Can't open 'config.is'","");
   return(FALSE);
   }
#endif

config.return_delay=60;       /** 1 second **/
config.scan_height = 4000;

#if !DEMO
while (fgets(line, 80, fp)) {
   switch (line[0]) {
      case 'Z':         /** return delay **/
         sscanf(line+2,"%d",&config.return_delay);
         break;

      case 'D':         /** dpi **/
         sscanf(line+2,"%d",&config.v_dpi);
         break;

      case 'T':         /** delays between scan **/
         sscanf(line+2,"%d",&config.scan_delay);
         break;

      case 'O':
         sscanf(line+2,"%d",&config.scan_offset);
         break;

      case 'I':         /** printer initilizing code **/
         config.init_len = read_decimal(config.init,line+2);
         break;

      case 'V':         /** Vertical feed **/
         config.v_feed_len = read_decimal(config.v_feed,line+2);
         break;

      case 'X':         /** reverse Vertical feed **/
         config.rev_feed_len = read_decimal(config.rev_feed,line+2);
         break;

      case 'H':         /** Horizontal feed **/
         config.h_feed_len = read_decimal(config.h_feed,line+2);
         if (config.h_feed[0]==0) {    /** zero means use our own **/
            for(i=1; i < 72; i++)
               config.h_feed[i]=32;
            config.h_feed[0]   = '.';
            config.h_feed[69]  = '.';
            config.h_feed[70]  = 13;
            config.h_feed_len = 71;
            }
         break;

      case 'R':         /** return **/
         config.h_return_len = read_decimal(config.h_return,line+2);
         if (config.h_return[0]==0) {    /** zero means use our own **/
            for(i=1; i < 72; i++)
               config.h_return[i]=32;
            config.h_return[0]   = '.';
            config.h_return[69]  = '.';
            config.h_return[70]  = 13;
            config.h_return_len = 71;
            }
         break;

      case ';':            /** ignore comment lines **/
      case 10:             /** ignore black lines **/
      case ' ':            /** ignore lines that start with a space **/
         break;

      default:
         telluser(0,"unkown command in config.is:",line);
      }
   }

fclose(fp);
#endif

/** set up some reasonble defaults for the rest of the config **/

config.sw  = 640;    /** screen width and height **/
config.sh  = 200;
config.actual_height = 0;

disp_bright_cont(0, 0);     /** set config.translate[] **/

return(TRUE);
}

/**********************************************************/
/*** Given an ascii decimal string, fill array of bytes ***/
/**********************************************************/

int read_decimal(b, d)

unsigned char b[];
char *d;

{
int kount;
int val;
char *strchr();

kount = 0;

do {
   sscanf(d,"%d",&val);
   b[kount++] = val;
   d = strchr(d,',');
   if (d)
      d++;
   } while (d);
return(kount);
}

/*********************************************************/
/** scans the page based on the setup in struct config  **/
/*********************************************************/

void do_scan(first_line)

int first_line;

{
struct IntuiMessage *message,*GetMsg();
extern struct config config;
FILE *fp, *fopen();
int i,j,row,pos;
int delay;
ULONG class;
USHORT code;
float vfeed, new_vfeed, old_frac;
float num_v_feeds();

if (first_line >=400)
   return;

if (first_line==0)
   clr();      /** clear screen **/

fp = fopen("par:","w");
if (fp==0)
   return(FALSE);
init_scanner();
get_byte();
pos = first_line*640;

for (i=0; i<config.init_len; i++)
   send_byte(config.init[i]);

vfeed = num_v_feeds();
old_frac = 0;
Delay(60);        /** give printer time to perform int sequence **/
Disable();                            /** do one dummy pass **/
for (i=0; i<config.h_feed_len; i++)   /** first pass is not timed **/
   send_byte(config.h_feed[i]);       /** the same as the rest **/
Enable();
Delay(60);
Disable();                            /** do one dummy pass **/
if (!(config.h_return[0]==1 && config.h_return_len==1)) {
   for (i=0; i<config.h_return_len; i++)
      send_byte(config.h_return[i]);
   }
Enable();
Delay(config.return_delay);

for (row=first_line; row < config.raw_data_size/640; row++) {

   while ((message=GetMsg(is_window->UserPort))!=0) {
      class=message->Class;
      code=message->Code;
      ReplyMsg(message);
      if (class==RAWKEY && code==0x40) {    /**space bar **/
         goto cancel_scan;
         }
      }

   config.actual_height = row+1;

   Disable();
   for (i=0; i<config.h_feed_len; i++)
      send_byte(config.h_feed[i]);

   for (j=0; j<config.scan_offset; j++) {
      for (delay=0; delay < 30; delay++) ;
      }

   get_byte();
   for (delay=0; delay < config.scan_delay; delay++) ;
   for (j=0; j<640; j++) {
      config.rawdata[pos++]=255-get_byte();
      for (delay=0; delay < config.scan_delay; delay++) ;
      }
   Enable();

   if (config.sh >=400 || config.flags&IMG_NOLACE) {
      for (j=0; j<640; j++) {
         SetAPen(is_window->RPort, config.translate[config.rawdata[row*640+j]]);
         WritePixel(is_window->RPort,j,row);
         if (config.sw > 320) {
            WritePixel(is_window->RPort,j,row);
            }
         else {
            j++;
            WritePixel(is_window->RPort,j/2,row);
            }
         }
      }
   else if ((row&1)==0) {
      for (j=0; j<640; j++) {
         SetAPen(is_window->RPort, config.translate[config.rawdata[row*640+j]]);
         if (config.sw > 320) {
            WritePixel(is_window->RPort,j,row/2);
            }
         else {
            j++;
            WritePixel(is_window->RPort,j/2,row/2);
            }
         }
      }

   if (!(config.h_return[0]==1 && config.h_return_len==1)) {
      for (i=0; i<config.h_return_len; i++)
         send_byte(config.h_return[i]);
      }

   new_vfeed = vfeed + old_frac;
   old_frac = new_vfeed - (float)((int)new_vfeed);
   if (new_vfeed >=1.0) {
      for (i=0; i < (int)new_vfeed; i++) {
         for (j=0; j<config.v_feed_len; j++)
            send_byte(config.v_feed[j]);
         }
      }

   Delay(config.return_delay);
   }

cancel_scan:

fclose(fp);
}

float num_v_feeds()

{
float f;

if  (config.flags&IMG_NOLACE)
   f = ((float)config.scan_height*(float)config.v_dpi)/(200.0*1000.0);
else
   f = ((float)config.scan_height*(float)config.v_dpi)/(400.0*1000.0);
if (f<.01) f = .01;
return(f);
}


#if 0
void do_scan(first_line)
int first_line;
{
char *buf;
unsigned short row, col;

buf = config.rawdata;

Disable();
digi_pix(buf+1);
Enable();
config.actual_height = 400;
refresh_screen();
}
#endif
