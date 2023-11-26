#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include <stdio.h>

#define MSG_COUNT 200

static char *msg_ptr[MSG_COUNT];
static short msg_id[MSG_COUNT];

extern struct IntuiText SubText[];
extern struct MenuItem SubItems[];
extern struct MenuItem Items[];
extern struct IntuiText ItemText[];
extern struct Menu Titles[];
extern int vid_clock;

/** Load msgs from disk **/

init_msg()
{
void free_msg();
short i,j;
int val,len;
FILE *fp;
char t[80];

for (i=0; i<MSG_COUNT; i++)
   msg_ptr[i]=0;

i=0;
fp = fopen("ps.txt","r");
if (fp==0)
   return(TRUE);

fscanf(fp,"%d",&vid_clock);

while (TRUE) {
   fscanf(fp,"%d",&val);
   if (val==999) {
         val = FALSE;      /** TEMP! */
   /*   val = init_menus(fp,i,t); TOOK THIS OUT WHILE STABALIZING MENUS*/
      fclose(fp);
      return(val);
      }
   fgets(t,80,fp);
   if (val!=0) {
      msg_ptr[i]=(char *)AllocMem((len=strlen(t)-1),0);
      if (msg_ptr[i]==0) {
         free_msg();
         fclose(fp);
         return(TRUE);
         }
      for (j=0; j<len; j++)
         if (t[j]==34) t[j]=' ';    /** turn quotes into spaces **/
      movmem(t+1,msg_ptr[i],len);
      msg_ptr[i][len-1]=0;   /** zero newline **/
      msg_id[i++]=val;
   /*printf("i=%d id %d str: %s\n",i,msg_id[i-1],msg_ptr[i-1]); */
   }
   if (i==MSG_COUNT) {
      fclose(fp);
      return(TRUE);
      }
   }
}

init_menus(fp,i,t)

int fp,i;
char *t;

{
int val,wid;
int len,j,k;
int xpos;

for (j=0; j<44; j++) {     /** MUST CHANGE 44 IF ADDING MENUS ITEMS **/
   fscanf(fp,"%d %d",&val,&wid);
   fgets(t,80,fp);
   if (val!=0) {
      msg_ptr[i]=(char *)AllocMem((len=strlen(t)-1),0);
      if (msg_ptr[i]==0) {
         return(TRUE);
         }
      for (k=0; k<len; k++)
         if (t[k]==34) t[k]=' ';    /** turn quotes into spaces **/
      movmem(t+1,msg_ptr[i],len);
      msg_ptr[i][len-1]=0;   /** zero newline **/
      msg_id[i++]=val;
/*   printf("i=%d id=%d wid=%d str: %s\n",i,val,wid,msg_ptr[i-1]); */
      }
   if (i==MSG_COUNT) {
      return(TRUE);
      }

   if (j<11) {
      SubText[j].IText = msg_ptr[i-1];
      SubItems[j].Width = wid;
      }
   else {
      ItemText[j-11].IText = msg_ptr[i-1];
      Items[j-11].Width = wid;
      }
   }

for (j=0; j<5; j++) {
   fscanf(fp,"%d %d %d",&val,&xpos,&wid);
   fgets(t,80,fp);
   if (val!=0) {
      msg_ptr[i]=(char *)AllocMem((len=strlen(t)-1),0);
      if (msg_ptr[i]==0) {
         return(TRUE);
         }
      movmem(t+1,msg_ptr[i],len);
      msg_ptr[i][len-1]=0;   /** zero newline **/
      msg_id[i++]=val;
    /* printf("i=%d id=%d xpos=%d wid=%d str: %s\n",
           i,val,xpos,wid,msg_ptr[i-1]); */
   }
   if (i==MSG_COUNT) {
      return(TRUE);
      }
   Titles[j].LeftEdge = xpos;
   Titles[j].Width = wid;
   Titles[j].MenuName = msg_ptr[i-1];
   }
return(FALSE);
}

void free_msg()
{
int i;

for (i=0; i<MSG_COUNT; i++)
   if (msg_ptr[i]) {
      FreeMem(msg_ptr[i],strlen(msg_ptr[i])+1);
      msg_ptr[i]=0;
      }
}

void show_msg(Window,val)
struct Window *Window;
{
void msg ();
short i;

for (i=0; i < MSG_COUNT; i++)
   if (msg_id[i]==val) {
      msg(Window,msg_ptr[i]);
      return;
      }
}

char *get_msg_ptr(val)
{
short i;

for (i=0; i < MSG_COUNT; i++)
   if (msg_id[i]==val) {
      return(msg_ptr[i]);
      }
}
/**********************************/
/**  write msg to cmd line       **/
/**********************************/

void msg (Window,message)
char *message;
struct Window *Window;
{
telluser(message,0);
}

