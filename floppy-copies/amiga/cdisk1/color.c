#include <stdio.h>
#define MODE_NEWFILE 1006

/*   print colors to okimate color printer */

main()
{

FILE *fp,*Open();
int i,k,j,l;

/* open printer */

fp=Open("KILLME",MODE_NEWFILE);
if (fp == 0) exit(99);

/* generate a test stream of pixels */

for(i=0;i<6;i++) {
   for(k=1;k<6;k++)
      for(j=1;j<6;j++)
         for(l=1;l<6;l++)
            putpix(fp,l*3,j*3,k*3);
   for(k=0;k<75;k++)
      putpix(fp,0,0,0);
   }
Close(fp);
}



/**************************************/
/***                                ***/
/***         Pixel Print            ***/
/***                                ***/
/**************************************/
/*
   This function prints a pixel at the
   next position. Pixels are printed
   left to right until 200 pixels are
   printed. At this time, the position
   pointer goes to the next line. */

putpix(fp,yellow,cyan,magenta)

FILE *fp;
int yellow,cyan,magenta;

{
static int row = 0;
static int pixnum=0;
static char pwr[]={1,2,4,8};
static char g_start[]={27,25};
static char g_end[]  ={13,10};
static char g_mode[] ={27,37,79};
static char yell[6][800];
static char cian[6][800];
static char magn[6][800];
int off,bit,i,k;
char val;

/* if this is a new line, set buffer arrays to zero */

if (row==0 && pixnum==0)
   for(i=0;i<6;i++)
      for(k=0;k<800;k++) {
         yell[i][k]='\0';
         cian[i][k]='\0';
         magn[i][k]='\0';
      }

/* plot yellow dots that go inside this pixel */

off=0;
bit=0;
if (yellow > 0) yellow++;
while (yellow > 0) {
   yell[row][pixnum*4+off]|=pwr[bit];
   for (i=0;i<3;i++)
   if (++bit==4) {bit=0; off++;}
   if (off == 4) off=0;
   yellow--;
   }

/* plot cyan points that go in this pixel */

off=1;
bit=0;
if (cyan > 0) cyan++;
while (cyan > 0) {
   cian[row][pixnum*4+off]|=pwr[bit];
   for (i=0;i<3;i++)
   if (++bit==4) {bit=0; off++;}
   if (off == 4) off=0;
   cyan--;
   }

/* plot magenta points that go in this pixel */

off=2;
bit=0;
if (magenta >0) magenta++;
while (magenta > 0) {
   magn[row][pixnum*4+off]|=pwr[bit];
   for (i=0;i<3;i++)
   if (++bit==4) {bit=0; off++;}
   if (off == 4) off=0;
   magenta--;
   }

/* this pixel is done, point to next one for next call */

if (++pixnum==200) {
   pixnum=0;
   row++;}

/* if this entire line is full, print it. otherwise, just return */

if (row < 6) return(0);

/* reset ribon to begining of yellow */

Write(fp,g_start,2);

/* write line */
/* yellow first */

Write(fp,g_mode,3);           /* 24 X 24 dot mode */
Write(fp,"\040\003",2);       /* 800 coloumns */
   for(k=0;k<800;k++)
      for(i=0;i<6;i+=2) {
         val=(yell[i][k]<<4) | yell[i+1][k];
         Write(fp,&val,1);
      }
Write(fp,"\015",1);

/* Print the magenta line */

Write(fp,g_mode,3);           /* 24 X 24 dot mode */
Write(fp,"\040\003",2);       /* 800 coloumns */
   for(k=0;k<800;k++)
      for(i=0;i<6;i+=2) {
         val=(magn[i][k]<<4) | magn[i+1][k];
         Write(fp,&val,1);
      }
Write(fp,"\015",1);

/* Print the cyan line */

Write(fp,g_mode,3);           /* 24 X 24 dot mode */
Write(fp,"\040\003",2);       /* 800 coloumns */
   for(k=0;k<800;k++)
      for(i=0;i<6;i+=2) {
         val=(cian[i][k]<<4) | cian[i+1][k];
         Write(fp,&val,1);
      }

/* do a c/r line feed */

Write(fp,g_end,1);

row=0;
pixnum=0;
return(0);
}


