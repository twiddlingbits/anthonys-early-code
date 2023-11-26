/* The Professional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */
/* (c) 1987 Thom Robertson */

#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"

char command[80],word[80];
struct location rm;

main()
{
FILE *fp;
int i,k,rk,rk2,r,codekount,token,prev_rm,nnum;
char c,cn[MAXLIST];
char terminator;
static char code[MAXCODE];
char thing[200];
int varval[MAXLIST];
static char *varlist[MAXLIST];
static char *nounlist[MAXLIST];
static char *verblist[MAXLIST];
static char *adjlist[MAXLIST];
char *preplist[5]; 
static char *prepolist[MAXLIST];
static char *typelist[MAXLIST];
char *listlist[5];
char roomdes[80],program[80];     /* filenames */
int noun,verb,prep,adj,prepobj,l,*ll;
int comcase,comlen,comif;
ll = &l;

/* create list of lists */

listlist[0]=0;
listlist[1]=verblist;
listlist[2]=nounlist;
listlist[3]=adjlist;
listlist[4]=prepolist;
 
/* define known prepositions */

preplist[0]="IN";
preplist[1]="ON";
preplist[2]="UNDER";
preplist[3]="WITH";
preplist[4]="BEHIND";

fp = fopen("symbol.adv","r");
fscanf(fp,"%d",nnum);
fscanf(fp,"%d",rk);
rk2 = 1;
do {
   memstring(fp,rm.desc[rk2]);
   fscanf(fp,"%d,%d,%d,%d,%d,%d",rm.n[rk2],rm.s[rk2],rm.e[rk2],rm.w[rk2],rm.u[rk2],rm.d[rk2]);
   i = 0;
   while (i<MAXRMS)
      rm.item[rk2][i++] = 0;
   i = 0;
   do {
      fscanf(fp,"%d",rm.item[rk2][i]);
      } while(rm.item[rk2][i++] > 0);
   rk2++;
   } while(rk2<rk);






/*  clear inventory   */
i = 0;
do {
  rm.item[0][i] = 0;
  i++;
  } while(i <= MAXRMS);

/*  clear variables   */
i = 0;
do {
  varval[i] = 0;
  i++;
  } while(i <= MAXLIST);


/************************************************/
/**      The following code runs the adventure **/
/************************************************/

printf("\n\n");    /* clear screen */
printf("Welcome to The Professional Adventure Runtime System\n\n");

l=1;
prev_rm=l;

newinput:
if (l<=0) {
   printf("Sorry, I can't  go that way.\n");
   l=prev_rm;
   }

printf("-------------------------------------------------------\n");
printf("%s\n",&rm.desc[l][0]);
printf("Obvious exits are: ");
if (rm.n[l] > 0)
   printf("North ");
if (rm.s[l] > 0)
   printf("South ");
if (rm.e[l] > 0)
   printf("East ");
if (rm.w[l] > 0)
   printf("West ");
if (rm.u[l] > 0)
   printf("Up ");
if (rm.d[l] > 0)
   printf("Down ");
printf("\n");

printf("You see:\n");
k = 0;
for(i=0;i<ITEMSINROOM;i++)
   if (rm.item[l][i]!=0) {
      printf("%s\n",nounlist[rm.item[l][i]]);
      k++;
      }
   if (k == 0)
      printf("Nothing.\n");

printf("-------------------------------------------------------\n\n");

printf("\nWhat is your command? ");
do {
   i = 0;
   do {
      scanf ("%c",&command[ i ]);
      i++;
      } while (command[ i-1 ]!='\n');
   command[i-1]=0;
} while (command[0] == 0) ;


cap(command);        /* make string all upper case */
prev_rm = l;

if (strcmp(command,"QUIT")==0) {
   printf("Exiting the Adventure system.\n\n");
   exit(0);
   }
if (strcmp(command,"NORTH")==0||strcmp(command,"N")==0) {
   l=rm.n[l];
   goto newinput;
   }
if (strcmp(command,"SOUTH")==0||strcmp(command,"S")==0) {
   l=rm.s[l];
   goto newinput;
   }
if (strcmp(command,"EAST")==0||strcmp(command,"E")==0) {
   l=rm.e[l];
   goto newinput;
   }
if (strcmp(command,"WEST")==0||strcmp(command,"W")==0) {
   l=rm.w[l];
   goto newinput;
   }
if (strcmp(command,"UP")==0||strcmp(command,"U")==0) {
   l=rm.u[l];
   goto newinput;
   }
if (strcmp(command,"DOWN")==0||strcmp(command,"D")==0) {
   l=rm.d[l];
   goto newinput;
   }

if (strcmp(command,"INVENTORY")==0||strcmp(command,"INV")==0) {
   printf("\nYou are carrying:\n");
   if (rm.item[0][0] == 0) {
      printf("Nothing.\n\n");
      goto newinput;
      }
      i = 1;
      while( i <= MAXRMS) {
      if (rm.item[0][i] != 0) 
         printf("   %s\n",nounlist[rm.item[0][i]]);
      i++;
         }
   goto newinput;
   }

/* parse the input */
i = 0;
rk = 0;
noun = 0;
verb = 0;
prep = 0;
adj = 0;
prepobj = 0;

while (command[ i ]!= 0) {
   i = getword(i);
   if (verb == 0)
      if ((k = scanlist( word,verblist) )!= 0) {
         verb = k;
         goto skip;
         }

   if (noun == 0)
      if ((k = scanlist(word,nounlist) )!= 0) {
         noun = k;
         goto skip;
         }

   if (adj == 0)
      if ((k = scanlist(word,adjlist) )!= 0) {
         adj = k;
         goto skip;
         }

   if (prep == 0)
      if ((k = scanlist(word,prepolist) )!= 0) {
         prep = k;
         if (command [ i ] == 0) {
            printf("Your preposition needs an object.");
            goto newinput;
            }
         i = getword (i);
         if ((prepobj = scanlist( word,nounlist )) == 0) {
            printf("I don't understand.\n");
            goto newinput;
            }
          }
skip: ;
} /* end parser while loop */


/*   interpreter     */

rk = 0;
comcase = 0;
ijump:

switch (code[rk]) {
 
   case VERB:
      comcase = 1;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == verb)
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;

   case NOUN:
    if (comcase != 1)
      comcase = 2;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == noun && comcase != 1)
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;

   case ADJ:
    if (comcase != 1 && comcase != 2)
      comcase = 3;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == adj && comcase != 1 && comcase != 2)
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;

   case TYPE:
      if (comcase == 0) {
         printf("%s",typelist[code[rk + 1]]);
         if (code[rk + 2] == 0)
            printf("\n");
         }
      comlen = 3;
      break;

   case NUMTYPE:
      if (comcase == 0)
         printf("%d",varval[code[rk + 1]]);
      comlen = 2;
      break;

   case DONE:
      if (comcase == 0)
         comcase = -1;
      comlen = 1;
      break;

   case ADJEND:
      if (comcase < 1 || comcase > 2)
      comcase = 0;
      comlen = 1;
      break;

   case NEND:
      if (comcase != 1)
      comcase = 0;
      comlen = 1;
      break;

   case ENDIF:
      if (comcase == 5) {
         if (comif == 0)
            comcase = 0;
         comif--;
         }
      comlen = 1;
      break;


   case RMPLUS:
    if (comcase == 0) {
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      add_r_i(l,code[rk + 1]);
      }
      comlen = 2;
      break;


   case RMMINUS:
    if (comcase == 0) {
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      sub_r_i(l,code[rk + 1]);
      }
      comlen = 2;
      break;


   case INPLUS:
    if (comcase == 0) {
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      add_r_i(0,code[rk + 1]);
      rm.item[0][0]++;
      }
      comlen = 2;
      break;


   case INMINUS:
    if (comcase == 0) {
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      if (rm.item[0][0] > 0) {
         sub_r_i(0,code[rk + 1]);
         rm.item[0][0]--;
         }
      }
      comlen = 2;
      break;


   case IFRMCON:
    i = 0;
    if (comcase == 5)
       comif++;
    if (comcase == 0) {
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      while (rm.item[l][i] != code[rk + 1] && i <MAXRMS +1)
         i++;
      if (i == MAXRMS + 1) {
         comif = 0;
         comcase = 5;
         }
      }
      comlen = 2;
      break;

 
   case IFINCON:
    if (comcase == 5)
       comif++;
    if (comcase == 0) {
      i = 0;
      if (code[rk + 1] == nnum)
         code[rk + 1] = noun;
      while (rm.item[0][i] != code[rk + 1] && i <MAXRMS +1)
         i++;
      if (i == MAXRMS + 1) {
         comif = 0;
         comcase = 5;
         }
      }
      comlen = 2;
      break;


   case ASSIGN:
    i = 4;
    if (comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         varval[code[rk + 1]] = varval[code[rk + 3]];
      else
         varval[code[rk + 1]] = code[rk + 3];

      while (code[rk + i] != 0) {
         if (code[rk + i] <1 || code[rk + i] >4) {
            printf("bad symbolized operator!!");
            exit(FALSE);
            }
         if (code[rk + i + 1] == VARIABLE)
            r = varval[code[rk + i + 2]];
         else
            r = code[rk + i + 2];
         if (code[rk + i] == 1)
            varval[code[rk + 1]] = varval[code[rk + 1]] + r;
         if (code[rk + i] == 2)
            varval[code[rk + 1]] = varval[code[rk + 1]] - r;
         if (code[rk + i] == 3)
            varval[code[rk + 1]] = varval[code[rk + 1]] * r;
         if (code[rk + i] == 4)
            varval[code[rk + 1]] = varval[code[rk + 1]] / r;
         i = i + 3;
         }
       }
       else {
          while (code[rk+i] != 0)
             i = i + 3;
          }
      comlen = i+1;
      break; /* assign end  */ 

   case IFVARLT:
    if (comcase == 5)
       comif++;
      if (comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3];
      if (varval[code[rk + 1]] >= i) {
         comif = 0;
         comcase = 5;
         }
      }
      comlen = 4;
      break;


   case IFVAREQ:
    if (comcase == 5)
       comif++;
      if (comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3];
      if (varval[code[rk + 1]] != i) {
         comif = 0;
         comcase = 5;
         }
      }
      comlen = 4;
      break;


   case IFVARGT:
    if (comcase == 5)
       comif++;
      if (comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3];
      if (varval[code[rk + 1]] <= i) {
         comif = 0;
         comcase = 5;
         }
      }
      comlen = 4;
      break;


   case CONNECT:
      if (comcase == 0) {
      if (code[rk + 2] == 1)
         rm.n[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 2)
         rm.s[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 3)
         rm.e[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 4)
         rm.w[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 5)
         rm.u[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 6)
         rm.d[code[rk + 1]] = code[rk + 3];

      if (code[rk + 4] == 1)
         rm.n[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 2)
         rm.s[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 3)
         rm.e[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 4)
         rm.w[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 5)
         rm.u[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 6)
         rm.d[code[rk + 3]] = code[rk + 1];
      }
      comlen = 5;
      break;

  default:    
      printf("bad command: line %d!\n\n\n",rk);
      printf(" %d %d %d\n",code[rk],code[rk+1],code[rk+2] );
      exit(FALSE);

  }  /*  switch end */

rk = rk + comlen;
if (comcase == -1)
   goto newinput;
if (rk < codekount)
   goto ijump;

/* no understand, Senyore... */
printf("I can't understand that.\n");
goto newinput;

}   /* terminates main() */


