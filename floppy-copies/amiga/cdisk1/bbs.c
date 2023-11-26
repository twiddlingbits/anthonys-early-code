/* Amiga Bulletin Board System */
/* Copyright 1986 by Anthony Wood */
/* last update 01-05-86 */

#include <df0:include/lattice/stdio.h>
#define MAX 50
main()
{
FILE *fp,*fopen();
int kount,len,no_topics,j,i,cmd1[MAX][25],cmd2[MAX][25],option;
int *next_menu[MAX],*prev_menu[MAX];
char *topic[MAX],*fname[MAX],*getmem();
char cmdlist[MAX][25],string[90];
printf("\014Anthony's great BBS Creation menu\n\n");
fp=fopen("DF0:menu-pointers","r");
if (fp==NULL) printf("Warning: No menus defined");
else {
      fscanf(fp,"%d",&no_topics);
      for(kount=0;kount!=no_topics;kount++) {
          if (fgets(string,99,fp)==NULL) bomb();
          topic[kount]=getmem(strlen(string)+1);
          strcpy(topic[kount],string);
          }
      kount=0;
      while(fgets(string,99,fp)!=NULL) {
          fname[kount]=getmem(strlen(string)+1);
          strcpy(fname[kount],string);
          if (fscanf(fp,"%d",&next_menu[kount])==EOF) bomb();
          if (fscanf(fp,"%d",&prev_menu[kount])==EOF) bomb();
          if (fscanf(fp,"%s",&cmdlist[kount][0])==EOF) bomb();
          len=strlen(&cmdlist[kount][0]);
          for(i=0;i!=len;i++)
              if (fscanf(fp,"%d %d",&cmd1[kount][i],&cmd2[kount][i])==EOF) bomb();
          }
          if(fclose(fp)==-1) bomb();
          printf("%d menus created in %d topics",kount-1,no_topics);
}
option=0;
while (option!=4) {
  printf("\n\n1.  Replace a menu\n2.  Add a menu\n");
  printf("3.  Delete a menu\n\n");
  printf("option?");
  scanf("%d",option);
  switch (option) {
   case 1: ; break;
   case 2: {
            printf("Adding menu number %d\n",kount);
           }
   case 3: ; break;
 }}
/* save menu definitions to disk */
if((fp=fopen("DF0:menu_pointers"))==NULL) {
    printf("ERROR: Unable to open 'menu_pointers' for write!");
    exit(1); }
printf("Writting new menu data to disk.");
for(i=0;i!=no_topics;i++)
    if(fputs(topic[i],fp)==EOF) bomb();
for(i=0;i!=kount-1;i++) {
    if(fputs(fname[i],fp)==EOF) bomb();
    fprintf(fp,"%d\n %d\n %s",next_menu[i],prev_menu[i],&cmdlist[i][0]);
    for(j=0;j!=strlen(&cmdlist[i][0]);j++)
        fprintf(fp,"%d\n %d\n",cmd1[i][j],cmd2[i][j]);
    }
fclose(fp);
exit(0);
}
/* Exit when disk error function */
bomb()
{
fprint("ERROR: Unexpected disk I/O error!");
exit(1);
}

