#include "stdio.h"

char p[5000][10];
char n[5000][11];
char pat_len[5000];  /** number of n's not p's ! **/

main()
{
char hyp[20];
char word[20];
char *loc, *search;
int num_patterns;
int i,j,k;
int len,match;

num_patterns = load_patterns();
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

printf("Hyphenating '%s'\n",word);

for (i=0; i<num_patterns; i++) {
   search = word;
   match = stcpm(search, p[i], &loc);
   while (match) {
      j = loc - word-1;
      printf("matched pattern %s at %d\n",p[i],j);
      show_pat(i);
      for (k=0; k < pat_len[i]; k++) {
         if ( (j+k) >= 0 && n[i][k] > hyp[j+k])
            hyp[j+k] = n[i][k];
         }
      search = loc+1;
      if (search >= word + len)
         match = 0;
      else
         match = stcpm(search, p[i], &loc);
      }
   }

for (i=0; i < len; i++)
   printf("hyp[%d] = %d\n",i,hyp[i]);

hyp[1] = 0;
hyp[len-2] = 0;
hyp[len-3] = 0;
hyp[len-4] = 0;

for (i=1; i < len-1; i++) {
   printf("%c",word[i]);
   if (hyp[i]&1)
      printf("-");
   }
printf("\n");
goto loop;
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
return(i-1);
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

