#include "stdio.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
     
COORD cursor_x=0;
COORD cursor_y=0;
COORD understart_x;
COORD understart_y;
struct laserfont *current_lfont;

/**********************/
/** under line start **/
/**********************/
     
int ps_underon()
{
understart_x = cursor_x;
understart_y = cursor_y;
}
     
/*****************/
/** under off ****/
/*****************/
     
int ps_underoff()
{
ps_hline(understart_x,understart_y + 6,cursor_x - understart_x,1,0xFFFF);
ps_hline(understart_x,understart_y + 7,cursor_x - understart_x,1,0xFFFF);
}
     
     
/*****************/
/*** do nothing **/
/*****************/
     
int ps_nada()
{
}
     
/******************************/
/*** PostScript Page seteup ***/
/******************************/
     
int ps_init()
{
extern int plaser;
extern short prt_passes;
     
plaser=TRUE;
     
/* poststr("executive"); */
poststr("clear");
/*
poststr("errordict begin");  This changes cntrl c to do a quit
poststr("/interrupt {quit} def");
poststr("userdict begin");
*/
poststr("/mx {currentpoint exch pop moveto} def");
poststr("/my {currentpoint pop exch moveto} def");
poststr("/nf {findfont exch scalefont setfont} def");
poststr("/sh {show} def\n0 0 moveto");
poststr("0 792 translate");    /** move orig. to upper left **/
poststr(".125 .125 scale");    /** userspace is now in COORDs **/
prt_passes = 1;
return(AOK);
}
     
/************************/
/*** PostScript movex ***/
/************************/
     
int ps_movex(x)
COORD x;
{
char t[25];
     
cursor_x=x;
sprintf(t,"%d mx",x);
poststr(t);
}
     
/*************************/
/*** PostScript move y ***/
/*************************/
     
int ps_movey(y)
COORD y;
{
char t[25];
     
cursor_y=y;
sprintf(t,"%d my", -y);
poststr(t);
}
     
/**************************/
/*** PostScript setfont ***/
/**************************/
     
int ps_setfont(lfont)
struct laserfont *lfont;
{
char t[50];
     
current_lfont = lfont;  /** need this in prtchr sometimes **/
sprintf(t,"%d /%s nf",lfont->size,lfont->fonton);
poststr(t);
}
     
/***************************/
/*** PostScript prtchar ****/
/***************************/
     
int ps_prtchar(ch)
UBYTE ch;
{
COORD y;
UBYTE amiga_ch;
char t[40];
short adj;    /** the amout to move "two little dots" on extended chars **/
extern UBYTE *translate;
extern UBYTE ps_extra[];
     
amiga_ch = ch;
ch = translate[ch];  /** translate amiga extended to printer kind **/
     
if (ch=='(') {
   poststr("(\\() sh");
   return(0);
   }
     
if (ch==')') {
   poststr("(\\)) sh");
   return(0);
   }
     
if (ch=='\\') {
   poststr("(\\\\) sh");  /** two for 'c' and two for PS **/
   return(0);
   }
     
if ( (ch&0x80) || ch < 32) {
   sprintf(t,"(\\%03o) sh",ch);
   poststr(t);
   }
else {
   sprintf(t,"(%c) sh",ch);
   poststr(t);
   }
     
if (ps_extra[amiga_ch]) {   /** Character with accent or something? **/
   y = cursor_y;
   adj = current_lfont->widthtable[ch] -
         current_lfont->widthtable[ps_extra[amiga_ch]];
   ps_movex(cursor_x + adj/2);    /** restore ps cursor to where it was  **/
   ps_movey(cursor_y-badfix(ch));  /** plus small offset to center dots **/
   sprintf(t,"(\\%03o) sh",ps_extra[amiga_ch]);
   poststr(t);
   ps_movey(y); /** put it back where its suposed to be **/
   }
}
     
     
/*******************************************************************/
/** given a U, A, O calculate vertical adjustment for "two dots" **/
/*******************************************************************/
     
badfix(ch)
     
char ch;
     
{
if (!strcmp(current_lfont->fontname,"roman"))
   if (ch < 'a')
      return((219*current_lfont->size)/1000);
   else
      return(0);
     
if (!strcmp(current_lfont->fontname,"helv"))
   if (ch < 'a')
      return((200*current_lfont->size)/1000);
   else
      return(0);
     
if (!strcmp(current_lfont->fontname,"courier"))
   if (ch < 'a')
      return((150*current_lfont->size)/1000);
   else
      return(0);
     
return(0);
}
     
     
/**************************/
/*** PostScript endpage ***/
/**************************/
     
