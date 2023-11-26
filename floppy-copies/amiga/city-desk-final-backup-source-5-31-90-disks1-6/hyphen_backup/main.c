#include "stdio.h"

static char p[4450][8];
static char n[4450][9];
static char pat_len[4450];  /** number of n's not p's ! **/
static int num_patterns;
static int num_excepts;
static char except[250];
static char except_hyp[250];
static char except_len[25];
static short except_ptr[25];

main()
{
int i;
char word[60];
char *hyp;
unsigned char start_time[8], end_time[8];
int elapsed_time;

  /** must be called before any calls to autohyphenate **/
load_patterns("hyphen.tex");
printf("loaded %d patterns\n",num_patterns);

loop:
printf("Enter word to hyphenate: ");
scanf("%s",word);
if (word[0]=='x')
   exit(0);

getclk(start_time);
hyp = autohyphenate(word);
getclk(end_time);

for (i=0; i < strlen(word); i++) {
   printf("%c",word[i]);
   if (hyp[i]&1)
      printf("-");
   }

elapsed_time = (end_time[6]*100+end_time[7]) -
               (start_time[6]*100+start_time[7]);

printf("\nTotal time = %d 100ths of second\n\n",elapsed_time);
goto loop;
}

/**************************************************************************/
/** Entry:  A sequence of characters to hyphenate                        **/
/** Exit:   returns  'char *hyp' where hyp is an array of small numbers  **/
/**         that indicates where to put hyphens.  An ODD value indicates **/
/**         a hyphenation point. Example:                                **/
/**         help-ing  == 0,2,0,5,2,0,0    (only small ODD numbers count) **/
/**************************************************************************/

autohyphenate(short_word)
char *short_word;
{
char word[62];
char pattern[10];
int i,j,k;
int len,pos,p_len;
static char hyp[62];

if (check_exception(short_word, hyp))
   return(hyp+1);

strcpy(&word[1], short_word);
word[0] = '.';          /** add periods that mark start & end of word **/
len = strlen(word);
word[len++] = '.';
word[len] = 0;

for (i=0; i < len; i++)
   hyp[i] = 0;

if (len < 7)         /** Don't hyphenate words less than 5 characters **/
   return(hyp+1);

for (pos=0; pos < len-1; pos++) {
   for (p_len=2; p_len <= min(8,len-pos); p_len++) {
      stccpy(pattern, &word[pos], p_len+1);
/* printf("p_len=%d pattern = %s\n",p_len,pattern); */
      i = bin_match(pattern);
      if (i!=-1) {
         j = pos - 1;
         for (k=0; k < pat_len[i]; k++) {
            if ( (j+k) >= 0 && n[i][k] > hyp[j+k])
               hyp[j+k] = n[i][k];
            }
         i++;
         }
      }
   }

/* for (i=0; i < len; i++)
   printf("hyp[%d] = %d\n",i,hyp[i]); */

hyp[1] = 0;
hyp[len-2] = 0;   /** require a at least three letters after hyphen **/
hyp[len-3] = 0;
hyp[len-4] = 0;
return(hyp+1);
}

/************************************************************************/
/** given a null terminated sequence of characters, do a binary search **/
/** on the hyphenation pattern tables for the characters.  Returns     **/
/** an index into the pattern tables if found. Otherwise, returns -1   **/
/************************************************************************/

bin_match(pattern)
char *pattern;
{
int index;
int x;
int top,bot;
int stuck;

top = 0;
bot = num_patterns - 1;
stuck = 0;

while (1) {
   index = top + (bot - top+1)/2;
/*printf("index %d top %d bot %d pat %s\n",index,top,bot,p[index]); */
   x=stricmp(pattern, p[index]);
   if (x < 0)       /** pattern < list pattern **/
      bot = index;
   else if (x > 0)   /** first string above second **/
      top = index;
   else {
      return(index);
      }
   if (top==bot)
      return(-1);
   if (bot-top==1) {
      stuck++;
      if (stuck==2)
         return(-1);
      }
   }
}

/**************************************************/
/** search the exception tables for a given word **/
/**************************************************/

check_exception(word, hyp)

char *word;
char *hyp;

{
int index;
int x;
int top,bot;
int stuck,i;

top = 0;
bot = num_excepts - 1;
stuck = 0;

while (1) {
   index = top + (bot - top+1)/2;
/* printf("index %d top %d bot %d word %5.5s\n",index,top,bot,&except[except_ptr[index]]); */
   x=strnicmp(word,&except[except_ptr[index]],except_len[index]);
   if (x < 0)       /** pattern < list pattern **/
      bot = index;
   else if (x > 0)   /** first string above second **/
      top = index;
   else {
      for (i=0; i<except_len[index]; i++)
         hyp[i+1]=except_hyp[except_ptr[index]+i];
      return(1);
      }
   if (top==bot)
      return(0);
   if (bot-top==1) {
      stuck++;
      if (stuck==2)
         return(0);
      }
   }
}

/********************************************************************/
/**   Entry:   path to a TeX style list of patterns and exceptions **/
/**   Exit:    Fills global variables that describe patterns and   **/
/**            hyphenation exceptions                              **/
/********************************************************************/

load_patterns(path)
char *path;
{
FILE *fp;
int i,j,k;
char line[80];
int maxsize=0;
int sum =0;
int e_ptr;

fp = fopen(path,"r");
if (fp==0) {
   printf("Can't open hyphen.tex\n");
   exit(0);
   }

fgets(line, 80, fp);
fgets(line, 80, fp);

i=0;
while (1) {
   fgets(line, 80, fp);
   if (line[0] == '}')
      break;

   for (k=0; k < 11; k++)
      n[i][k]=0;

   k = 0;
   for (j=0; j < strlen(line)-1; j++) {
      if (isalpha(line[j]) || line[j]=='.') {
         p[i][k++] = line[j];
         }
      else {
         n[i][k] = line[j] - '0';
         }
      }
   p[i][k]=0;
   pat_len[i] = k+1;
   if (pat_len[i] > maxsize) maxsize = pat_len[i];
   sum = sum + k;
   /* show_pat(i); */
   i++;
   }
printf("maxsize = %d avg = %d\n",maxsize,sum/(i-1));
num_patterns = i;

fgets(line, 80, fp); /** skip \hyphenation line **/

k=e_ptr=0;

while (1) {
   fgets(line, 80, fp);
   if (line[0] == '}')
      break;

   except_ptr[k] = e_ptr;
   for (j=0; j < strlen(line)-1; j++) {
      if (line[j]!='-') {
         except[e_ptr++] = line[j];
         }
      else {
         except_hyp[e_ptr-1] = 1;
         }
      }
   except_len[k] = e_ptr - except_ptr[k];
   k++;
   }

num_excepts = k;

/*
for (i=0; i < num_excepts; i++)  {
   for (k=0; k<except_len[i]; k++) {
      printf("%c",except[except_ptr[i]+k]);
      }
   printf("\n");

   for (k=0; k<except_len[i]; k++) {
      printf("%d ",except_hyp[except_ptr[i]+k]);
      }
   printf("\n");
   }  */

fclose(fp);
}


show_pat(i)

int i;

{
int j;

printf("%d\n",pat_len[i]);
printf("%s\n",p[i]);
for (j=0; j<pat_len[i]; j++)
   printf("%d ",n[i][j]);
printf("\n\n");
}

