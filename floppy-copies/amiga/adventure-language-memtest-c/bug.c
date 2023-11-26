/* The Profesional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */

#include <stdio.h>
#include <ctype.h>

#define MAXRMS 50
#define MAXLIST 100
#define ITEMSPACE 40
#define MAXCODE 2000   /*maximum length of program code */
#define ITEMSINROOM 20

/* define tokens */

#define FALSE 0
#define  FIN -1
#define VERB 1
#define NOUN 2
#define ADJ 3
#define PREP 4       /* numbers less than 10 reserved for
                        tokens followed by a list */
#define IF 255       /* not an actual token */
#define IFVAREQ 10   /*if variable equals */
#define IFVARGT 11   /* numbers 10-19 reserved for IF... */
#define IFVARLT 12
#define IFRMCON 13    /*if room contains */
#define IFINCON 14    /*if inventory contains */
#define TYPE    20
#define DONE    21
#define ENDIF   22
#define ASSIGN  30        /* eg., var=10 */
struct location {
char desc[MAXRMS][80];
unsigned char north[MAXRMS];
unsigned char south[MAXRMS],east[MAXRMS],west[MAXRMS];
unsigned char up[MAXRMS],down[MAXRMS];
unsigned char item[MAXRMS][ITEMSINROOM];
};

main()
{
static struct location rm;
FILE *fp;
int i,k,rk,r,codekount,token,l,prev_rm;
char c;
char terminator;
static char code[MAXCODE];
char thing[80];
char command[80],word[40];
int varval[MAXLIST];
static char varlist[MAXLIST][ITEMSPACE];
static char nounlist[MAXLIST][ITEMSPACE];
static char verblist[MAXLIST][ITEMSPACE];
static char adjlist[MAXLIST][ITEMSPACE];
char *preplist[10];
static char typelist[MAXLIST][ITEMSPACE];
char *listlist[5];
char roomdes[80],program[80];     /* filenames */
int noun,verb,prep,adj,prepobj;

/* create list of lists */

listlist[0]=(char *)0;
listlist[1]=(char *)verblist;
listlist[2]=(char *)nounlist;
listlist[3]=(char *)adjlist;
listlist[4]=(char *)preplist;
 
/* define known prepositions */

preplist[1]="IN";
preplist[2]="ON";
preplist[3]="UNDER";
preplist[4]="WITH";
preplist[4]="BEHIND";

printf("Enter the room description filename: ");
/* scanf("%s",roomdes);

printf("Enter the adventure program filename: ");
scanf("%s",program); */

fp=fopen("roomdat.adv","r");
if (fp==NULL) {
   printf("Can't open description file\n");
   exit(10);
   }

rk=1;     /* room kounter */

loop:
r=fgetc(fp);
printf("%c",r);
if (r==EOF) goto loaded;
if (r==13 || r==10) goto loop;
if (r=='/' || r==' ') {
   getline(fp,rm.desc[rk]);
   printf("%s\n",rm.desc[rk]);
   goto loop;
   }
printf("\nungeting: %c\n",r);
ungetc(r,fp);
getline(fp,rm.desc[rk]);
printf("%s",rm.desc[rk]);
fscanf(fp,"%d,%d,%d,",&rm.north[rk],&rm.south[rk],&rm.east[rk]);
printf("n,s,e read\n");
fscanf(fp,"%d,%d,%d",&rm.west[rk],&rm.up[rk],&rm.down[rk]);
printf("read rooms\n");
thing="";
i=0;
while(thing[0]!='$') {
   fscanf(fp,"%s",thing);  /* read in an item in this room */
   if (thing[0]!='$') rm.item[rk][i++]=addlist(thing,nounlist);
   }
rk++;
goto loop;

loaded:
printf("%d Rooms read, loading program file...\n",rk-1);
if ((fp=fopen("prog.adv","r"))==0) {
   printf("Can't open program file!\n");
   exit(10);
   }

codekount=0;

while((terminator=getfrag(fp,thing))!=EOF) {
   token=gettoken(thing);
   if (token<10) {      /* <10 means token followed by a list */
      code[codekount++]=token;
      do {
         terminator=getfrag(fp,thing);   /* get list item */
         code[codekount++]=addlist(thing,listlist[token]);
         } while (terminator==',');
      code[codekount++]=0;    /* end of list marker */
      }
    else switch(token) {
      case IF: terminator=getfrag(fp,thing);
               if (thing[0]=='$' && thing[1]=='R' && thing[2]=='M') {
                  code[codekount++]=IFRMCON;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;
                  }
               if (thing[0]=='$' && thing[1]=='I' && thing[2]=='N') {
                  code[codekount++]=IFINCON;
                  getfrag(fp,thing);    /* thing becomes noun checking for */
                  code[codekount++]=addlist(thing,nounlist);
                  break;
                  }
               if (terminator=='<') code[codekount++]=IFVARLT;
               else if (terminator=='=') code[codekount++]=IFVAREQ;
               else if (terminator=='>') code[codekount++]=IFVARGT;
               else {
                     printf("Undefine comparison expression in IF.\n");
                     goto error;
                     }
               fscanf(fp,"%d",&code[codekount++]);
               printf("%d\n",code[codekount-1]);
               break;
      case TYPE: code[codekount++]=TYPE;
                 getline(fp,thing);
                 printf("%s",thing);
                 code[codekount++]=addlist(thing,typelist);
                 break;
      case ASSIGN: code[codekount++]=ASSIGN; /*must be a variable = */
                   if (isalpha(thing[0])==FALSE) {
                      printf("\nSyntax Error, variable expected\n");
                      getline(fp,thing);  /* skip rest of line */
                      break;
                      }
                   code[codekount++]=addlist(thing,varlist);
                    if (terminator!='=') {
                      printf("\nSyntax Error, '=' expected\n");
                      getline(fp,thing);
                      break;
                      }
                   c=fgetc(fp);
                   ungetc(c,fp);
                   if (isalpha(c)!=FALSE) {
                      printf("\nSyntax Error, decimal number expected.\n");
                      getline(fp,thing);  /* skip rest of line */
                      break;
                      }
                   fscanf(fp,"%d",&code[codekount++]);
                   printf("%d\n",code[codekount-1]);
                   break;
      default:     code[codekount++]=token;
                   break;

      }  /* terminates switch */
   }     /* terminates main while loop */

printf("\nPrint tables? ");
scanf("%s",thing);
if (thing[0]=='y' || thing[0]=='Y') {

printf("program code follows:\n\n");
printlist(varlist);
printlist(nounlist);
printlist(verblist);
printlist(adjlist);
printlist(preplist);
printlist(typelist);
for (i=0;i<codekount;i++)
   printf("%d %d \n",i,code[i]);
}
error:

/************************************************/
/**      The following code runs the adventure **/
/************************************************/

printf(" [H [J");    /* clear screen */
printf("Welcome to The Professional Adventure Runtime System\n\n");

l=1;
prev_rm=l;

newinput:
if (l==0) {
   printf("Sorry, I can't  go that way.");
   l=prev_rm;
   }

printf("%s\n",rm.desc[l]);
printf("I see:\n");
for(i=0;i<ITEMSINROOM;i++)
   if (rm.item[l][i]!=0) printf("%s\n",nounlist[rm.item[l][i]]);

printf("\nWhat is your command? ");
i=0;
while (scanf("%c",&command[i++])!=EOF) ;
command[i]=0;
cap(command);        /* make string all upper case */
prev_rm=l;
if (strcmp(command,"NORTH")==0) {
   l=rm.north[l];
   goto newinput;
   }
if (strcmp(command,"SOUTH")==0) {
   l=rm.south[l];
   goto newinput;
   }
if (strcmp(command,"EAST")==0) {
   l=rm.east[l];
   goto newinput;
   }
if (strcmp(command,"WEST")==0) {
   l=rm.west[l];
   goto newinput;
   }
if (strcmp(command,"UP")==0) {
   l=rm.up[l];
   goto newinput;
   }
if (strcmp(command,"DOWN")==0) {
   l=rm.down[l];
   goto newinput;
   }

/* parse the input */
i=0;
noun=0;
verb=0;
prep=0;
adj=0;
prepobj=0;

printf("com: %s\n",command);

while(command[i]!=0) {
   getword(&i,command,word);
   printf("word: %s\n",word);
   if (verb==0)
      if ((k=scanlist(word,verblist))!=0) {
         verb=k;
         goto skip;
         }

   if (noun==0)
      if ((k=scanlist(word,nounlist))!=0) {
         noun=k;
         goto skip;
         }

   if (adj==0)
      if ((k=scanlist(word,adjlist))!=0) {
         adj=k;
         goto skip;
         }

   if (prep==0)
      if ((k=scanlist(word,preplist))!=0) {
         prep=k;
         if (command[i]==0) {
            printf("Your preposition needs an object.");
            goto newinput;
            }
         getword(&i,command,word);
         if ((prepobj=scanlist(word,nounlist))==0) {
            printf("I don't understand.\n");
            goto newinput;
            }
          }
skip: ;
} /* end parser while loop */
printf("verb, noun, adj, prep, prepobj %d %d %d %d %d",verb,noun,adj,
        prep,prepobj);



printf("I don't understand\n");

goto newinput;

}   /* terminats main() */


