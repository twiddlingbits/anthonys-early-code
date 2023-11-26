/* The Professional Adventure System ver 4.0 */
/* Amiga version */
/* (c) 1986 Anthony Wood */
/* (c) 1987 Thom Robertson */

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <lattice/stdio.h>
#include "adventure.h"

struct IntuitionBase *IntuitionBase;
char command[80],word[80];
struct loc2 rm;
char *nounlist[MAXLIST];
char *verblist[MAXLIST];
char *adjlist[MAXLIST];
char *extlist[MAXLIST];
char *ext2list[MAXRMS];
char *ext3list[MAXLIST];
char *prepolist[MAXLIST];
char *typelist[MAXLIST];

main()
{
FILE *fp;
int i,ii,iq,k,rk,rk2,r,codekount,token,prev_rm,rnum,nnum;
int nrflag,orflag,andflag;
int screenw,screenh,numplanes,picx,picy;
int winx,winy,winw,winh;
char c,cn,terminator;
static char code[MAXCODE];
char thing[200];
int inmode,fragend,mainnum,varval[MAXLIST];
int backnum,objnum[2],objx[2][10],objy[2][10],objlist[100];
static int noun_pics[MAXLIST];
char *preplist[6];
char *listlist[5];
char roomdes[80],program[80];     /* filenames */
int noun,verb,prep,adj,prepobj,l,*ll;
int comcase,comlen,comif;
struct Image *object_images[MAXLIST];
struct winprint winp1,winp2;
struct Window *win1,*win2;
struct Screen *screen;
struct graphic graph1;
struct Image image,*tpoint;

IntuitionBase = (struct IntuitionBase *) OpenLibrary(
                "intuition.library",0);
if (IntuitionBase == NULL)   exit(10);

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


   getlist1(fp);


   fscanf(fp,"%d,%d,",&screenw,&screenh);
   fscanf(fp,"%d,",&numplanes);
   fscanf(fp,"%d,%d,",&picx,&picy);
   fscanf(fp,"%d,%d,",&winx,&winy);
   fscanf(fp,"%d,%d,",&winw,&winh);

   printf(">%d,%d,",winx,winy);
   printf(">%d,%d,",winw,winh);

   fscanf(fp,"%d,",&objnum[0]);
   i = 0;
   while (i < objnum[0])
      fscanf(fp,"%d,%d,",&objx[0][i],&objy[0][i++]);
   
   fscanf(fp,"%d,",&objnum[1]);
   i = 0;
   while (i < objnum[1])
      fscanf(fp,"%d,%d,",&objx[1][i],&objy[1][i++]);
   
   i = 0;
   while (i++ <MAXLIST) {
      fscanf(fp,"%d,",&noun_pics[i]);
      }
   
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
screen = openscreen(screenw,screenh,numplanes);
printf("boo1");
win1 = nwwin(screen,winx,winy,winw,winh);
printf("boo2");
winp1.WIN = win1;
winp1.stopnum = 0;
winp1.x = winx;
winp1.y = winy;
printx(&winp1,"^c");
printf("boo3");

i = 1;
while (ext3list[i][0] != 'z' || ext3list[i][1] != 'y' || ext3list[i][2] != 'x') {
   object_images[i] = AllocMem(sizeof(struct Image),MEMF_CHIP);
   if (object_images[i] == 0) {
      printf("no space for object images, dammit!");
      goto ender1;
      }
   l = 0;
   l = getgraphic(object_images[i],ext3list[i],0);
   if (l != 0) {
      printf("can't get an  object, dammit!");
      goto ender1;
      }
   i++;
   }

i = 0;
i = getgraphic(&image,ext2list[1],1);
if (i != 0) {
   goto ender1;
   }
DrawImage(&screen->RastPort,&image,0,0);

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

i = 0;
do {
  objlist[i] = 0;
  } while(++i <100);


/************************************************/
/**      The following code runs the adventure **/
/************************************************/

printx(&winp1,"Welcome to The Professional Adventure Runtime System^n^n");

nrflag = 1;
l=1;
prev_rm=l;

inmode = 1;
goto interp;

newinput:

if (l<=0) {
   printx(&winp1,"Sorry, I can't  go that way.^n");
   l=prev_rm;
   nrflag = 0;
   }
varval[rnum] = l;

if (nrflag != 0) {
   /***** load standard room graphic here. *****/
i = 0;
i = getgraphic(&image,ext2list[l+1],1);
if (i != 0) {
   goto ender1;
   }
DrawImage(&screen->RastPort,&image,picx,picy);
nrflag = 0;

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
}

winp1.stopnum = 0;

printx(&winp1,"^nWhat is your command? ");
winp1.stopnum = 0;

getx(&winp1);

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
   ender1:
   if (win1 > 0)
      CloseWindow(win1);
   if (screen > 0)
      CloseScreen(screen);

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
   printx(&winp1,"^nYou are carrying:^n");
   if (rm.item[0][0] == 0) {
      printx(&winp1,"Nothing.^n^n");
      goto newinput;
      }
      i = 1;
      while( i <= MAXRMS) {
      if (rm.item[0][i] != 0) {
         printx(&winp1,nounlist[rm.item[0][i]]);
         printx(&winp1,"^n");
         }
      i++;
         }
   printx(&winp1,"^h");
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
            printx(&winp1,"Your preposition needs an object.^n");
            goto newinput;
            }
         i = getword (i);
         if ((prepobj = scanlist2( word,prepolist )) == 0) {
            printx(&winp1,"I don't understand that preposition.^n");
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
      comlen = caseverb(&comcase,&code[rk],verb);
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
         printx(&winp1,typelist[code[rk + 1]]);
         if (code[rk + 2] == 0)
            printx(&winp1,"^n");
         }
      comlen = 3;
      break;

   case NUMTYPE:
      if (comcase == 0)
         printy(&winp1,varval[code[rk + 1]]);
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
               printx(&winp1,"^nOR not followed by if statment!!!^n");
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
      printx(&winp1,"^n^n   This Adventure brought to you by");
      printx(&winp1,"^n         Sunrize Industries.^n^h");
      if (win1 > 0)
         CloseWindow(win1);
     if (screen > 0)
         CloseScreen(screen);
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
      if (noun_pics[cn] > 0) {
         i = 0;
         while (objlist[i] > 0 && i<101)
            i++;
         if (i <objnum[0]) {
            DrawImage(&screen->RastPort,object_images[noun_pics[cn]],
            objx[0][i],objy[0][i]);
            objlist[i] = noun_pics[cn];
            }
         else if (i<101)
            objlist[i] = noun_pics[cn];
         }
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
      if (noun_pics[cn] > 0) {
         i = 0;
         while (objlist[i] != noun_pics[cn] && i<100)
            i++;
         if (i <objnum[0]) {
            SetAPen(&screen->RastPort,0);
            tpoint = object_images[noun_pics[cn]];
            RectFill(&screen->RastPort, objx[0][i], objy[0][i],
            objx[0][i] + tpoint->Width, objy[0][i] + tpoint->Height);
            objlist[i] = 0;
            ii = objnum[0];
            while (objlist[ii] == 0 && ii<100)
               ii++;
            if (ii < 100) {
               objlist[i] = objlist[ii];
               objlist[ii] = 0;
               DrawImage(&screen->RastPort,object_images[objlist[i]],
               objx[0][i],objy[0][i]);
               }
            }
         else if (i<101)
            objlist[i] = 0;
         }
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
   printx(&winp1,"I can't understand that.^n");
   inmode = 0;
   }
goto interp;


}   /* terminates main() */


