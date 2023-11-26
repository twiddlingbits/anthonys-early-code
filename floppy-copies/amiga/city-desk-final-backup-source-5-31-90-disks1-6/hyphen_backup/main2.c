#include "stdio.h"

char p[4450][8];
char n[4450][9];
char pat_len[4450];  /** number of n's not p's ! **/
int num_patterns;

main()
{
char hyp[20];
char word[20];
int i,j,k;
int len,match,pos;
unsigned char start_time[8], end_time[8];
int elapsed_time;

load_patterns();
printf("loaded %d patterns\n",num_patterns);

loop:

printf("Enter word to hyphenate: ");
scanf("%s",&word[1]);
if (word[1]=='x')
   exit(0);

word[0] = '.';             /** add periods that mark start & end of word **/
len = strlen(word);
word[len++] = '.';
word[len] = 0;
for (i=0; i < len; i++)
   hyp[i] = 0;

/* printf("Hyphenating '%s'\n",word); */

getclk(start_time);

for (pos=0; pos < len-1; pos++) {
   i = first_match(&word[pos]);
   if (i!=-1) {
      show_pat(i);
      do {
         j = pos - 1;
         for (k=0; k < pat_len[i]; k++) {
            if ( (j+k) >= 0 && n[i][k] > hyp[j+k])
               hyp[j+k] = n[i][k];
            }
         i++;
         match = strnicmp(&word[pos], p[i], pat_len[i]-1);
         } while (match==0);
      }
   }

getclk(end_time);

/* for (i=0; i < len; i++)
   printf("hyp[%d] = %d\n",i,hyp[i]); */

hyp[1] = 0;
hyp[len-2] = 0;
hyp[len-3] = 0;
hyp[len-4] = 0;

for (i=1; i < len-1; i++) {
   printf("%c",word[i]);
   if (hyp[i]&1)
      printf("-");
   }
elapsed_time = (end_time[6]*100+end_time[7]) -
               (start_time[6]*100+start_time[7]);
printf("\nTotal time = %d 100ths of second\n\n",elapsed_time);
goto loop;
}

first_match(word_part)
char *word_part;
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
printf("index %d top %d bot %d pat %s\n",index,top,bot,p[index]);
   x=strnicmp(word_part, p[index], pat_len[index]-1);
   if (x < 0)       /** word_part < pattern **/
      bot = index;
   else if (x > 0)   /** first string above second **/
      top = index;
   else {            /** found a match, backup to the first match */
      do {
         index--;
         x=strnicmp(word_part, p[index], pat_len[index]-1);
         } while (x==0);
      return(index+1);
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

load_patterns()
{
FILE *fp;
int i,j,k;
char line[80];
int maxsize=0;
int sum =0;

fp = fopen("aw:cd/tex/hyphen.tex","r");
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
fclose(fp);
printf("maxsize = %d avg = %d\n",maxsize,sum/(i-1));
num_patterns = i-1;
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