/****************/
/**  getword   **/
/****************/

/* get a word out of string, ignore THE, A, TO */

getword(i,string,word)
char string[],word[];
int *i;
{
int k;
k=0;
printf("entered get word\n");
while(string[*i]!=13 && string[*i]!=10 && string[*i]!=' ') {
   printf("%d\n",string[*i]);
   word[k++]=string[*i];
   (*i)++;
   }
(*i)++;
word[k]=0;

if (strcmp(word,"THE")==0) getword(i,string,word);
if (strcmp(word,"A")==0) getword(i,string,word);
if (strcmp(word,"TO")==0) getword(i,string,word);

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


/*****************/
/* print a list  */
/*****************/

printlist(list)
char list[MAXLIST][ITEMSPACE];
{
int i;
i=0;
printf("\nNew list:\n");
while  (list[i++][0]!=0)
   printf("%s\n",list[i]);
}

/* read in a line until a c/r is found */

getline(fp,string)
FILE *fp;
char *string;
{
int i,k;
k=0;

while ((i=(char)fgetc(fp))!=13 && i!=10 && k++<80 && i!=EOF) {
   *string++=(char)i;
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
while(list[kount][0]!=0 && strcmp(list[kount],item)!=0) kount++;
if (list[kount][0]!=0) return(kount);
strcpy(list[kount++],item);
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
return(c);
}

/*****************************/
/**    gettoken             **/
/*****************************/


gettoken(thing)
char *thing;
{
if (strcmp(thing,"IF")==0) return(IF);
if (strcmp(thing,"TY")==0) return(TYPE);
if (strcmp(thing,"V")==0) return(VERB);
if (strcmp(thing,"N")==0) return(NOUN);
if (strcmp(thing,"A")==0) return(ADJ);
if (strcmp(thing,"P")==0) return(PREP);
if (strcmp(thing,"DONE")==0) return (DONE);
if (strcmp(thing,"ENDIF")==0) return (ENDIF);
return(ASSIGN);
}

