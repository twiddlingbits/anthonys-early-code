/* The Professional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */
/* (c) 1987 Thom Robertson */

#include "exec/types.h"
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"

char command[80],word[80];
struct location rm;

main()
{
FILE *fp;
int i,k,rk,rk2,r,codekount,token,rnum,nnum,*ll,l,lxx;
int lyy;
char c,cn[MAXLIST];
char terminator;
static char code[MAXCODE];
char thing[200],thing2[200];
int gtype,mainnum,backnum,varval[MAXLIST];
int screenw,screenh,numplanes,picx,picy;
int winx,winy,winw,winh,objnum[2];
int objx[2][10],objy[2][10];

static char varlist[MAXLIST][ITEMSPACE];
static char nounlist[MAXLIST][ITEMSPACE];
static int noun_pics[MAXLIST];
static char verblist[MAXLIST][ITEMSPACE];
static char adjlist[MAXLIST][ITEMSPACE];
static char extlist[MAXLIST][ITEMSPACE];
static char ext2list[MAXRMS][ITEMSPACE];
static char ext3list[MAXLIST][ITEMSPACE];
static char prepolist[MAXLIST][ITEMSPACE];
static char typelist[MAXLIST][ITEMSPACE];
char *listlist[7];
char roomdes[80],program[80];     /* filenames */

/* create list of lists */

listlist[0]=(char *)0;
listlist[1]=(char *)verblist;
listlist[2]=(char *)nounlist;
listlist[3]=(char *)adjlist;
listlist[4]=(char *)prepolist;
listlist[5]=(char *)extlist;
listlist[6]=(char *)ext2list;
 
ll = &l;
gtype = TEXT;

printf("Enter the room description filename: ");
/* scanf("%s",roomdes);

printf("Enter the adventure program filename: ");
scanf("%s",program); */

fp=fopen("roomdat.adv","r");
if (fp==NULL) {
   printf("Can't open description file\n");
   exit(10);
   }
/***** initialize 'NOUN' generic noun ********/
       nnum = addlist("NOUN",nounlist);

/***** initialize 'ROOM' generic variable ********/
       rnum = addlist("ROOM",varlist);

rk=1;     /* room kounter */

loop:
r=fgetc(fp);
if (r==EOF) goto loaded;
if (r==13 || r==10) goto loop;
if (r=='/' || r==' ') {
   getline(fp,&rm.desc[rk][0]);
   goto loop;
   }
ungetc(r,fp);
getline(fp,&rm.desc[rk][0]);
fscanf(fp,"%d,%d,%d",&rm.n[rk],&rm.s[rk],&rm.e[rk]);
fscanf(fp,"%d,%d,%d",&rm.w[rk],&rm.u[rk],&rm.d[rk]);

printf("\n%d,%d,%d,",rm.n[rk],rm.s[rk],rm.e[rk]);
printf("%d,%d,%d",rm.w[rk],rm.u[rk],rm.d[rk]);

thing[0] = 0;
i=0;
while(i < MAXRMS)  {
   rm.item[rk][i++] = 0;  }

i = 0;
while(thing[0]!='$') {
   fscanf(fp,"%s",thing);  /* read in an item in this room */
   cap(thing);
   if (thing[0]!='$') rm.item[rk][i++]=addlist(thing,nounlist);
   }
rk++;
goto loop;

loaded:
fclose(fp);

printf("%d Rooms read, loading program file...\n",rk-1);
if ((fp=fopen("prog.adv","r"))==0) {
   printf("Can't open program file!\n");
   exit(10);
   }

codekount=0;
mainnum = 0;
backnum = 0;

while((terminator=getfrag(fp,thing))!=EOF) {
   token=gettoken(thing,terminator);
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
               if (thing[0]=='R' && thing[1]=='M' && terminator=='$') {
                  code[codekount++]=IFRMCON;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;
                  }
               if (thing[0]=='I' && thing[1]=='N' && terminator=='$') {
                  code[codekount++]=IFINCON;
                  getfrag(fp,thing);    /* thing becomes noun checking for */
                  code[codekount++]=addlist(thing,nounlist);
                  break;
                  }
               if (terminator=='<') {
                  code[codekount++]=IFVARLT;
                  code[codekount++]=addlist(thing,varlist);
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     }
                  }
               else if (terminator=='=') {
                  code[codekount++]=IFVAREQ;
                   code[codekount++]=addlist(thing,varlist);
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++] = l;
                     }
                   }
               else if (terminator=='>') {
                  code[codekount++]=IFVARGT;
                  code[codekount++]=addlist(thing,varlist);
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     }
                  }
               else {
                     printf("Undefine comparison expression in IF.\n");
                     goto error;
                     }
               break;
      case TYPE: code[codekount++]=TYPE;
                 i=0;
                 while (i<201)
                    thing[i++]=0;
                 getline(fp,thing);
                 printf("%s\n",thing);
                 l = 0;
                 i = strlen( thing ) -1;
                 if (thing[i] == '^') {
                    thing[i] = 0;
                    l = 1;
                    }
                 code[codekount++]=addlist(thing,typelist);
                 code[codekount++]=l;
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
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     }
                  if (terminator ==0 || terminator == 13 || terminator == 10) {
                     code[codekount++]= 0;
                     break;
                     }

                  while(i == i)  {
                     terminator=getfrag2(fp,thing);
                     if (terminator ==0 || terminator == 13 || terminator == 10)
                        break;
                     if (thing[0] == '+' || thing[1] == '+')
                        code[codekount++]= 1 ;
                     else if (thing[0] == '-' || thing[1] == '-')
                        code[codekount++]= 2 ;
                     else if (thing[0] == '*' || thing[1] == '*')
                        code[codekount++]= 3 ;
                     else if (thing[0] == '/' || thing[1] == '/')
                        code[codekount++]= 4 ;
                     else  {
                     printf("invalid operator!!");
                     exit(FALSE);
                     }
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     }
                     if (terminator ==0 || terminator == 13 || terminator == 10)
                        break;
                   }    /* while end  */
                   code[codekount++] = 0;

                   break;

      case TOROOM:   code[codekount++]=TOROOM;
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++]=VARIABLE;
                     code[codekount++]=addlist(thing,varlist);
                     }
                  else {
                     code[codekount++]=NUMBER;
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     }
                  break;

      case RMPLUS:   code[codekount++]=RMPLUS;
                  getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     code[codekount++] = 0;
                     code[codekount++]=addlist(thing,nounlist);
                     }
                  else  {
                     stcd_i (thing,ll);
                     code[codekount++] = l;
                     getfrag(fp,thing);
                     code[codekount++]=addlist(thing,nounlist);
                     }
                  break;

      case RMMINUS:   code[codekount++]=RMMINUS;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;

      case INPLUS:   code[codekount++]=INPLUS;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;

      case INMINUS:   code[codekount++]=INMINUS;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;

      case DESTROY:   code[codekount++]=DESTROY;
                  getfrag(fp,thing);          /* thing becomes a noun */
                  code[codekount++]=addlist(thing,nounlist);
                  break;

      case NUMTYPE:   code[codekount++]=NUMTYPE;
                  getfrag(fp,thing);          /* thing becomes a variable */
                  code[codekount++]=addlist(thing,varlist);
                  break;

      case CONNECT:   code[codekount++]=CONNECT;
                  i = 1;
                  while (i < 3)  {
                     getfrag2(fp,thing);
                     stcd_i (thing,ll);
                     code[codekount++]= l;
                     if (l <0 || l >MAXRMS)  {
                        printf("bad room number!");
                        exit(FALSE);
                        }
                     getfrag2(fp,thing);
                     if (thing[0] == 'N')
                        code[codekount++] = 1;
                     else if (thing[0] == 'S')
                        code[codekount++] = 2;
                     else if (thing[0] == 'E')
                        code[codekount++] = 3;
                     else if (thing[0] == 'W')
                        code[codekount++] = 4;
                     else if (thing[0] == 'U')
                        code[codekount++] = 5;
                     else if (thing[0] == 'D')
                        code[codekount++] = 6;
                     else {
                        printf("<%s>\n",thing);
                        printf("bad direction!!");
                        exit(FALSE);
                        }
                     i++;
                     }
                   break;

      case MAIN:
         mainnum = codekount;
         break;

      case BACKGROUND:
         backnum = codekount;
         break;

      case GRAPHIC:
         gtype = GRAPHIC;
         break;

      default:     code[codekount++]=token;
                   break;

      }  /* terminates switch */
   }     /* terminates main while loop */

