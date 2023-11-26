#include "exec/types.h"
#include "desk.h"
#include "laser.h"
#include "stdio.h"
#include "fcntl.h"

#define NUMTOP(x,size) ((((x)<<3)*size)/1000)

struct font_afm_info {
   char *afm_name;      /** adobe file name or null for none exits **/
   char *afm_name_b;    /** adobe file name for bold**/
   char *afm_name_i;    /** adobe file name for italic**/
   char *afm_name_bi;   /** adobe file name for bold-italic **/
   char *people_name;   /** name to use in city desk font requesters **/
   char *font_name;     /** name of amiga bitmap font to use on display **/
   };

UBYTE charnames[256][20];
UWORD pair[1000];
COORD kernval[1000];
int numkerns;
struct font_afm_info f[13];
short append;           /** append to existing file ? **/

main()
{
int out;
char temp[12];
char fname[80];
char globe_sizes[80];
char point_sizes[80];
char *p;
UBYTE id=POSTSCRIPT;
SHORT kount;
SHORT gen_normal, gen_b, gen_i, gen_bi;
int size;
struct font_afm_info *font_data();

font_data();   /** file 'f' structure with its data **/

printf("\n\nPostScript - City Desk Font Generator Ver 1.0\n");
printf("© 1988 SunRize Industries\n\n");
printf("Enter output file name: ");

gets(fname,80);
if (fname[0]==0) {
   exit(10);
   }

out = open(fname, O_RDONLY);

if (out!=-1) {
   close(out);
   printf("\nfile '%s' already exits. (A)ppend or (O)verwrite ? ",fname);
   gets(temp,10);
   temp[0] = tolower(temp[0]);
   }

if (temp[0]=='a') {
   append = TRUE;
   out=open(fname, O_WRONLY|O_APPEND,0);
   }
else {
   out=creat(fname, 0);
   append = FALSE;
   }

if (out==-1) {
   printf("Can't open output file\n");
   exit(10);
   }

printf("\nEnter Global Font Size or Return: ");
gets(globe_sizes,80);

gen_normal = TRUE;
gen_b = TRUE;
gen_i = TRUE;
gen_bi = TRUE;

printf("\nGenerate fonts with no attributes (YES) ? ");
gets(temp,10);
if (tolower(temp[0])=='n')
   gen_normal = FALSE;

printf("Generate bold versions of fonts (YES) ? ");
gets(temp,10);
if (tolower(temp[0])=='n')
   gen_b = FALSE;

printf("Generate italic versions of fonts (YES) ? ");
gets(temp,10);
if (tolower(temp[0])=='n')
   gen_i = FALSE;

printf("Generate bold-italic versions of fonts (YES) ? ");
gets(temp,10);
if (tolower(temp[0])=='n')
   gen_bi = FALSE;

if (!append)
   write(out,&id,1);       /** indicate fonts in this file **/

for (kount = 0; kount < 10; kount++) {
   printf("\n\nWorking on Font '%s'\n",f[kount].people_name);

   if (globe_sizes[0]==0) {
      printf("   Enter list of point sizes to generate: ");
      gets(point_sizes, 80);
      p = point_sizes;
      }
   else {
      p = globe_sizes;
      }

   while (*p!=0) {
      sscanf(p, "%d\n", &size);
      if (gen_normal)
         process_file (f[kount].afm_name,    &f[kount], out, size, 0);
      if (gen_b)
         process_file (f[kount].afm_name_b,  &f[kount], out, size, BOLD);
      if (gen_i)
         process_file (f[kount].afm_name_i,  &f[kount], out, size, ITALIC);
      if (gen_bi)
         process_file (f[kount].afm_name_bi, &f[kount], out, size, BOLD|ITALIC);

      do {        /** skip to next point size **/
         p++;
         } while (isdigit(*p));
      p++;

      if (*p==0)
         break;
      }
   }
close(out);
printf("\n");
}

/**********************************************************************/
/** Given afm file name, size to generate, and other data, genereate **/
/** the City Desk font info and append to 'out' file                 **/
/**********************************************************************/

process_file (fname, f, out, size, flags)

char *fname, *out;
int size, flags;
struct font_afm_info *f;

