/* The Professional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */
/* (c) 1987 Thom Robertson */

#include <exec/types.h>
#include <intuition/intuition.h>
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"

char command[80],word[80];
struct loc2 rm;

main()
{
FILE *fp;
int i,iq,k,rk,rk2,r,codekount,token,prev_rm,rnum,nnum;
int nrflag,orflag,andflag;
char c,cn,terminator;
static char code[MAXCODE];
char thing[200];
int inmode,fragend,mainnum,varval[MAXLIST];
int backnum;
static char *nounlist[MAXLIST];
static char *verblist[MAXLIST];
static char *adjlist[MAXLIST];
static char *extlist[MAXLIST];
char *preplist[6];
static char *prepolist[MAXLIST];
static char *typelist[MAXLIST];
char *listlist[5];
char roomdes[80],program[80];     /* filenames */
int noun,verb,prep,adj,prepobj,l,*ll;
int comcase,comlen,comif;
struct winprint winp1,winp2;
struct Window *win1,*win2;
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
preplist[5]="USING";

fp = fopen("symbol.adv","r");
if (fp == NULL) {
   printf("can't open symbol.adv!!");
   exit(10);
   }
fscanf(fp,"%d,",&nnum);
fscanf(fp,"%d,",&rnum);
fscanf(fp,"%d,",&rk);
fscanf(fp,"%d,%d,",&mainnum,&backnum);

rk2 = 1;
do {
   rm.desc[rk2] = memstring(fp);
   fscanf(fp,"%d,%d,%d,%d,%d,%d,",&rm.n[rk2],&rm.s[rk2],&rm.e[rk2],&rm.w[rk2],&rm.u[rk2],&rm.d[rk2]);
   i = 0;
   while (i<MAXLIST-1)
      rm.item[rk2][i++] = 0;
   i = 1;
   do {
      fscanf(fp,"%d,",&rm.item[rk2][i]);
      } while(i<MAXLIST && rm.item[rk2][i++] > 0);
   } while(++rk2 < rk);

i = 0;
do {
   i++;
   nounlist[i] = memstring(fp);
   } while (nounlist[i][0] != 'z' || nounlist[i][1] != 'y' || nounlist[i][2] != 'x') ;

i = 0;
do {
   i++;
   verblist[i] = memstring(fp);
   } while (verblist[i][0] != 'z' || verblist[i][1] != 'y' || verblist[i][2] != 'x') ;

i = 0;
do {
   i++;
   adjlist[i] = memstring(fp);
   } while (adjlist[i][0] != 'z' || adjlist[i][1] != 'y' || adjlist[i][2] != 'x') ;

i = 0;
do {
   i++;
   prepolist[i] = memstring(fp);
   } while (prepolist[i][0] != 'z' || prepolist[i][1] != 'y' || prepolist[i][2] != 'x') ;

i = 0;
do {
   i++;
   typelist[i] = memstring(fp);
   } while (typelist[i][0] != 'z' || typelist[i][1] != 'y' || typelist[i][2] != 'x') ;

fscanf(fp,"%d,",&codekount);
l = -1;
do  {
   ++l;
   fscanf(fp,"%d,",&i);
   code[l]=i;
   } while (l <= codekount);

printf("load done.");
fclose(fp);

/*  open windows, etc.  */
win1 = nwwin(0,0,640,66);
win2 = nwwin(0,66,640,134);
winp1.WIN = win1;
winp2.WIN = win2;
winp1.stopnum = 0;
winp2.stopnum = 0;
winp1.x = 0;
winp2.x = 0;
winp1.y = 0;
winp2.y = 0;
printx(&winp1,"^c");
printx(&winp2,"^c");

/*  clear inventory   */
i = 1;
do {
  rm.item[0][i] = 0;
  i++;
  } while(i <= MAXLIST-1);

/*  clear variables   */
i = 0;
do {
  varval[i] = 0;
  } while(++i <= MAXLIST-1);


/************************************************/
/**      The following code runs the adventure **/
/************************************************/

printx(&winp2,"Welcome to The Professional Adventure Runtime System^n^n");

nrflag = 1;
l=1;
prev_rm=l;

inmode = 1;
goto interp;

newinput:

winp2.stopnum = 0;

if (l<=0) {
   printx(&winp2,"Sorry, I can't  go that way.^n");
   l=prev_rm;
   nrflag = 0;
   }
varval[rnum] = l;

/*   if (nrflag != 0) {
   ***** load standard room graphic here. *****
   printx(&winp2,"room picture loaded!?!^n");
   }        */
nrflag = 0;

printx(&winp1,"^c");
printx(&winp1,&rm.desc[l][0]);
printx(&winp1,"^n");
printx(&winp1,"Obvious exits are: ");
i = 0;
if (rm.n[l] > 0) {
   printx(&winp1,"North ");
   i = 1;
   }
if (rm.s[l] > 0) {
   printx(&winp1,"South ");
   i = 1;
   }
if (rm.e[l] > 0) {
   printx(&winp1,"East ");
   i = 1;
   }
if (rm.w[l] > 0) {
   printx(&winp1,"West ");
   i = 1;
   }
if (rm.u[l] > 0) {
   printx(&winp1,"Up ");
   i = 1;
   }
if (rm.d[l] > 0) {
   printx(&winp1,"Down ");
   i = 1;
   }
if (i = 0)
   printx(&winp1,"None. ");

printx(&winp1,"^n");

printx(&winp1,"You see:^n");
k = 0;
for(i=0;i<ITEMSINROOM;i++)
   if (rm.item[l][i]!=0) {
      printx(&winp1,nounlist[rm.item[l][i]]);
      printx(&winp1," ");
      k++;
      }
   if (k == 0)
      printx(&winp1,"Nothing.^n");


printx(&winp2,"^nWhat is your command? ");

getx(&winp2);

cap(command);        /* make string all upper case */
prev_rm = l;

if (strcmp(command,"SHOW")==0) {
   i = 0;
   do  {
      ++i;
      printf("%d--%d\n",i,code[i]);
      } while (i <= codekount);
   goto newinput;
   }

if (strcmp(command,"QUIT")==0) {
   if (win1 > 0)
      CloseWindow(win1);
   if (win2 > 0)
      CloseWindow(win2);

   printf("Exiting the Adventure system.\n\n");
   exit(0);
   }
if (strcmp(command,"NORTH")==0||strcmp(command,"N")==0) {
   l=rm.n[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }
if (strcmp(command,"SOUTH")==0||strcmp(command,"S")==0) {
   l=rm.s[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }
if (strcmp(command,"EAST")==0||strcmp(command,"E")==0) {
   l=rm.e[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }
if (strcmp(command,"WEST")==0||strcmp(command,"W")==0) {
   l=rm.w[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }
if (strcmp(command,"UP")==0||strcmp(command,"U")==0) {
   l=rm.u[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }
if (strcmp(command,"DOWN")==0||strcmp(command,"D")==0) {
   l=rm.d[l];
   inmode = 3;
   nrflag = 1;
   goto interp;
   }

if (strcmp(command,"INVENTORY")==0||strcmp(command,"INV")==0) {
   printx(&winp2,"^nYou are carrying:^n");
   if (rm.item[0][0] == 0) {
      printx(&winp2,"Nothing.^n^n");
      goto newinput;
      }
      i = 1;
      while( i <= MAXRMS) {
      if (rm.item[0][i] != 0) {
         printx(&winp2,nounlist[rm.item[0][i]]);
         printx(&winp2,"^n");
         }
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
      if ((k = scanlist2( word,verblist) )!= 0) {
         verb = k;
         goto skip;
         }

   if (noun == 0)
      if ((k = scanlist2(word,nounlist) )!= 0) {
         noun = k;
         goto skip;
         }

   if (adj == 0)
      if ((k = scanlist2(word,adjlist) )!= 0) {
         adj = k;
         goto skip;
         }

   if (prep == 0)
      if ((k = scanlist3(word,preplist) )!= 0) {
         prep = k;
         if (command [ i ] == 0) {
            printx(&winp2,"Your preposition needs an object.^n");
            goto newinput;
            }
         i = getword (i);
         if ((prepobj = scanlist2( word,prepolist )) == 0) {
            printx(&winp2,"I don't understand that preposition.^n");
            goto newinput;
            }
          }
skip:

} /* end parser while loop */

inmode = 2;

/*   interpreter     */

interp:

if (l < 1)
   goto newinput;

if (inmode == 1) {
   inmode = 0;
   rk = 0;
   comif = 0;
   comcase = 0;
   fragend = mainnum;
   goto ijump;
   }

if (inmode == 2) {
   inmode = 3;
   rk = mainnum;
   comif = 0;
   comcase = 0;
   fragend = backnum;
   goto ijump;
   }

if (inmode == 3) {
   inmode = 0;
   rk = backnum;
   comif = 0;
   comcase = 0;
   fragend = codekount;
   goto ijump;
   }

goto newinput;


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

   case PREP:
    if (comcase <1 || comcase > 3)
      comcase = 4;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == prepobj && (comcase < 1 || comcase > 3 ))
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;

   case TYPE:
      if (comcase == 0) {
         printx(&winp2,typelist[code[rk + 1]]);
         if (code[rk + 2] == 0)
            printx(&winp2,"^n");
         }
      comlen = 3;
      break;

   case NUMTYPE:
      if (comcase == 0)
         printy(&winp2,varval[code[rk + 1]]);
      comlen = 2;
      break;

   case OR:
      i = 1;
    if (comif < 1) {
      if (comcase== 0 || comcase== 5) {
         if (comcase== 0) {
            if(code[rk+1] == IFRMCON || code[rk+1] == IFINCON)
               i = i + 2;
            else if(code[rk+1] == IFVARLT || code[rk+1] == IFVAREQ || code[rk+1] == IFVARGT)
               i = i + 4;
            else {
               printx(&winp2,"^nOR not followed by if statment!!!^n");
               exit(10);
               }
            }
         else
            comcase = 0;
         }
       }
      else comif--;

      comlen = i;
      break;
     
   case AND:
      i = 1;
      if (comif > 0)
         comif--;
      comlen = i;
      break;
     
   case DONE:
      if (comcase == 0)
         comcase = -1;
      comlen = 1;
      break;

   case POEND:
      if (comcase < 1 || comcase > 3)
      comcase = 0;
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

   case VEND:
      comcase = 0;
      comlen = 1;
      break;

   case ENDIF:
      if (comcase == 5) {
         if (comif < 1)
            comcase = 0;
         comif--;
         }
      comlen = 1;
      break;

   case END:
    if (comcase == 0) {
      printx(&winp2,"^n^n   This Adventure brought to you by");
      printx(&winp2,"^n         Sunrize Industries.^n^h");
      if (win1 > 0)
         CloseWindow(win1);
      if (win2 > 0)
         CloseWindow(win2);
      exit(0);
      }
    comlen = 1;
    break;

   case TOROOM:
    if (comcase == 0) {
       if (code[rk + 1] == VARIABLE)
          l = varval[code[rk + 2]];
       else
          l = code[rk + 2];
       nrflag = 1;
       varval[rnum] = l;
       }
    comlen = 3;
    break;

   case RMPLUS:
    if (comcase == 0) {
      i = code[rk + 1];
      if (i == 0)
        i = l;
      cn = code[rk + 2];
      if (cn == nnum)
         cn = noun;
      add_r_i(i,cn);
      }
      comlen = 3;
      break;

   case RMMINUS:
    if (comcase == 0) {
      cn = code[rk + 1];
      if (cn == nnum)
         cn = noun;
      sub_r_i(l,cn);
      }
      comlen = 2;
      break;


   case INPLUS:
    if (comcase == 0) {
      cn = code[rk + 1];
      if (cn == nnum)
         cn = noun;
      add_r_i(0,cn);
      rm.item[0][0]++;
      }
      comlen = 2;
      break;


   case INMINUS:
    if (comcase == 0) {
      cn = code[rk + 1];
      if (cn == nnum)
         cn = noun;
      i = 0;
      while (i<=MAXRMS) {
        if (rm.item[0][i] == cn)
           rm.item[0][0]--;
        i++;
        }
      sub_r_i(0,cn);
      }
      comlen = 2;
      break;


   case INSTEAD:
      if (comcase == 5 && comif <1)
         comcase = 0;
      else
      if (comcase == 0 && comif <1) {
         comif = 0;
         comcase = 5;
         }
      comlen = 1;
      break;

   case DESTROY:
    i = 1;
    if (comcase == 0 ) {
      cn = code[rk+1];
      if (cn == nnum)
         cn = noun;
      while (i<MAXRMS) {
        if (rm.item[0][i] == cn) {
           rm.item[0][i] = 0;
           rm.item[0][0]--;
           }
        i++;
        }
      iq = 1;
      while (iq <rk) {
         i = 1;
         while (i<MAXRMS) {
           if (rm.item[iq][i] == cn)
              rm.item[iq][i] = 0;
           i++;
           }
         iq++;
         }
      }
      comlen = 2;
      break;

   case IFRMCON:
    i = 0;
    if (comcase == 5 )
       comif++;
    if (comcase == 0 ) {
      cn = code[rk+1];
      if (cn == nnum)
         cn = noun;
      while (rm.item[l][i] != cn && i <MAXRMS +1)
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
      cn = code[rk+1];
      if (cn == nnum)
         cn = noun;
      while (rm.item[0][i] != cn && i <MAXRMS +1)
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
            exit(10);
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
      exit(10);

  }  /*  switch end */

rk = rk + comlen;
if (comcase == -1)
   goto interp;
if (rk < fragend)
   goto ijump;

/* no understand, Senyore... */
if (inmode == 3) {
   printx(&winp2,"I can't understand that.^n");
   inmode = 0;
   }
goto interp;


}   /* terminates main() */


