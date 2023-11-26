#include "exec/types.h"
#include "desk.h"
#include "laser.h"
#include "stdio.h"
#include "fcntl.h"

/** This file should be maintained in parallel with hp.c & main.c **/
/** of the laser utility module **/

hp_font_setup(path)

char *path;

{
int id;
FILE *list_fp;
FILE *fp;
char line[160];
char file[160];
char fname[80];
char dummy[20];
int size, style;
struct laserfont *laser_openfont();
struct laserfont *lf;
extern struct printerprefs printerprefs;

setdefport();     /** sets par: or ser: **/

if (printerprefs.flags & USE_PAR) {
   fp = fopen("par:", "w");
   }
else {
   fp = fopen("ser:", "w");
   }

if (fp==0) {
   return(CANCEL);
   }

list_fp = fopen(path, "r");
if (list_fp==0) {
   fclose(fp);
   return(CANCEL);
   }

open_font_msg();

while (fgets(line, 80, list_fp)) {

switch (line[0]) {

   case 'R':
   case 'r':
      fprintf(fp,"\033*c0F"); /** delete fonts **/
      fprintf(fp,"\033E");    /** reset printer **/
      break;

   case 'D':
   case 'd':
      sscanf(line, "%s %s %d",dummy, file, &id);
      download_font(fp, file, id);
      break;

   case 'X':      /** download & change fontchange sequence **/
   case 'x':
      sscanf(line, "%s %s %d %s %d %d",dummy, file, &id, fname, &size, &style);
      download_font(fp, file, id);
      lf = laser_openfont(fname, size*8, style);
      if (lf)
         sprintf(lf->fonton,"\033(%dX",id);
      break;
      }
   }
fclose(list_fp);
fclose(fp);
close_font_msg();
return(AOK);
}

download_font(fp, path, font_id)

FILE *fp;      /** printer file **/
char *path;    /** soft font path to open **/
int font_id;   /** id to call this font when we download it **/

{
int fh,len;
unsigned char byte[256];

fh = open(path, O_RDONLY,0);
if (fh==-1) {
   return(FALSE);
   }

fprintf(fp, "\033*c%dD",font_id);          /** set font id **/

while ((len=read(fh, byte, 256))==256) {           /** copy font to printer **/
   fwrite(byte, 256, 1, fp);
   }
if (len)
   fwrite(byte, len, 1, fp);


close(fh);

fprintf(fp, "\033*c%dD",font_id);          /** set font id **/
fprintf(fp,"\033*c5F\033(%dX",font_id);  /** make perm & primary **/
return(TRUE);
}