{
FILE *fp;
struct laserfont lf;
short len;
long fontsize;

if (fname==0)
   return;

printf("   In File '%s' ",fname);

setmem(&lf, sizeof(lf), 0);

fp=fopen(fname,"r");
if (fp==0) {
   printf("Can't open file %s.\n",fname);
   close(out);
   exit (10);
   }

if (readafm(fp,&lf,size, f->people_name)!=AOK) {
   printf("Error during read\n");
   fclose(fp);
   close(out);
   exit(10);
   }

fclose(fp);

lf.style = flags;

write(out,&lf,sizeof(lf));
write(out,&numkerns,4);
if (numkerns!=0) {
   write(out,pair,numkerns<<1);
   write(out,kernval,numkerns<<1);
   }

len = strlen(f->font_name)+1;
write(out,&len,2);         /** write name of screen font to use. **/
write(out,f->font_name,len);
fontsize = 12 << 3;              /** 12 pts **/
write(out,&fontsize,4);
}


/*******************************************************************/
/**** Read PostScript Font Metric file and fill lfont structure ****/
/*******************************************************************/

readafm(fp,lf,size, cd_name)

FILE *fp;
struct laserfont *lf;
int size;                     /** point size for this font **/
char *cd_name;

{
char cmd[80];
int llx,lly,urx,ury;
int i;
char *p;
char **dum;

dum=0;
numkerns=0;
for (i=0; i<256; i++)
   charnames[i][0]=0;

fscanf(fp,"%s",cmd);
if (strcmp(cmd,"StartFontMetrics"))
   return(NOTAFM);
fscanf(fp,"%s",cmd);    /** skip version number **/

lf->place=PSFONT;       /** mark font as a postscript font **/
strcpy(lf->fontname, cd_name);
for (p=lf->fontname; *p!='\0'; p++)
   *p = tolower(*p);

while (TRUE) {

   if (fscanf(fp,"%s",cmd)==EOF)
      return(BADAFM);

   if (!strcmp(cmd,"FontName")) {      /** got name used to turn on font **/
      fscanf(fp,"%s",lf->fonton);
      }

   if (!strcmp(cmd,"FontBBox")) {   /** font box size **/
      fscanf(fp,"%d %d %d %d",&llx,&lly,&urx,&ury);
      lf->size=(size<<3);    /** this is USER size, not real size **/
      lf->depth = NUMTOP(-lly,size);
      lf->height = NUMTOP(ury,size);
      }

   if (!strcmp(cmd,"StartCharMetrics")) {
      getwidths(lf,fp,size);
      }
  
   if (!strcmp(cmd,"StartKernPairs")) {
      getkerns(fp,size);
      }

   if (!strcmp(cmd,"StartTrackKern")) {
      ignorekerns(fp);
      }

   if (!strcmp(cmd,"EndFontMetrics")) {
      printf("found %d kerned pairs.\n",numkerns);
      return(AOK);
      }

   if (!strcmp(cmd,"StartComposites")) {
      while (strcmp(cmd,"EndComposites"))
         if (fscanf(fp,"%s",cmd)==EOF)
            return(BADAFM);
      }
   }
}

ignorekerns(fp)

FILE *fp;

{
char cmd[80];
cmd[0]=0;
while (strcmp(cmd,"EndTrackKern"))
   fscanf(fp,"%s",cmd);
}


getkerns(fp,size)

FILE *fp;

{
char cmd[80];
char line[80];
char line2[80];
char c1;
int i;

fgets(&cmd[0],80,fp);              /** get one line **/

while (TRUE) {
   fscanf(fp,"%s",cmd);
   if (!strcmp(cmd,"EndKernPairs"))
      return(AOK);
   if (strcmp(cmd,"KPX")) {
      printf("don't understand this kern format\n");
      return(AOK);
      }

   fgets(&cmd[0],80,fp);              /** get one line **/
   pair[numkerns]=0;
   sscanf(cmd,"%s %s %d",line,line2,&i);
   c1=charpos(line);
   pair[numkerns]=c1<<8;
   c1=charpos(line2);
   pair[numkerns] |=c1;
   kernval[numkerns++]=NUMTOP(i,size);
   }
}

