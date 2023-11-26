

#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


void main () {

struct Screen *screen;
static char string[2000];
char *string2;
int *f;
int i,j,status;
char c;

openlibs();
/*
f = open("df1:testfile1",0);
i = 0;
j = lseek(f,0L,2);
lseek(f,0L,0);
read(f,&string[0],j);
string[j] = 0;
close(f);

printf("stringlen=%d\n",strlen(&string[0]));
  */
screen = openscreen(640,200,2);
SetRGB4(&(screen->ViewPort),0,15,15,15);
SetRGB4(&(screen->ViewPort),1,10,10,10);
SetRGB4(&(screen->ViewPort),2,5,5,5);
SetRGB4(&(screen->ViewPort),3,0,0,0);

    /*
string2 = texted(screen,&string[0],200,10,10,400,80,&status);
     */

string2 = texted(screen,0, 0,10,10,600,80,&status);

CloseScreen(screen);
printf("status = %d\n",status);

if (status == 2) {
   printf("%s",string2);
   FreeMem(string2,strlen(string2)+1);
   }
}

/* dummy sub for converting text on a load  */

textconv(src_stt,len,dest_ret,len_ret)
char *src_stt,**dest_ret;
int len,*len_ret;
{

*dest_ret = src_stt;
*len_ret = len;

return(AOK);
}


