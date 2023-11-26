#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"
#include "stdio.h"
#include "fcntl.h"

void main()
{
FILE *fp;
int out;
struct laserfont lf;
char fname[80],line[80],attr[80];
UBYTE id=HPLASER;
char fontname[80];
LONG index,val,fontsize,baseline,cellheight,numkerns;
SHORT len;
SHORT trys=0;
long i,k,size;
char *p;


out=creat("ram:fonts",0);
if (out==-1) {
   printf("Can't open output file\n");
   exit(10);
   }

write(out,&id,1);       /** indicate fonts in this file **/

again:

while (TRUE) {
   printf("Enter HP file to add (X to exit): ");
   gets(fname);

   if ((fname[0]=='X' || fname[0]=='x') && fname[1]==0) {
      printf("Normal exit\n");
      close(out);
      exit(0);
      }

   setmem(&lf,sizeof(lf),0);
   lf.place=HPFONT;

   if (strcmp(fname,"FIXED")==0) { /** not a proportional font! **/
      printf("Enter pitch in 1/8 of a points: ");
      scanf("%d",&val);
      for (index=0; index < 256; index++)
         lf.widthtable[index]=val;

      printf("Enter height in points: ");
      scanf("%d",&size);
      lf.size= size*8;    /** USER size, not real size **/
      size = size * 10;
      lf.height=(size*216*576)/(3000*73);  /*** This is NOT CORRECT! **/
      lf.depth=((size*576)/720) - lf.height;
      }
   else {
      fp=fopen(fname,"r");
      if (fp==0) {
         printf("Can't open file %s.\n",fname);
         if (trys++ < 3)
            goto again;
         close(out);
         exit(10);
         }
    
      fgets(line,80,fp);
      fgets(line,80,fp);
      fgets(line,80,fp);   /** get user height **/
      sscanf(&line[60],"%d",&i);
      lf.size = i*8;
/* printf("size=%d\n",lf.size); */

      fgets(line,80,fp);   /** get baseline **/
      sscanf(&line[71],"%d",&baseline);
/* printf("baseline=%d\n",baseline); */

      fgets(line,80,fp);   /** get cell height **/
      sscanf(&line[73],"%d",&cellheight);
/* printf("cellheighte=%d\n",cellheight); */

      lf.height = lptoc(baseline);
      lf.depth  = lptoc(cellheight - baseline);

      for (i=0; i < 8; i++)
         fgets(line,80,fp);   /** skip to height sizes **/

      while (TRUE) {
         i=fgets(line,80,fp);   /** get line of cell info **/
/* printf("%s",line); */
         if (line[4]==' ' || i==0 || line[0]=='\n')
            break;
         sscanf(&line[2],"%d,",&index);
         sscanf(&line[62],"%d,",&val);
/* printf("i=%d v=%d\n",index,val); */
         lf.widthtable[index]=lptoc(val);
         }
      fclose(fp);
      }

   printf("Enter screen fontname: ");
   gets(fontname);

   printf("Enter screen fontsize: ");
   scanf("%d\n",&fontsize);
   fontsize = fontsize << 3;     /** points to coord **/
/* printf("fs=%d\n",fontsize); */
   gets(line);                /** gets rid of something? **/

   printf("Enter laser font name: ");
   scanf("%s\n",lf.fontname);
   gets(line);                /** gets rid of something? **/

   printf("Enter laser font attributes: ");  /* BIC */
   gets(attr);

   for (p=attr; *p!=' ' && *p!='\0'; p++) {
      if (*p=='B') lf.style |=BOLD;
      if (*p=='I') lf.style |=ITALIC;
      if (*p=='C') lf.style |=COMPRESS;
      }

/* printf("style=%x\n",lf.style); */

   printf("Enter activation esc sequence: ");
   scanf("%s",lf.fonton);
   gets(line);                /** gets rid of something? **/
   k=0;                               /* change all [ to ESC */
   while(lf.fonton[k]!=NULL && k < 30) {
      if (lf.fonton[k]=='[') lf.fonton[k]='\033';
      k++;
      }

   write(out,&lf,sizeof(lf));
   numkerns = 0;
   write(out,&numkerns,4);

   len = strlen(fontname)+1;
   write(out,&len,2);         /** write name of screen font to use. **/
   write(out,fontname,len);
   write(out,&fontsize,4);
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

