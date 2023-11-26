
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


extern char command[80],word[80];
extern char varlist[MAXLIST][ITEMSPACE];
extern char nounlist[MAXLIST][ITEMSPACE];
extern int noun_pics[MAXLIST];
extern char verblist[MAXLIST][ITEMSPACE];
extern char adjlist[MAXLIST][ITEMSPACE];
extern char extlist[MAXLIST][ITEMSPACE];
extern char ext2list[MAXRMS][ITEMSPACE];
extern char ext3list[MAXLIST][ITEMSPACE];
extern char prepolist[MAXLIST][ITEMSPACE];
extern char typelist[MAXLIST][ITEMSPACE];
extern char terminator;
extern UBYTE code[MAXCODE];
extern char thing[200],thing2[200];
extern int codekount;

void codeput1() {

int l,*ll;
ll = &l;

code[codekount++]=NUMBER;
stcd_i (thing,ll);
code[codekount++]= l & 255;
l = l>>8;
code[codekount++]= l & 255;
}


void dataput2() {

code[codekount++]=VARIABLE;
code[codekount++]=addlist(thing,varlist);
}

void dataput3() {

int l,*ll;
ll = &l;

code[codekount++]=NUMBER;
stcd_i (thing,ll);
code[codekount++]= l;
}