int ps_endpage()
{
char t[25];
extern FILE *fp;
extern struct printerprefs printerprefs;

sprintf(t,"/#copies %d def",printerprefs.copies);
poststr(t);
poststr("showpage\n");
if (checkcan()) {
   poststr("\004\003");  /* cntrl D, C */
   }
     
if (fp==0) {
   closepost();
   Delay(1000);   /** wait about 20 seconds... Give page time to print **/
   }
else {
   fclose(fp);
   }
plaser=FALSE;
}
     
/***********************************/
/*** PostScript draw horiz. line ***/
/***********************************/
     
/** note, x,y is the upper left corner of the line **/
     
int ps_hline(x,y,w,thick,pat)
     
COORD x,y,w,thick;
UWORD pat;
     
{
char t[60];
     
sprintf(t,"newpath\n%d %d moveto",x, -y - (thick>>1));
poststr(t);
     
sprintf(t,"%d %d rlineto",w,0);
poststr(t);
     
sprintf(t,"%d setlinewidth",thick);
poststr(t);
     
makepat(pat);                    /** generate "setdash" command **/
poststr("stroke");
poststr("0 0 moveto");
}
     
/***********************************/
/*** PostScript draw vert.line   ***/
/***********************************/
     
/** note, x ,y marks the upper left corner of the line **/
     
int ps_vline(x,y,w,thick,pat)
     
COORD x,y,w,thick;
UWORD pat;
     
{
char t[60];
     
sprintf(t,"newpath\n%d %d moveto", x + (thick>>1), -y);
poststr(t);
     
sprintf(t,"%d %d rlineto",0,-w);
poststr(t);
     
sprintf(t,"%d setlinewidth",thick);
poststr(t);
     
makepat(pat);                    /** generate "setdash" command **/
poststr("stroke");
poststr("0 0 moveto");
}
     
/***********************************************/
/*** genereate  "setdash" PostScript command ***/
/***********************************************/
     
makepat(pat)
UWORD pat;
{
extern FILE *fp;
SHORT kountons,bit,onbits;
char t[40];
     
if (pat==0xFFFF) {
   poststr("[] 0 setdash");
   return(0);
   }
     
poststr2("[");    /** send with no newline after **/
bit=7;
     
if (pat & 128)
   kountons=TRUE;
else {
   poststr2("0 ");
   kountons=FALSE;
   }
     
while (TRUE) {
     
   onbits=0;
   if (kountons) {   /** kount on bits **/
      while (bit >=0 && (pat&(1<<bit))) {
         onbits++;
         bit--;
         }
      kountons=FALSE;
      }
   else {       /** kount off bits **/
      while (bit >=0 && (pat&(1<<bit))==0) {
         onbits++;
         bit--;
         }
      kountons=TRUE;
      }
     
     
   if (onbits != 0)  {
      sprintf(t,"%d ",onbits<<3);
      poststr2(t);
      }
     
   if (bit < 0) {
      poststr("] 0 setdash");
      return(0);
      }
   }
}
     
/***********************************/
/*** PostScrip box draw function ***/
/***********************************/
     
int ps_box(x,y,h,w,thick,pat)
     
COORD x,y,h,w,thick;
UWORD pat;
     
{
char t[65];
     
sprintf(t,"newpath\n%d setlinewidth",thick);
poststr(t);
     
makepat(pat);
     
sprintf(t,"%d %d moveto",x,-y);
poststr(t);
sprintf(t,"%d %d rlineto",0,-h);
poststr(t);
sprintf(t,"%d %d rlineto",w,0);
poststr(t);
sprintf(t,"%d %d rlineto",0,h);
poststr(t);
     
poststr("closepath");
poststr("stroke");
poststr("0 0 moveto");
}
     
/**********************************************/
/*** Send a graphic to a PostScript printer ***/
/**********************************************/
     
int ps_graphic(obj)
     
struct object *obj;
     
{
extern FILE *fp;
extern struct printerprefs printerprefs;
int bytesperrow;
long i,j,k,off;
struct graphic *g;
UBYTE temp;
char t[70];
static char hex[]={"FEDCBA9876543210"};

     
set_light(printerprefs.flags);    /** Checks DITHER_LIGHT **/

g=(struct graphic *)obj->data;

if (g->numplanes==1) {
   return(ps_graphic_oneplane(obj));
   }
     
j = g->usew*g->useh;   /** total bytes in graphic **/

for (i=256; i>1; i--)   /** find largest multiple **/
   if ( (j/i)*i == j)
      break;
     
printf("len %d piclen %d\n",j,i);
     
sprintf(t,"gsave\n/picstr %d string def",i);
poststr(t);
sprintf(t,"%d %d translate",obj->x,-obj->y - obj->h);
poststr(t);
sprintf(t,"%d %d scale\n0 setgray",obj->w,obj->h);
poststr(t);
sprintf(t,"%d %d 8 [%d 0 0 %d 0 %d]",
   g->usew,g->useh,g->usew,-g->useh,g->useh);
poststr(t);
     
/* poststr("{(%stdin) (r) file picstr readhexstring pop}"); */
     
poststr("{currentfile picstr readhexstring pop}");
poststr("image");
     
j=0;  /** I should change this two 2 samples per byte!!! **/
      /** see note bottom pg 74 PostScript Ref Man **/

for (i=0; i < g->useh; i++) {
   for (k=0 ; k < g->usew ; k++) {

      t[0] = hex[get_pix_val(g, k, i)];
      t[1] = t[0];
      t[2] = 0;
      poststr2(t);
     
      if (j++ > 35) {
         j=0;
         poststr("");
         }
      }
   if (checkcan())
      break;
   }
     
poststr("\ngrestore");
     
return(AOK);
}
     
