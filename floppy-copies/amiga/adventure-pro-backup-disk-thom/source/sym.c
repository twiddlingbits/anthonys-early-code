/* The Professional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */
/* (c) 1987 Thom Robertson */

#include <exec/types.h>
#include <graphics/gfx.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include "adventure.h"

char command[80],word[80];
struct loc2 rm;
char varlist[MAXLIST][ITEMSPACE];
char nounlist[MAXLIST][ITEMSPACE];
int noun_pics[MAXLIST];
char verblist[MAXLIST][ITEMSPACE];
char adjlist[MAXLIST][ITEMSPACE];
char extlist[MAXLIST][ITEMSPACE];
char ext2list[MAXRMS][ITEMSPACE];
char ext3list[MAXLIST][ITEMSPACE];
char prepolist[MAXLIST][ITEMSPACE];
char typelist[MAXLIST][TYPESPACE];
char terminator;
UBYTE code[MAXCODE];
char thing[400],thing2[400];
int codekount;
main()
{
FILE *fp;
int i,k,rk,rk2,r,token,rnum,nnum,*ll,lxx;
int l,lyy,xx1,xx2,xx3,spckeys,secnum,secnum2;
int mcnum,xnum,ynum;
int eeenum;
int keys[3][20];
char c;
int gtype,mainnum,backnum,varval[MAXLIST];
int screenw,screenh,numplanes,picx,picy;
int winx,winy,winw,winh,objnum[2];
int objx[2][10],objy[2][10];
static struct sequence sequences[MAXLIST],*s;

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

printf("The Adventure-Pro Symbolizer is now operational. ");

fp=fopen("roomdat.adv","r");
if (fp==NULL) {
   printf("Can't open description file\n");
   exit(10);
   }
/***** initialize  generic nouns ********/
       nnum = addlist("NOUN",nounlist);
       mcnum = addlist("MOUSECLICK",verblist);

/***** initialize  generic variables ********/
       rnum = addlist("ROOM",varlist);
       xnum = addlist("MOUSEX",varlist);
       ynum = addlist("MOUSEY",varlist);
       eeenum = addlist("ERROR",varlist);

rk=1;     /* room kounter */

loop:
r=fgetc(fp);
if (r==EOF) goto loaded;
if (r==13 || r==10) goto loop;
if (r=='/' || r==' ') {
   getline(fp,thing);
   goto loop;
   }
ungetc(r,fp);
getline(fp,thing);
rm.desc[rk]=addtype(thing,typelist);
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
                     dataput2();
                     code[codekount++]=0;
                     }
                  else {
                     codeput1();
                     }
                  }
               else if (terminator=='=') {
                  code[codekount++]=IFVAREQ;
                   code[codekount++]=addlist(thing,varlist);
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     code[codekount++]=0;
                     }
                  else {
                     codeput1();
                     }
                   }
               else if (terminator=='>') {
                  code[codekount++]=IFVARGT;
                  code[codekount++]=addlist(thing,varlist);
                  terminator=getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     code[codekount++]=0;
                     }
                  else {
                     codeput1();
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
                 i=addtype(thing,typelist);
                 code[codekount++]= i & 255;
                 i = i>>8;
                 code[codekount++]= i & 255;
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
                     dataput2();
                     code[codekount++]=0;
                     }
                  else {
                     codeput1();
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
                     dataput2();
                     code[codekount++]=0;
                     }
                  else {
                     codeput1();
                     }
                     if (terminator ==0 || terminator == 13 || terminator == 10)
                        break;
                   }    /* while end  */
                   code[codekount++] = 0;

                   break;

      case TOROOM:   code[codekount++]=TOROOM;
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     dataput3();
                     }
                  break;
      case INJECT:   code[codekount++]=INJECT;
                  getline(fp,thing);
                  code[codekount++]=addtype(thing,typelist);
                  break;

      case CRMG:   code[codekount++]=CRMG;
                  getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++] = l;
                  getline(fp,thing);
                  code[codekount++]=addlist(thing,ext2list);
                  break;

      case CRMS:  code[codekount++]=CRMS;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  break;

      case WAIT:  code[codekount++]=WAIT;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  break;

      case COBJG:   code[codekount++]=COBJG;
                  terminator= getfrag2(fp,thing);
                  code[codekount++]=addlist(thing,nounlist);
                  getline(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     dataput3();
                     }
                  break;

      case DG:   code[codekount++]=DG;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     codeput1();
                     }
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     codeput1();
                     }
                  break;

      case DS:   code[codekount++]=DS;
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     dataput3();
                     }
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     codeput1();
                     }
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     codeput1();
                     }
                  break;

      case ACTS:   code[codekount++]=ACTS;
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     dataput3();
                     }
                  break;

      case ENDS:   code[codekount++]=ENDS;
                  terminator= getfrag2(fp,thing);
                  if (isalpha(thing[0]))  {
                     dataput2();
                     }
                  else {
                     dataput3();
                     }
                  break;

      case LOADS:   code[codekount++]=LOADS;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  break;

      case STOPS:   code[codekount++]=STOPS;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  break;

      case CLEARS:   code[codekount++]=CLEARS;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  break;

      case PLAYS:   code[codekount++]=PLAYS;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  break;

      case OPEN:   code[codekount++]=OPEN;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  terminator= getfrag2(fp,thing);
                  if (thing[0] == 'R')
                     code[codekount++]=0;
                  else
                     code[codekount++]=1;
                  break;

      case READ:   code[codekount++]=READ;
                  terminator= getfrag2(fp,thing);
                  code[codekount++]=addlist(thing,varlist);
                  break;

      case WRITE:   code[codekount++]=WRITE;
                  terminator= getfrag2(fp,thing);
                  code[codekount++]=addlist(thing,varlist);
                  break;

      case STOREG:   code[codekount++]=STOREG;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  break;

      case LOADG:   code[codekount++]=LOADG;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  break;

      case SLAVE:   code[codekount++]=SLAVE;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  terminator= getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
                  break;

      case UNSLAVE:   code[codekount++]=UNSLAVE;
                  fscanf(fp,"%s ",&thing[0]);
                  code[codekount++]=addlist(thing,extlist);
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

      case RND:   code[codekount++]=RND;
                  getfrag(fp,thing);
                  code[codekount++]=addlist(thing,varlist);
                  getfrag2(fp,thing);
                  stcd_i (thing,ll);
                  code[codekount++]= l;
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
   addlist(thing,ext3list);
   fscanf(fp,"%d,%d,",&picx,&picy);
   i = 1;
   while (i < rk) {
      fscanf(fp,"%s",&thing[0]);
      rm.pic[i]=addlist(thing,ext2list);
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
      lxx = addlist(thing,nounlist);
      if ((xx1 = stcd_i (thing2,ll)) == 0) {
         l = addlist(thing2,ext3list);
         noun_pics[lxx] = l;
         }
      else
         noun_pics[lxx] = l * -1;
      i--;
      }

   fscanf(fp,"%d,",&i);
   spckeys = i;
   i = 0;
   while(i < spckeys) {
      fscanf(fp,"%d,%d,",&keys[0][i],&keys[1][i]);
      getline(fp,thing);
      l = addlist(thing,extlist);
      keys[2][i] = l;
      i++;
      }

   fclose(fp);

   fp=fopen("sequences.adv","r");
   if (fp==NULL) {
      printf("Can't open sequence data file\n");
      exit(10);
      }
   fscanf(fp,"%d,",&secnum);
   if (secnum == 0)
      goto xxsemester;
   i = 0;
   while(i < secnum) {
      s = &sequences[i];
      fscanf(fp,"%d,%d,%d,",&xx1,&xx2,&xx3);
      s->speed = (char) xx1;
      s->number = (char) xx2;
      s->repeat = (char) xx3;
      fscanf(fp,"%d,%d,",&xx1,&xx2);
      s->room = (char) xx1;
      s->flags = xx2;
      l = 0;
      while (l <s->number) {
      fscanf(fp,"%d,%d,%d,",&(sequences[i].sec[l]),
                            &(sequences[i].x[l]),&(sequences[i].y[l]));
         l++;
         }
      l = 0;
      fscanf(fp,"%d,",&xx1);
      s->framenum = (char) xx1;
      while (l <s->framenum) {
         fscanf(fp,"%s ",&thing2[0]);
         lxx = strlen(&thing2[0]);
         s->names[l] = AllocMem(lxx+1,0);
         strcpy(s->names[l],&thing2[0]);
         l++;
         }
      i++;
      }
   fclose(fp);
   }