getwidths(lf,fp,size)
struct laserfont *lf;
FILE *fp;
int size;
{
char line[80];
int num;
int val,index;
int k;
extern UBYTE charnames[256][20];

fscanf(fp,"%d",&num);                   /** number of char. entrys **/
fgets(&line[0],80,fp);              /** get one line **/
while (TRUE) {
   line[0]=0;
   fgets(&line[0],80,fp);              /** get one line **/
   if (!strcmp(line,"EndCharMetrics"))
      return(AOK);                      /** end of char data **/
   if (line[0]!='C')                    /** should be 'C' **/
      return(BADAFM);                   /** bad file **/
   sscanf(&line[1],"%d;",&index);       /** char number **/
   if (index!=-1) {                     /** must be font char. **/
      k=1;                              /** find WX ... **/
      while (line[k]!='X' && k < 80) k++;
      if (line[k-1]!='W')
         return(BADAFM);
      sscanf(&line[k+1],"%d",&val);  /** get width **/
      lf->widthtable[index]=NUMTOP(val,size);

      /** put name into name table **/

      k=1;                              /** find N ... **/
      while (line[k]!='N' && k < 80) k++;
      if (k < 78)
         sscanf(&line[k+1],"%s",charnames[index]);
      else
         charnames[index][0]=0;
      }
   }
}

/************************************/
/*** given name, find it char pos ***/
/************************************/

charpos(str)

char *str;

{
int i;

i=0;

while (strcmp(charnames[i],str) && i < 256)
   i++;

if (i > 255) {
   printf("can't find: %s\n",str);
   i=-1;
   }
return(i);
}

struct font_afm_info *font_data()

{

f[0].afm_name    = "disk1/couri.afm";
f[0].afm_name_b  = "disk1/courib.afm";
f[0].afm_name_i  = "disk1/courio.afm";
f[0].afm_name_bi = "disk1/couribo.afm";
f[0].people_name = "courier";
f[0].font_name   = "courier";

f[1].afm_name    = "disk1/helve.afm";
f[1].afm_name_b  = "disk1/helveb.afm";
f[1].afm_name_i  = "disk1/helveo.afm";
f[1].afm_name_bi = "disk1/helvebo.afm";
f[1].people_name = "helv";
f[1].font_name   = "helvetica";

f[2].afm_name    = "disk1/timesr.afm";
f[2].afm_name_b  = "disk1/timesb.afm";
f[2].afm_name_i  = "disk1/timesi.afm";
f[2].afm_name_bi = "disk1/timesbi.afm";
f[2].people_name = "roman";
f[2].font_name   = "times";

f[3].afm_name    = "disk1/symbol.afm";
f[3].afm_name_b  = 0;
f[3].afm_name_i  = 0;
f[3].afm_name_bi = 0;
f[3].people_name = "symbol";
f[3].font_name   = "symbol";

f[4].afm_name    = "disk1/helven.afm";
f[4].afm_name_b  = "disk1/helvenb.afm";
f[4].afm_name_i  = "disk1/helveno.afm";
f[4].afm_name_bi = "disk1/helvenbo.afm";
f[4].people_name = "helvN";
f[4].font_name   = "helveticaN";

f[5].afm_name    = "disk2/avangbk.afm";
f[5].afm_name_b  = "disk2/avangd.afm";
f[5].afm_name_i  = "disk2/avangbko.afm";
f[5].afm_name_bi = "disk2/avangdo.afm";
f[5].people_name = "garde";
f[5].font_name   = "AvantGarde";

f[6].afm_name    = "disk2/bookml.afm";
f[6].afm_name_b  = "disk2/bookmd.afm";
f[6].afm_name_i  = "disk2/bookmli.afm";
f[6].afm_name_bi = "disk2/bookmdi.afm";
f[6].people_name = "bookman";
f[6].font_name   = "Bookman";

f[7].afm_name    = "disk2/newcsr.afm";
f[7].afm_name_b  = "disk2/newcsb.afm";
f[7].afm_name_i  = "disk2/newcsi.afm";
f[7].afm_name_bi = "disk2/newcsbi.afm";
f[7].people_name = "century";
f[7].font_name   = "newcentury";


f[8].afm_name    = "disk2/palatr.afm";
f[8].afm_name_b  = "disk2/palatb.afm";
f[8].afm_name_i  = "disk2/palati.afm";
f[8].afm_name_bi = "disk2/palatbi.afm";
f[8].people_name = "palatino";
f[8].font_name   = "palatino";


f[9].afm_name    = "disk2/zapfcmi.afm";
f[9].afm_name_b  = 0;
f[9].afm_name_i  = 0;
f[9].afm_name_bi = 0;
f[9].people_name = "chancery";
f[9].font_name   = "ZapfChancery";

return(&f);
}

