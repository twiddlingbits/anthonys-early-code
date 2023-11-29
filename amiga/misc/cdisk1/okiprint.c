#include <stdio.h>
#define MODE_NEWFILE 1006
#define MODE_OLDFILE 1005

/* Change 'hold and modify' screen dump to okimate printer dump  */
/* Copyright 1986 Anthony J. Wood */
/* 503 Timber Ridge Apt#210 */
/* College Station, TX 77840 */
/* April 19, 1986 */


main(argc,argv)
int argc;
char *argv[];

{

FILE *fp,*in,*Open();
int row;
unsigned int i,len,column;
unsigned int control,color,byte,bit,val,index;
static unsigned char dump[48000];
static unsigned int red[200],green[200],blue[200];

/* enter '?' as command? */
if ((*++argv)[0]=='?' || argc !=2) {
   printf("This program will take a bit plane dump of a 'hold and\n");
   printf("modify' screen and print it on an okimate 20 color printer.");
   printf("\nThe digiviewer demo pictures are examples of the type of\n");
   printf("files that can be printed. This mode allows 4096 possible\n");
   printf("colors on the screen or printer simultaneously.\n\n");
   printf("Use the format: \n\n");
   printf("%s <filename to print>\n\n",*(argv-1));
   printf("Copyright by Anthony J. Wood 4-19-86\n\n");
   exit(0);
   }


printf("\nOkimate 20 'Hold and modify' screen print.\nBy Anthony J. Wood");
printf(" 4-19-86\n\n");

/* open output and input file */

fp=Open("PAR:",MODE_NEWFILE);
if (fp == 0) {
   printf("Can't open device PAR:\n");
   exit(20);
   }
in=Open(*argv,MODE_OLDFILE);
if (in == 0) {
   printf("Can't open input file!\n");
   exit(20);
   }

/* read in Screen dump */

len=Read(in,dump,48000);
if (len != 48000) {
   printf("Can't read input file!\n");
   exit(20);
   }


/* convert file to okimate format */

for(column=0;column<320;column++)
   for(row=199;row>=0;row--) {
      val=0;
      for(i=0;i<6;i++) {
         index=i*8000+row*40+(column>>3);
         byte=dump[index];
         bit=(byte>>(7-(column&7)))&1;
         val|=bit<<i;
         }

/* seperate control information from color info */

      control=val>>4;
      color=val&15;

/* Use control and color info to find correct color for this pixel */

      if (control == 1) blue[row]=color;
      if (control == 2) red[row]=color;
      if (control == 3) green[row]=color;
      if (control == 0)
         printf("Warning: This file references a color register.\n");
      putpix(fp,15-blue[row],15-red[row],15-green[row]);
      }

/* print nulls to flush out last line from line buffer */

    for(i=0;i<650;i++)
       putpix(fp,0,0,0);

/* close printer and input file */

Close(fp);
Close(in);
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
   pointer goes to the next line.
   No pixels are actualy printer until
   the line buffer is filled     */

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

off=1;
bit=0;
/* if (yellow > 0) yellow--; */
while (yellow > 0) {
   yell[row][pixnum*4+off]|=pwr[bit];
   for (i=0;i<3;i++)
   if (++bit==4) {bit=0; off++;}
   if (off == 4) off=0;
   yellow--;
   }

/* plot cyan points that go in this pixel */

off=1;
bit=1;
/* if (cyan > 0) cyan--; */
while (cyan > 0) {
   cian[row][pixnum*4+off]|=pwr[bit];
   for (i=0;i<3;i++)
   if (++bit==4) {bit=0; off++;}
   if (off == 4) off=0;
   cyan--;
   }

/* plot magenta points that go in this pixel */

off=1;
bit=2;
/* if (magenta >0) magenta--; */
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

Write(fp,g_end,2);

row=0;
pixnum=0;
return(0);
}