xxsemester:
printf("\nPrint tables? ");
scanf("%s",thing);
if (thing[0]=='y' || thing[0]=='Y') {

printf("program code follows:\n\n");
printlist(varlist);
printlist(nounlist);
printlist(verblist);
printlist(adjlist);
printlist(prepolist);
printlist2(typelist);
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
fprintf(fp,"%d,",mcnum);
fprintf(fp,"%d,",xnum);
fprintf(fp,"%d,",ynum);
fprintf(fp,"%d,",eeenum);
fprintf(fp,"%d,",rk);
fprintf(fp,"%d,",mainnum);
fprintf(fp,"%d,",backnum);
loopa:
fprintf(fp,"%d,",rm.desc[rk2]);
fprintf(fp,"%d,",rm.pic[rk2]);
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
filelist2(fp,typelist);
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
   
   fprintf(fp,"%d,",spckeys);
   i = 0;
   while(i < spckeys) {
      fprintf(fp,"%d,%d,%d,",keys[0][i],keys[1][i],keys[2][i]);
      i++;
      }

fprintf(fp,"%d\n",secnum);
if (secnum > 0)  {
   i = 0;
   while(i < secnum) {
      s = &sequences[i];
      fprintf(fp,"%d,%d,%d,",s->speed,s->number,s->repeat);
      fprintf(fp,"%d,%d,",s->room,s->flags);
      l = 0;
      while (l <s->number) {
      fprintf(fp,"%d,%d,%d,",s->sec[l],s->x[l],s->y[l]);
         l++;
         }
      l = 0;
      fprintf(fp,"%d,",s->framenum);
      while (l <s->framenum) {
         fprintf(fp,"%s\n",s->names[l]);
         l++;
         }
      i++;
      }
   }

}

fprintf(fp,"%d,",codekount);
for (i=0;i <= codekount;i++)
   fprintf(fp,"%d,",code[i]);


fclose(fp);


   i = 0;
   while(i < secnum) {
      s = &sequences[i];
      l = 0;
      while (l <s->framenum)
         FreeMem(s->names[l],strlen(s->names[l++])+1);
      i++;
      }

printf("tokenization done.\n\n\n");


}