if (mainnum == 0) {
   printf("No MAIN specified!");
   exit(0);
   }

if (backnum == 0) {
   printf("No BACKGROUND specified!");
   exit(0);
   }

fclose(fp);

if (gtype == GRAPHIC) {
   /***********************load graphical data********************/
   fp=fopen("graphicdat.adv","r");
   if (fp==NULL) {
      printf("Can't open graphic data file\n");
      exit(10);
      }
   fscanf(fp,"%d,%d,",&screenw,&screenh);
   fscanf(fp,"%d,",&numplanes);
   fscanf(fp,"%s",&thing[0]);
   addlist(thing,ext2list);
   fscanf(fp,"%d,%d,",&picx,&picy);
   i = 1;
   while (i < rk) {
      fscanf(fp,"%s",&thing[0]);
      addlist(thing,ext2list);
      i++;
      }
   fscanf(fp,"%d,%d,",&winx,&winy);
   fscanf(fp,"%d,%d,",&winw,&winh);

   fscanf(fp,"%d,",&i);
   objnum[0] = i;
   while(i > 0) {
      fscanf(fp,"%d,%d,",&objx[0][i],&objy[0][i]);
      i--;
      }

   fscanf(fp,"%d,",&i);
   objnum[1] = i;
   while(i > 0) {
      fscanf(fp,"%d,%d,",&objx[1][i],&objy[1][i]);
      i--;
      }

   fscanf(fp,"%d,",&i);
   lyy = i;
   while(i > 0) {
      fscanf(fp,"%s ",&thing[0]);
      fscanf(fp,"%s ",&thing2[0]);
      l = addlist(thing2,ext3list);
      lxx = addlist(thing,nounlist);
      noun_pics[lxx] = l;
      i--;
      }


   fclose(fp);
   }


