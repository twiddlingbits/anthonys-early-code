
#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include "adventure.h"



getlist1(fp,rk)
FILE *fp;
int rk;
{
extern char *nounlist[MAXLIST];
extern char *verblist[MAXLIST];
extern char *adjlist[MAXLIST];
extern char *prepolist[MAXLIST];
extern char *extlist[MAXLIST];
extern char *ext2list[MAXLIST];
extern char *ext3list[MAXLIST];
extern char *typelist[MAXLIST];
extern struct loc2 rm;
int i;

i = 0;
do {
   i++;
   nounlist[i] = memstring(fp);
   } while (nounlist[i][0] != 'z' || nounlist[i][1] != 'y' || nounlist[i][2] != 'x') ;
nounlist[++i] = 0;

i = 0;
do {
   i++;
   verblist[i] = memstring(fp);
   } while (verblist[i][0] != 'z' || verblist[i][1] != 'y' || verblist[i][2] != 'x') ;
verblist[++i] = 0;

i = 0;
do {
   i++;
   adjlist[i] = memstring(fp);
   } while (adjlist[i][0] != 'z' || adjlist[i][1] != 'y' || adjlist[i][2] != 'x') ;
adjlist[++i] = 0;

i = 0;
do {
   i++;
   prepolist[i] = memstring(fp);
   } while (prepolist[i][0] != 'z' || prepolist[i][1] != 'y' || prepolist[i][2] != 'x') ;
prepolist[++i] = 0;

i = 0;
do {
   i++;
   typelist[i] = memstring(fp);
   } while (typelist[i][0] != 'z' || typelist[i][1] != 'y' || typelist[i][2] != 'x') ;
typelist[++i] = 0;

i = 0;
do {
   i++;
   extlist[i] = memstring(fp);
   } while (extlist[i][0] != 'z' || extlist[i][1] != 'y' || extlist[i][2] != 'x') ;
extlist[++i] = 0;

i = 0;
do {
   i++;
   ext2list[i] = memstring(fp);
   } while (ext2list[i][0] != 'z' || ext2list[i][1] != 'y' || ext2list[i][2] != 'x') ;
ext2list[++i] = 0;

i = 0;
do {
   i++;
   ext3list[i] = memstring(fp);
   } while (ext3list[i][0] != 'z' || ext3list[i][1] != 'y' || ext3list[i][2] != 'x') ;
ext3list[++i] = 0;


}

