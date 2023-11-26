#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"
#include "stdio.h"
#include "fcntl.h"

#define FONT_HEADER    0x2973
#define CHAR_CODE      0x2a63
#define DOWNLOAD_CHAR  0x2873

hp_soft_to_cd(r_path, w_path, fontname, fontsize, cd_name, cd_size, font_id,
              attr)

char *r_path;     /** disk path of soft font to create .hp files from **/
char *w_path;     /** name of .hp file to create **/
char *fontname;   /** name of screen font to represent this font **/
LONG fontsize;    /** size of screen font in points **/
char *cd_name;    /** name to call font inside city desk **/
int cd_size;      /** size of this laserfont in points according to cd **/
int font_id;      /** number to id this font inside laser printer **/
int attr;         /** attributed for this font (BOLD, ITALIC, etc.) **/

{
int w_fh, r_fh;
int append;
struct laserfont lf;
UBYTE id=HPLASER;
short len;
long numkerns;
char msg[200];

append = FALSE;
w_fh = open(w_path, O_RDONLY);

if (w_fh!=-1) {
   close(w_fh);
   append = overwrite(0);
   }

if (append) {
   w_fh=open(w_path, O_WRONLY|O_APPEND,0);
   }
else {
   w_fh=creat(w_path, 0);
   }

if (w_fh==-1) {
   stat("Can't open output file.");
   return;
   }

if (!append)
   write(w_fh,&id,1);       /** indicate fonts in this file **/

r_fh = open(r_path, O_RDONLY,0);
if (r_fh==-1) {
   sprintf(msg,"Can't open file '%s'",r_path);
   stat(msg);
   close(w_fh);
   return(0);
   }

setmem(&lf,sizeof(lf),0);
/* lf.widthtable[32] = 1; */

while (process_esc(r_fh, &lf));
close (r_fh);

if (lf.widthtable[32]==0)
   lf.widthtable[32] = lf.widthtable[0x69];  /** use width of 'i' **/

lf.place=HPFONT;
lf.size = cd_size*8;
/* printf("size=%d\n",lf.size); */
strcpy(lf.fontname, cd_name);
lf.style = attr;
/* printf("style=%x\n",lf.style); */
/* sprintf(lf.fonton,"\033(%dX",font_id); */
write(w_fh,&lf,sizeof(lf));
numkerns = 0;
write(w_fh,&numkerns,4);

len = strlen(fontname)+1;
write(w_fh,&len,2);         /** write name of screen font to use. **/
write(w_fh,fontname,len);
fontsize = fontsize << 3;     /** points to coord **/
/* printf("fs=%d\n",fontsize); */
write(w_fh,&fontsize,4);
close (w_fh);
stat("Font Created.");
}


process_esc(fh, lf)

int fh;
struct laserfont *lf;

{
unsigned short word;
unsigned char buf[100];
static int current_char;
int len,val,symnum,v1,v2;
long baseline,cellheight;
float fpts,fpitch;
static char *sym[] = {"8U","0U","0E","0D","1E","0F","0G","0I","0S","1S","1U","0B",
               "8M","0Q","1Q","0A","2Q"};

if (read(fh, buf, 1)!=1)
   return(FALSE);

if (buf[0]==0)
   return(TRUE);           /** skip zeros **/

if (buf[0]!=0x1b) {       /* esc */
   stat("ESC not found when expected!");
   return(FALSE);
   }

read(fh, &word, 2);

if (word==FONT_HEADER) {
/*    printf("font_header\n"); */
   read(fh, buf, 2);
   val = 10*(buf[0]-'0')+buf[1]-'0';
   read(fh, buf, val+1);
   baseline = buf[7]*256+buf[8];
   cellheight = buf[11]*256+buf[12];

/* printf("baseline=%d\n",baseline);
printf("cellheighte=%d\n",cellheight); */

   lf->height = lptoc(baseline);
   lf->depth  = lptoc(cellheight - baseline);

   fpts = buf[19]*256+buf[20];
   fpts = 72.*fpts/1200.;

   fpitch = buf[17]*256+buf[18];
   fpitch = 1200./fpitch;

/* printf("height in pts %f\n",fpts); */

   for (symnum=0; symnum <16 ; symnum++) {      /** actualy 17 codes **/
      v1 = *sym[symnum]-'0';
      v2 = sym[symnum][1] - 64;
 /* printf("val %d\n",v1*32+v2); */
      if ( (v1*32+v2) == (buf[15]*256+buf[16])) break;
      }

 /* printf("[&l%dO[(%s[(s%dP%2.2fH%2.2fV%dS%dB%dT",
       buf[13],sym[symnum],buf[14],fpitch,fpts,buf[24],buf[25],buf[26]); */

  sprintf(lf->fonton,"\033&l%dO\033(%s\033(s%dp%2.2fh%2.2fv%ds%db%dt",
       buf[13],sym[symnum],buf[14],fpitch,fpts,buf[24],buf[25],buf[26]);

   return(TRUE);
   }

else if (word==CHAR_CODE) {
   read(fh, buf, 3);
   if (buf[2]=='E') {
      current_char = 10*(buf[0]-'0')+buf[1]-'0';
      }
   else {
      current_char = 100*(buf[0]-'0')+10*(buf[1]-'0')+(buf[2]-'0');
      read(fh, buf, 1);
      if (buf[0]!='E')
         stat("Invalid esc code encounterd 1");
      }
/*    printf("char code %d\n",current_char); */
   return(TRUE);
   }
else if (word==DOWNLOAD_CHAR) {
   read(fh, buf, 3);
   if (buf[2]=='W') {
      len = 10*(buf[0]-'0')+buf[1]-'0';
      }
   else {
      len = 100*(buf[0]-'0')+10*(buf[1]-'0')+(buf[2]-'0');
      read(fh, buf, 1);
      if (buf[0]!='W') {
         len = len*10+buf[0]-'0';
         read(fh, buf, 1);
         if (buf[0]!='W') {
            stat("Invalid esc code encounterd 2");
            }
         }
      }
   read(fh, buf, 16);      /** get character width info, etc **/
   val = (256*buf[14] + buf[15])/4;
/*    printf("download char i=%d v=%d len %d\n",current_char,val,len); */
   lf->widthtable[current_char]=lptoc(val);
   lseek(fh, len-16, 1L);  /** discard font info **/
   return(TRUE);
   }
else {
   stat("Unexpected command in font file!");
   return(FALSE);
   }
}


/*************************************/
/*** lptoc -- laserprinter to coord **/
/*************************************/

lptoc(i)
int i;
{
return((i*576)/300);
}


delete_all_fonts(fp)
{
fprintf(fp,"\033*c0F");
}

reset_printer(fp)
{
fprintf(fp, "\033E");
}

self_test(fp)
{
fprintf(fp, "\033z");
}

download_font(fp, path, font_id)

FILE *fp;      /** printer file **/
char *path;    /** soft font path to open **/
int font_id;   /** id to call this font when we download it **/

{
int fh,len;
unsigned char byte[256];
char msg[60];

fh = open(path, O_RDONLY,0);
if (fh==-1) {
   sprintf(msg,"Can't open file '%s'",path);
   stat(msg);
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

