#include "exec/types.h"
#include "psound.h"

/** use -fim for mixed mode, ieee libraries **/

int resample(Window, cur)

struct Window *Window;
int cur;
{
char *AllocMem();
extern struct Samp samp[MAX_SAMPLES];
int i,j,k;
float flt;
float fnewrate,foldrate;
float fnewlen,foldlen;

   if ((i=getslot(Window))==-1) {
      show_msg(Window,146);
      return(cur);
      }
   samp[i].name=AllocMem(strlen(samp[cur].name)+3,0);
   if (samp[i].name==0) {
      nomem(Window);
      return(cur);
      }
   strcpy(samp[i].name,samp[cur].name);
   strcpy(samp[i].name+strlen(samp[cur].name),"-2");
   samp[i].rate=getint(Window,samp[cur].rate,2000,50000, get_msg_ptr(241),350);
   fnewrate = (float)samp[i].rate;
   foldrate = (float)samp[cur].rate;
   foldlen = (float)samp[cur].length;
   samp[i].length=even((int)(fnewrate*foldlen/foldrate));
   fnewlen = (float)samp[i].length;
/* printf("old len %f rate %f new len %f rate%f\n",foldlen,foldrate,fnewlen,fnewrate); */
   samp[i].lmem=AllocMem(samp[i].length, 0);
   if (samp[i].lmem==0) {
      FreeMem(samp[i].name,strlen(samp[i].name)+1);
      samp[i].lmem=0;
      samp[i].rmem=0;
      samp[i].length=0;
      samp[i].oneshot=0;
      nomem(Window);
      return(cur);
      }
   show_msg(Window, 112);
   for (j=0; j < samp[i].length; j++) {
      flt = foldlen*(float)j;
      k = (int)(flt/fnewlen);
      samp[i].lmem[j]=samp[cur].lmem[k];
      }
   if (samp[cur].rmem==samp[cur].lmem) {     /** mono **/
      samp[i].rmem=samp[i].lmem;
      }
   else {
      samp[i].rmem=AllocMem(samp[i].length, 0);
      if (samp[i].rmem==0) {
         FreeMem(samp[i].lmem, samp[i].length);
         FreeMem(samp[i].name,strlen(samp[i].name)+1);
         samp[i].lmem=0;
         samp[i].rmem=0;
         samp[i].length=0;
         samp[i].oneshot=0;
         nomem(Window);
         return(cur);
         }
      for (j=0; j < samp[i].length; j++) {
         flt = foldlen*(float)j;
         k = (int)(flt/fnewlen);
         samp[i].rmem[j]=samp[cur].rmem[k];
         }
      }
   samp[i].type=samp[cur].type;
   samp[i].oneshot=0;
   PutName(Window,samp[cur].name,cur,REG_VID);
   PutName(Window,samp[i].name,i,REV_VID);
   cur=i;
   show_msg(Window,147);
   return(cur);
}