/************************************************************/
/*** Send a one bit plane graphic to a PostScript printer ***/
/************************************************************/
     
int ps_graphic_oneplane(obj)
     
struct object *obj;
     
{
extern FILE *fp;
int bytesperrow;
long i,j,k,off;
struct graphic *g;
UBYTE temp;
char t[70];
static char hex[]={"0123456789ABCDEF"};
     
g=(struct graphic *)obj->data;
     
j = ((g->usew+7) >> 3)*((long)g->useh);   /** total bytes in graphic **/
     
for (i=256; i>1; i--)   /** find largest multiple **/
   if ( (j/i)*i == j)
      break;
     
/* printf("len %d piclen %d\n",j,i); */
     
sprintf(t,"gsave\n/picstr %d string def",i);
poststr(t);
sprintf(t,"%d %d translate",obj->x,-obj->y - obj->h);
poststr(t);
sprintf(t,"%d %d scale\n0 setgray",obj->w,obj->h);
poststr(t);
sprintf(t,"%d %d true [%d 0 0 %d 0 %d]",
   g->usew,g->useh,g->usew,-g->useh,g->useh);
poststr(t);
     
/* poststr("{(%stdin) (r) file picstr readhexstring pop}"); */
     
poststr("{currentfile picstr readhexstring pop}");
poststr("imagemask");
     
bytesperrow=RASSIZE(g->w,1);
off=RASSIZE(g->w,g->usey);
j=0;
     
for (i=0; i < g->useh; i++) {
   for (k=0 ; k < ((g->usew+7)>>3) ; k++) {
      temp=*(g->planes[0]+off+i*bytesperrow+k+(g->usex>>3));
      t[1]=hex[temp&0xF];
      temp = temp >> 4;
      t[0]=hex[temp&0xF];
      t[2]=0;
      poststr2(t);
     
      if (j++ > 35) {
         j=0;
         poststr("");
         }
      }
   if (checkcan())
      break;
   }
     
poststr("\ngrestore");
     
return(AOK);
}

/******************************************/
/*** PostScript text rendering function ***/
/******************************************/
     
ps_dotext(obj)
     
struct object *obj;
     
{
char t[70];
     
if (obj->flags & INV)            /** render in white not black **/
   poststr("1 setgray");
     
if (obj->flags & TOP) {          /** clear area size of box before draw **/
   poststr("newpath");
   poststr("1 setgray");
   sprintf(t,"%d %d moveto",obj->x, -obj->y);
   poststr(t);
   sprintf(t,"%d %d rlineto",0,-obj->h);
   poststr(t);
   sprintf(t,"%d %d rlineto",obj->w,0);
   poststr(t);
   sprintf(t,"%d %d rlineto",0,obj->h);
   poststr(t);
   sprintf(t,"closepath\nfill\n0 setgray\n0 0 moveto");
   poststr(t);
   }
     
draw_text(obj,0,0,0,0,0, ctolplong(1<<16, obj->dpi),
                         ctolplong(1<<16, obj->dpi));
   poststr("0 setgray");
}
     
/*************************************/
/*** PostScrip gray block function ***/
/*************************************/
     
int ps_doblock(obj)
     
struct object *obj;
     
{
char t[65];
     
poststr("newpath");
     
sprintf(t,"%d %d moveto",obj->x,-obj->y);
poststr(t);
sprintf(t,"%d %d rlineto",0,-obj->h);
poststr(t);
sprintf(t,"%d %d rlineto",obj->w,0);
poststr(t);
sprintf(t,"%d %d rlineto",0,obj->h);
poststr(t);
     
poststr("closepath");
if ( obj->density == 0) {
   poststr("1 setgray");
   }
else {
   sprintf(t,".%d setgray",100 - (int)obj->density);
   poststr(t);
   }
poststr("fill\n0 setgray\n0 0 moveto");
}

