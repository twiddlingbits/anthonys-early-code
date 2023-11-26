#include "libraries/dos.h"


/**************************/
/**  about perfect sound **/
/**************************/

aboutps(Window)


struct Window *Window;
{
int fp;

fp=Open("RAW:56/0/528/200/About Perfect Sound",MODE_NEWFILE);
if (fp==0) {
   msg(Window,"ERROR -- can't open AmigaDOS RAW:");
   return(0);
   }

pr(fp,"");
pr(fp,"This program, \"The Perfect Sound Sound Editor,\" is");
pr(fp,"copyrighted 1987 by Anthony J. Wood. You are free");
pr(fp,"to distribute copies of this program for non-commercial ");
pr(fp,"use.  Use of this program for more than two weeks ");
pr(fp,"without either:\n");
pr(fp,"1. Buying the Perfect Sound Stereo Digitizer, or");
pr(fp,"2. Sending $20 to SunRize Industries for this Software\n");
pr(fp,"is prohibited. $20 will buy you a manual, update info, ");
pr(fp,"source code, and our appreciation.\n");
pr(fp,"This sound editor was written to support the \"Perfect ");
pr(fp,"Sound\" Audio digitizer. You can purchase the Perfect");
pr(fp,"Sound stereo digitizer, this editor with source code,");
pr(fp,"Manual, and a library of recorded sounds (gunshots,");
pr(fp,"splashes, laughing, etc.) for $89.95 from the address");
pr(fp,"below.  Visa and Mastercard are welcome, and dealer");
pr(fp,"inquiries are invited.");
waitkey(fp);
pr(fp,"\n\n\n\n\n\n");
pr(fp,"                    SunRize Industries");
pr(fp,"                      PO Box 1453");
pr(fp,"                College Station, TX 77841");
pr(fp,"                    (409) 846-1311");
waitkey(fp);
Close(fp);
}


