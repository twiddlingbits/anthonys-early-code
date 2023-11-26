
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "texted.h"


remakeline(rp,string,l,ld,maxline,length)
struct RastPort *rp;
char *string;
int l,maxline,*length;
struct linedata *ld;
{

struct linedata *ld2;
int len,len2,len3,i,j;
char tempstring[300],*mover,*p1,*p2;
extern struct screenline line[50];

SetAPen(rp,GRAY1);
SetBPen(rp,0);
SetDrMd(rp,JAM2);

p1 =string + ld->start[line[l].start];
p2 =string + ld->start[line[l+1].start];

if (line[l].start < l) {
   ld2 = ld->next;
   p1 =string + ld2->start[line[l].start];
   }
if (line[l+1].start < l+1) {
   ld2 = ld->next;
   p2 =string + ld2->start[line[l+1].start];
   }
mover =p2;
if (line[l+1].start < 0) {
   mover = p1 + line[l].l2;
   }
/*  if (*(mover - 1) == ' ')
  mover--; */

len = strlen(string);
len2 = strlen(mover);

j = 0;
i = 0;
while (i<line[l].l) {
   if (line[l].letters[i] == -74)  {
      tempstring[j++] = '\n';
      }
   else
      tempstring[j++] = line[l].letters[i];
   i++;
   }

tempstring[j] = 0;
len3 = strlen(tempstring);


if (len2>0 /* && line[l+1].start != -1 && l != maxline -1 */ ) {
   movmem(mover,p1 + len3,len2+1);
   *(p1 + len3 + len2 + 1) = 0;
   }
else
   *(p1 + len3) = 0;

if (j != 0) {
   movmem(tempstring,p1,len3);
   }
*length = strlen(string);

line[l].l2 = line[l].l;


}


patternsearch(string,target,max)
char *string,*target;
int max;
{

int i,j;
char *p1,*p2,*maxp;

p1 = string;
j = strlen(target);
p2 = string + strlen(string) - j;
maxp = p2;
if (max != -1)
   maxp = max + p1;

while (p1 < maxp) {

   i = 0;
   while (i < j && *(p1+i) == *(target+i))
      i++;
   if (i == j) {
      return( (int)(p1 - string));
      }
   p1++;
   }
return (-1);

}




patternschback(string,target,current)
char *string,*target;
int current;
{

int i,j;
char *p1;

j = strlen(target);
p1 = string + current - j;

while (p1 >= string) {

   i = 0;
   while (i < j && *(p1+i) == *(target+i))
      i++;
   if (i == j) {
      return( (int)(p1 - string));
      }
   p1--;
   }
return (-1);

}


replace(string,l,search,repl,length)
char *string,*search,*repl;
int l,*length;
{

int len,len2,len3,i,j;
char *mover,*p1,*p2;

p1 =string + l;
if (search != NULL)
   p2 =p1 + strlen(search);
else 
   p2 = p1;
mover =p2;

len = strlen(string);
len2 = strlen(mover);
len3 = strlen(repl);


movmem(mover,p1 + len3,len2+2);

movmem(repl,p1,len3);

*length = strlen(string);


}


cut(string,c1,c2,length)
char *string;
int c1,c2;
int *length;
{

int len3,n1,n2,p1;

if (c1 > c2) {
p1 = c1;
c1 = c2;
c2 = p1;
}
n1 = c1;
n2 = c2;

len3 = strlen(string+n2);

movmem(string+n2,string+n1,len3+2);

*length = strlen(string);

}