printf("\nPrint tables? ");
scanf("%s",thing);
if (thing[0]=='y' || thing[0]=='Y') {

printf("program code follows:\n\n");
printlist(varlist);
printlist(nounlist);
printlist(verblist);
printlist(adjlist);
printlist(prepolist);
printlist(typelist);
printlist(extlist);
printlist(ext2list);
printlist(ext3list);
for (i=0;i<codekount;i++)
   printf("%d %d \n",i,code[i]);
}
error:

fp = fopen("symbol.adv","w");


rk2=1;     /* room kounter */
fprintf(fp,"%d,",nnum);
fprintf(fp,"%d,",rnum);
fprintf(fp,"%d,",rk);
fprintf(fp,"%d,",mainnum);
fprintf(fp,"%d,",backnum);
loopa:
fprintf(fp,"%s\n",rm.desc[rk2]);
fprintf(fp,"%d,%d,%d,",rm.n[rk2],rm.s[rk2],rm.e[rk2]);
fprintf(fp,"%d,%d,%d,",rm.w[rk2],rm.u[rk2],rm.d[rk2]);

i = 0;
while(rm.item[rk2][i] != 0)
   fprintf(fp,"%d,",rm.item[rk2][i++]);
fprintf(fp,"%d,",rm.item[rk2][i]);

rk2++;
if (rk2 < rk)
   goto loopa;


filelist(fp,nounlist);
filelist(fp,verblist);
filelist(fp,adjlist);
filelist(fp,prepolist);
filelist(fp,typelist);
filelist(fp,extlist);
filelist(fp,ext2list);
filelist(fp,ext3list);

if (gtype == GRAPHIC) {
fprintf(fp,"%d,%d,",screenw,screenh);
fprintf(fp,"%d,",numplanes);
fprintf(fp,"%d,%d,",picx,picy);
fprintf(fp,"%d,%d,",winx,winy);
fprintf(fp,"%d,%d,",winw,winh);

fprintf(fp,"%d,",objnum[0]);
   i = objnum[0];
   while(i > 0) {
      fprintf(fp,"%d,%d,",objx[0][i],objy[0][i]);
      i--;
      }

fprintf(fp,"%d,",objnum[1]);
   i = objnum[1];
   while(i > 0) {
      fprintf(fp,"%d,%d,",objx[1][i],objy[1][i]);
      i--;
      }


i = 0;
while(i++ <MAXLIST)
   fprintf(fp,"%d,",noun_pics[i]);
   


}

fprintf(fp,"%d,",codekount);
for (i=0;i <= codekount;i++)
   fprintf(fp,"%d,",code[i]);


fclose(fp);
printf("tokenization done.\n\n\n");


}


