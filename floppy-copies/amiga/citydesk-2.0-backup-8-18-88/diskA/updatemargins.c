#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"


extern struct pageinfo defaultpage;


/********************/
/** change margins **/
/********************/

updatemargins(screen,page)

struct pageinfo *page;
struct Screen *screen;

{
#if GERMAN
static char *margins[]={"Rechter Papierrand:","Linker Papierrand:",
             "Oberer Papierrand:",
             "Unterer Papierrand:","Anzahl der Spalten:",
             "Spaltenabstand:",
             "Spaltenlinie?","Seiten-Umrandung?",""};
#else
static char *margins[]={"Right Margin:","Left Margin:","Top Margin:",
             "Bottom Margin:","Num of Columns:","Column Gutter:",
             "Draw Rules?","Outline Page?",""};
#endif

static char *m[8];
float val;
int i;

for (i=0;i<8;i++) {
   m[i]=(char *)AllocMem(61,MEMF_CLEAR);
   if (m[i]==0) {
      while (--i>=0)
         FreeMem(m[i],61);
      return(FALSE);
      }
   }   

val=((float)page->rm)/576.0;
sprintf(m[0],"%6.3f",val);

val=(float)page->lm/576.0;
sprintf(m[1],"%6.3f",val);

val=(float)page->tm/576.0;
sprintf(m[2],"%6.3f",val);

val=(float)page->bm/576.0;
sprintf(m[3],"%6.3f",val);

val=(float)page->nc;
sprintf(m[4],"%2.0f",val);

val=(float)page->cg/576.0;
sprintf(m[5],"%6.3f",val);

if (page->flags & DRAWRULES)
   sprintf(m[6],"YES");
else
   sprintf(m[6],"NO");

if (page->flags & OUTLINEPAGE)
   sprintf(m[7],"YES");
else
   sprintf(m[7],"NO");

if (page == &defaultpage)     /** are we altering the default page? **/
#if GERMAN
   itemlist(screen,margins,m,"standard Seiten-Format:");
#else
   itemlist(screen,margins,m,"Default Page Format:");
#endif
else
#if GERMAN
   itemlist(screen,margins,m,"editiere jetziges Seiten-Format:");
#else
   itemlist(screen,margins,m,"Edit Current Page Format:");
#endif

page->rm=convert_stringi(m[0]);
page->lm=convert_stringi(m[1]);
page->tm=convert_stringi(m[2]);
page->bm=convert_stringi(m[3]);
sscanf(m[4],"%d",&i);
page->nc=(SHORT)i;

if (page->nc < 1) page->nc=1;
if (page->nc > 20) page->nc=20;

page->cg=convert_stringi(m[5]);

for (i=0; (m[6][i]==' ' && i<10); i++) ;
if (m[6][i]=='Y' || m[6][i]=='y')
   page->flags |= DRAWRULES;
else
   page->flags &= (~DRAWRULES);

for (i=0; (m[7][i]==' ' && i<10); i++) ;
if (m[7][i]=='Y' || m[7][i]=='y')
   page->flags |= OUTLINEPAGE;
else
   page->flags &= (~OUTLINEPAGE);

for (i=0;i<8;i++)
   FreeMem(m[i],61);

return(TRUE);
}

convert_stringi(s)
char *s;

{
char *cp;
int i;

cp=s;
i=get_num(&cp , NUM_DEF_INCH);
if (i < 0) i=0;
if (i > 10000) i = 576;    /** arbritary max check **/
return(i);
}

