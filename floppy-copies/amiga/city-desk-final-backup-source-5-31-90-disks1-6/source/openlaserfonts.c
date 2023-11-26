#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
#include "fcntl.h"


struct Remember *key;
extern struct symtab lfont_table;
extern struct IntuitionBase *IntuitionBase;

/* extern struct text_kern sample_kern;      */

/*************************************/
/*** Read in a list on laserfonts ****/
/*************************************/


openlaserfonts(fname)
char *fname;

{
extern struct laserfont *firstlaserfont;
int fd;
struct laserfont *lf,*oldfont;
int isfirstfont;
char fontname[30];
LONG fontsize;
SHORT fontlen;
SHORT font_error_count;
int len;
LONG numkerns;
UBYTE type;
VOID add_symbol();
oldfont=0;           /** get rid of compiler warning **/

/* printf("file: %s\n",fname); */

font_error_count = 0;
fd=open(fname,O_RDONLY);
if (fd==-1)
   return(CANTOPENFILE);

len=lseek(fd,0L,2);     /** get len of file **/
lseek(fd,0L,0);         /** seek back to begining **/

len--;                  /** adjust for type at start of file **/

read(fd,&type,1);      /** get font type **/
/* printf("type %d\n",type); */
if (type!=HPLASER && type!=POSTSCRIPT) {
   close(fd);
   return(BADFILEFORMAT);
   }

if (firstlaserfont) {   /** This is not the first font file... **/
   for (oldfont=firstlaserfont; oldfont->next; oldfont=oldfont->next) ;
   isfirstfont=FALSE;
   }
else {
   isfirstfont=TRUE;
   }

while (len > 0) {
   lf=(struct laserfont *)
       AllocRemember(&key,sizeof(struct laserfont),MEMF_CLEAR);
   if (lf==NULL) {
      FreeRemember(&key,TRUE);
      key=NULL;
      close(fd);
      firstlaserfont=NULL;
      return(OUTOFMEM);
      }

   if (read(fd,lf,sizeof(struct laserfont)) != sizeof(struct laserfont)) {
      close(fd);
      firstlaserfont=NULL;
      FreeRemember(&key,TRUE);
      key=NULL;
      return(BADFILEFORMAT);
      }

   convert_laser(lf);
   len = len - sizeof(struct laserfont);
   read(fd,&numkerns,4);
 /* printf("numkerns=%d\n",numkerns); */
   len = len - 4;       /** keep track of how much file left to read **/
   lf->kern=0;
   if (numkerns > 0) {
      lf->kern=
         (struct text_kern *)AllocRemember(&key,sizeof(struct text_kern),0);
      if (lf->kern==NULL) {
         FreeRemember(&key,TRUE);
         key=NULL;
         close(fd);
         firstlaserfont=NULL;
         return(OUTOFMEM);
         }
      lf->kern->num=numkerns;
      lf->kern->pairs=(UWORD *)AllocRemember(&key,numkerns<<1,0);
      lf->kern->value=(COORD *)AllocRemember(&key,numkerns<<1,0);
      if (lf->kern->pairs==NULL || lf->kern->value==NULL) {
         FreeRemember(&key,TRUE);
         key=NULL;
         close(fd);
         firstlaserfont=NULL;
         return(OUTOFMEM);
         }
      read(fd,lf->kern->pairs,numkerns<<1);
      read(fd,lf->kern->value,numkerns<<1);
      convert_kerns(lf->kern->value, numkerns);
      len = len - (numkerns<<2);
      }

   read(fd,&fontlen,2);
 /* printf("fontlen %d\n",fontlen); */
   fontlen = min(29,fontlen);
   read(fd, fontname, fontlen);
   fontname[fontlen] = 0;
 /* printf("fontname: %s\n",fontname); */
   if (read(fd,&fontsize,4L)!=4) {
         FreeRemember(&key,TRUE);
         key=NULL;
         close(fd);
         firstlaserfont=NULL;
         return(BADFILEFORMAT);
         }
 /* printf("fontsize = %d\n",fontsize); */
   len = len - 6 - fontlen;
   lf->screenfont=screen_openfont (fontname, fontsize, lf->style);
   if (lf->screenfont==NULL) {
      if (font_error_count++ < 4) {
         telluser2(0,"Can't open Amiga Screen font!",
            fontname,FALSE);
            }
      lf->screenfont=IntuitionBase->ActiveScreen->RastPort.Font;   /** get known font **/
      }

   if (isfirstfont==TRUE) {
      firstlaserfont=lf;
      isfirstfont=FALSE;
      }
   else
      oldfont->next=lf;     /* create pointer from previous font to this **/

   oldfont=lf;

   add_symbol(&lfont_table, lf->fontname, 0);

   /*printf("screenfont=%x\n",lf->screenfont);
   printf("sfn=%s\n",lf->screenfont->tf_Message.mn_Node.ln_Name);*/
   }

close(fd);
return(AOK);
}

/*****************************************/
/** Free all laserfont structure memory **/
/*****************************************/

closelaserfonts()
{
extern struct laserfont *firstlaserfont;
VOID free_symbol_table();

if (firstlaserfont!=NULL)   /** make sure there is a font chain to close **/
   FreeRemember(&key,TRUE);
key=NULL;
firstlaserfont=NULL;
free_symbol_table(&lfont_table);
}

convert_laser(lf)

struct laserfont *lf;

{
int i;

for (i=5; i<256; i++)   /** changed from zero to allow larger fonton cmd */
   lf->widthtable[i] = (lf->widthtable[i]*300)/576;

lf->height = (lf->height*300)/576;
lf->depth  = (lf->depth*300)/576;
/* lf->size   = (lf->size*300)/576; */
}

convert_kerns(value, num)

COORD *value;
int num;
{
int i;

for (i=0; i < num; i++)
   value[i] = (value[i]*300)/576;
}

