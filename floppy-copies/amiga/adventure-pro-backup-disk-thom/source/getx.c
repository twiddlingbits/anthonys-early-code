

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


getx (wprint)
struct winprint *wprint;
{

extern int objnum[2],objx[2][10],objy[2][10],objlist[2][100],
        noun_pics[MAXLIST];
extern struct Image *object_images[MAXLIST];
extern char *nounlist[MAXLIST];
extern int extlist[MAXLIST];
extern int secnum;
struct Image *tpoint;
struct IntuiMessage *mess;
int z,zz,q,i,l,j,ex1,ex2;
extern char command[80],last_com[80];
extern int spckeys,keys[3][20];
LONG mclass;
USHORT mcode[2],mqual;
SHORT Mx,My;
char temp1[2],temp_com[80];
extern struct sequence *sequences[MAXLIST];
extern int varval[MAXLIST],xnum,ynum;
int k,ii,jj,ll;
struct sequence *a;
struct Image *b,*bb;
struct BitMap *c;

static char key_code_table1[128] =
{0 ,'1','2','3','4','5','6','7','8','9','0','-','=','\\', 0 ,'0',
'q','w','e','r','t','y','u','i','o','p','[',']', 0 ,'1','2','3',
'a','s','d','f','g','h','j','k','l',';', 0 , 0 , 0 ,'4','5','6',
 0 ,'z','x','c','v','b','n','m',',','.','/', 0 ,'.','7','8','9',
' ', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

static char key_code_table2[128] = {
'~','!','@','#','$','%','^','&','*','(',')','_','+','|', 0 ,'0',
'Q','W','E','R','T','Y','U','I','O','P','{','}', 0 ,'1','2','3',
'A','S','D','F','G','H','J','K','L',':','"', 0 , 0 ,'4','5','6',
 0 ,'Z','X','C','V','B','N','M','<','>','?', 0 ,'.','7','8','9',
' ', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

newpointer(wprint->WIN->WScreen,NORMAL);

ReportMouse(TRUE,wprint->WIN);
ModifyIDCMP(wprint->WIN, INTUITICKS | RAWKEY | MOUSEBUTTONS);

j = 0;
l = 0;
temp1[1] = 0;
printz(wprint,"_^b");
do {

   Wait(1<<wprint->WIN->UserPort->mp_SigBit);
   while(mess = GetMsg(wprint->WIN->UserPort)) {

      mclass = mess->Class;
      mcode[0] = mess->Code;
      mqual = mess->Qualifier;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == RAWKEY && mcode[0] < 128) {
         z = 0;
         while (z<spckeys) {
            if (mcode[0] == keys[0][z])
               if ((mqual & 255) == 0 || (mqual & keys[1][z])) {
                  zz=0;
                  while(zz++ < l)
                     printz(wprint," ^b^b");
                  strcpy(&command[0],extlist[keys[2][z]]);
                  zz = strlen(&command[0])-1;
                  if (command[zz] == '^') {
                     command[zz] = 0;
                     printz(wprint,&command[0]);
                     printz(wprint,"_^b");
                     l = zz;
                     }
                  else { 
                     printz(wprint,&command[0]);
                     printz(wprint,"^n");
                     goto ret1;
                     }
                  }
            z++;
            }

         if ((mqual & 255) == 0 && key_code_table1[mcode[0]] != 0 && l<80) {
            ReportMouse(FALSE,wprint->WIN);
            temp1[0] = key_code_table1[mcode[0]];
            printz(wprint,&temp1[0]);
            ReportMouse(TRUE,wprint->WIN);
            command[l++] = key_code_table1[mcode[0]];
            printz(wprint,"_^b");
            }
         else if ((mqual & 248) == 0 && key_code_table2[mcode[0]] != 0 && l<80) {
            ReportMouse(FALSE,wprint->WIN);
            temp1[0] = key_code_table2[mcode[0]];
            printz(wprint,&temp1[0]);
            ReportMouse(TRUE,wprint->WIN);
            command[l++] = key_code_table2[mcode[0]];
            printz(wprint,"_^b");
            }
         if (mcode[0] == 67 || mcode[0] == 68) {
            ReportMouse(FALSE,wprint->WIN);
            printz(wprint," ^n");
            ReportMouse(TRUE,wprint->WIN);
            command[l++] = 0;
            goto ret1;
            }
         else if (mcode[0] == 65 && l > 0) {
            ReportMouse(FALSE,wprint->WIN);
            printz(wprint," ^b^b_^b");
            ReportMouse(TRUE,wprint->WIN);
            l--;
            }
         else if (mcode[0] == 76 ) {
            ReportMouse(FALSE,wprint->WIN);
            command[l++] = 0;
            z=0;
            while(++z < l)
               printz(wprint," ^b^b");
            l=0;
            l=strlen(&last_com[0]);
            strcpy(&temp_com[0],&command[0]);
            strcpy(&command[0],&last_com[0]);
            strcpy(&last_com[0],&temp_com[0]);
            printz(wprint,&command[0]);
            printz(wprint,"_^b");
            ReportMouse(TRUE,wprint->WIN);
            }
         else if ((mqual & 64) && mcode[0] == 50 ) {
            ReportMouse(FALSE,wprint->WIN);
            command[0] = 0;
            z=0;
            while(z++ < l)
               printz(wprint," ^b^b");
            printz(wprint,"_^b");
            l=0;
            ReportMouse(TRUE,wprint->WIN);
            }
         }

      if (mclass == MOUSEBUTTONS) {
         if (mcode[0] == SELECTDOWN) {
            i = 0;
            while (i<objnum[1] ) {
            if (objlist[1][i] > 0) {
               q = objlist[1][i];
               tpoint = object_images[q];
               if( Mx >= objx[1][i] && My >= objy[1][i] && Mx <=
               objx[1][i] + tpoint->Width && My <= objy[1][i] + tpoint->Height) {
                  j=0;
                  while (noun_pics[j] != q)
                     j++;
                  strcpy(&command[0],"GET ");
                  strcpy(&command[4],nounlist[j]);
                  z=0;
                  while(z++ < l)
                     printz(wprint," ^b^b");
                  printz(wprint,&command[0]);
                  printz(wprint,"^n");
                  goto ret1;
                  }
               }
            if (objlist[1][i] < 0) {
               a = sequences[(objlist[1][i] * -1) - 1];
               c = &(a->background);
               ex1 = c->BytesPerRow *8;
               ex2 = c->Rows;
               if( Mx >= objx[1][i] && My >= objy[1][i] && Mx <=
               objx[1][i] + ex1 && My <= objy[1][i] + ex2) {
                  j=0;
                  while (noun_pics[j] != objlist[1][i])
                     j++;
                  strcpy(&command[0],"GET ");
                  strcpy(&command[4],nounlist[j]);
                  z=0;
                  while(z++ < l)
                     printz(wprint," ^b^b");
                  printz(wprint,&command[0]);
                  printz(wprint,"^n");
                  goto ret1;
                  }
               }
               i++;
               }
            i = 0;
            while (i < objnum[0]) {
               if (objlist[0][i] > 0) {
                  q = objlist[0][i];
                  tpoint = object_images[q];
                  if( Mx >= objx[0][i] && My >= objy[0][i] && Mx <=
                  objx[0][i] + tpoint->Width && My <=
                  objy[0][i] + tpoint->Height) {
                     j=0;
                     while (noun_pics[j] != q)
                        j++;
                     strcpy(&command[0],"DROP ");
                     strcpy(&command[5],nounlist[j]);
                     z=0;
                     while(z++ < l)
                        printz(wprint," ^b^b");
                     printz(wprint,&command[0]);
                     printz(wprint,"^n");
                     goto ret1;
                     }
                  }
               if (objlist[0][i] < 0) {
                  a = sequences[(objlist[0][i] * -1) - 1];
                  c = &(a->background);
                  ex1 = c->BytesPerRow *8;
                  ex2 = c->Rows;
                  if( Mx >= objx[0][i] && My >= objy[0][i] && Mx <=
                  objx[0][i] + ex1 && My <= objy[0][i] + ex2) {
                     j=0;
                     while (noun_pics[j] != objlist[0][i])
                        j++;
                     strcpy(&command[0],"DROP ");
                     strcpy(&command[5],nounlist[j]);
                     z=0;
                     while(z++ < l)
                        printz(wprint," ^b^b");
                     printz(wprint,&command[0]);
                     printz(wprint,"^n");
                     goto ret1;
                     }
                  }
               i++;
               }
            varval[xnum] = Mx;
            varval[ynum] = My;
            strcpy(&command[0],"MOUSECLICK ");
            printz(wprint," ^b");
            goto ret2;
            }
         }
      if (mclass == INTUITICKS) {
         updatesec(wprint);
         } /* intuiticks end */
      }
   } while(TRUE) ;
   ret1:
   if (command[0] !=0)
      strcpy(&last_com[0],&command[0]);
   ret2:
   srand(command[2]);
   ReportMouse(FALSE,wprint->WIN);
   newpointer(wprint->WIN->WScreen,BUSY);

}



