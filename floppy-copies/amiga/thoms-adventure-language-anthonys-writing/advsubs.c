
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"


/****************/
/**  getword   **/
/****************/

/* get a word out of string, ignore THE, A, TO */

getword(i)
int i;
{
int k;
extern char command[80],word[40];
k=0;
while(command[i]!=0  && command[i]!=' ' && k<40) {
   word[k++]=command[i];
   i++;
   }
i++;
word[k]=0;

if (strcmp(word,"THE")==0) i = getword(i);
if (strcmp(word,"A")==0) i = getword(i);
if (strcmp(word,"TO")==0) i = getword(i);

return(i);
}


/******************/
/**  scanlist    **/
/******************/

scanlist(item,list)

char *item;
char list[MAXLIST][ITEMSPACE];
{
int i;
i=1;
while (list[i][0]!=0)
   if (strcmp(list[i++],item)==0) return(i-1);
return(0);
}



/****************/
/**  cap       **/
/****************/
cap(string)
char string[];
{
int i;
i=0;
while (string[i]!=0) {
   string[i]=toupper(string[i]);
   i++;
   }
}

/* read in a line until a c/r is found */

getline(fp,string)
FILE *fp;
char *string;
{

int k,l;
char i;
k=0;
while ((i=fgetc(fp))!=13 && i!=10 && k++<500 && i!=EOF) {
   *string++=i;
   }
*string=0;

}

/********************************************************/
/* addlist--add a word to a list and return item number */
/********************************************************/

addlist(item,list)
char *item;
char list[MAXLIST][ITEMSPACE];
{
int kount;
kount=1;
while(list[kount][0]!=0 && strcmp(list[kount],item)!=0) {
   kount++;
   if (kount > MAXLIST) {
      printf("OVERFLOW! -- out of room in list arrays.");
      exit(20);
      }
   }
if (list[kount][0]!=0) return(kount);
strcpy(list[kount],item);
return(kount);
}

/*****************************/
/**       getfrag           **/
/*****************************/
/* gets an ascii string intill a non-apha character */
getfrag(fp,thing)
FILE *fp;
char *thing;
{
char c,d;
/* first skip leading spaces or c/r */
top:
while((c=fgetc(fp))==' ' || c==13 || c==10) printf("%c",c);
ungetc(c,fp);
top2:
if ((c=fgetc(fp))==EOF) return(EOF);
c=toupper(c);
if (c=='/') {   /* is this a comment? */
   d=fgetc(fp);
   if (d=='/') {    /* skip comments */
      again:
      while (fgetc(fp)!='/') ;
      if (fgetc(fp)!='/') goto again;
      goto top;
      }
   else ungetc(d,fp);
   }
printf("%c",c);
if (isalpha(c)) {
   *thing++=c;
   goto top2;
   }
*thing=0;     /* string terminator */
return((int)c);
}


/* gets an ascii string until a space */
getfrag2(fp,thing)
FILE *fp;
char *thing;
{
char c,d;
/* first skip leading spaces or c/r */
top:
while((c=fgetc(fp))==' ') printf("%c",c);
ungetc(c,fp);
if((c=fgetc(fp))==13 || c==10) return (0);
ungetc(c,fp);
top2:
if ((c=fgetc(fp))==EOF) return(EOF);
c=toupper(c);
printf("%c",c);
if (c != ' ' && c != 13 & c != 10) {
   *thing++=c;
   goto top2;
   }
return((int)c);
}


/******** add_r_i : adds noun b to room a ************/
add_r_i(a,b)
int a;
char b;  {
   int i;
   extern struct location rm;
   i = 1;
   while (rm.item[a][i] > 0 && i < MAXRMS)
      i++;
   rm.item[a][i] = b;

}


/******** sub_r_i : subtracts noun b from room a ************/
sub_r_i(a,b)
int a;
char b;  {
   int i;
   extern struct location rm;
   i = 1;
   while (rm.item[a][i] != b  && i < MAXRMS)
      i++;
   rm.item[a][i] = 0;

}



