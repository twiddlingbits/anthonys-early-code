#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )

#define CDID MakeID('C','T','Y','D')   /** City Desk Project ID **/
#define OBJT MakeID('O','B','J','T')   /** Object follows ***/
#define GRAF MakeID('G','R','A','F')   /** Graphic obj data follows **/
#define RAWD MakeID('R','A','W','D')   /** Grap. Obj. image data follows **/
#define ROBJ MakeID('R','U','L','E')   /** Rule object follows **/
#define TXTC MakeID('T','X','T','C')   /** Text Chain follows **/
#define TOBJ MakeID('T','E','X','T')   /** First. Text object follows **/
#define PAGE MakeID('P','A','G','E')   /** page info follows **/
#define TXTL MakeID('T','X','T','L')   /** Text line structure **/
#define EOTL MakeID('E','O','T','L')   /** end of Text line structure **/

#define ARRAY_SIZE 1600

extern UWORD xinch, yinch;
extern struct printerprefs printerprefs;
extern struct text_state defaultstate;

char fname[34]={0};
char drawer[35]={0};

static long *old_ptr;
static long *new_ptr;
static unsigned short ptr_kount;
static short offset;    /** used when appending projects **/

saveall(saveas)

int  saveas;              /** TRUE if we should get fname **/

{
extern struct Window *clipboard;
extern struct pageinfo page[];
char path[80];
int cancel;
int fd;

if (saveas) {
#if GERMAN
   cancel=get_fname(clipboard,clipboard->WScreen,
      "Projekt sichern als...",fname,drawer);
#else
   cancel=get_fname(clipboard,clipboard->WScreen,
      "Save Project As...",fname,drawer);
#endif
   if (cancel==NULL) return(CANCEL);
}

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

fd=creat(path,0);
if (fd==-1) return(CANTOPENFILE);

putid(fd,CDID);
write(fd,&printerprefs.printertype,2);
write (fd,&xinch,2);         /** X printer resolution **/
write (fd,&yinch,2);         /** Y printer resolution **/
write (fd,&xinch,2);         /** take up 4 bytes to remain **/
write (fd,&yinch,2);         /** compattible with last version **/
cancel=savepageinfo(fd,0,MAXPAGES);
if (cancel==AOK) {
   cancel=saveobjects(fd,0,MAXPAGES);
   unghost_save();
   }

close(fd);
return(cancel);
}

/*******************************************************/
/** Save default even & odd page only for 'autoprefs' **/
/*******************************************************/

save_default(fd)

int fd;

{
extern struct Window *clipboard;
extern struct pageinfo page[];
int cancel;

putid(fd,CDID);
write(fd,&printerprefs.printertype,2);
write (fd,&xinch,2);         /** X printer resolution **/
write (fd,&yinch,2);         /** Y printer resolution **/
write (fd,&xinch,2);         /** X printer resolution **/
write (fd,&yinch,2);         /** Y printer resolution **/
cancel=savepageinfo(fd, DEFAULTODD, DEFAULTEVEN+1);
if (cancel==AOK) {
   cancel=saveobjects(fd, DEFAULTODD, DEFAULTEVEN+1);
   unghost_save();
   }
return(CANCEL);
}

/*************************/
/** load entire project **/
/*************************/

loadall(append)

int append;       /** TRUE means append insted of load **/

{
extern struct Window *clipboard;
extern struct pageinfo page[];
char path[80];
int cancel;
int fd,i;
ULONG temp;
UWORD xcpp,ycpp;
WORD ptype;

#if GERMAN
cancel=get_fname(clipboard,clipboard->WScreen,"Projekt einlesen",fname,drawer);

#else

if (append)
   cancel=get_fname(clipboard,clipboard->WScreen,"Append Project",fname,drawer);
else 
   cancel=get_fname(clipboard,clipboard->WScreen,"Load Project",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

fd=open(path,0);
if (fd==-1) {
   return(FILENOTFOUND);
   }

read (fd,&temp,4);

if (temp != CDID) {
   close(fd);
   return (NOTCDPROJECT);
   }

read(fd,&ptype,2);         /** read printertype **/
read (fd,&xcpp,2);         /** read xinch **/
read (fd,&ycpp,2);         /** read yinch **/
read (fd,&xcpp,2);         /** read xinch **/
read (fd,&ycpp,2);         /** read yinch **/

if (ptype != printerprefs.printertype ||
      xcpp != xinch || ycpp != yinch) {
   i=telluser(clipboard->WScreen,"File created with different resolution.",
         "Continue Load?");
   if (!i) {
      close(fd);
      return(CANCEL);
      }
   }

/* newpointer(clipboard->WScreen,BUSY); */

if (!append) {
   for (i=1; i<MAXPAGES; i++) {
      delete_page(i);
      }
   }

i=loadpageinfo(fd,append);
if (i==AOK) {
   i = alloc_ptr_adjust();
   if (i == AOK) {
      i = loadobjects(fd,append);
      unghost_save();
      correct_ptrs(append);      /** alters top_link, next, first **/
      free_ptr_adjust();
      }
   }
close(fd);
return(i);
}

/**********************************************************/
/** load default even & odd page for use by 'load prefs' **/
/**********************************************************/

load_default(fd)

int fd;

{
extern struct Window *clipboard;
extern struct pageinfo page[];
int i;
ULONG temp;
UWORD xcpp,ycpp;
WORD ptype;

read (fd, &temp, 4);

if (temp != CDID) {
   close(fd);
   return (NOTCDPROJECT);
   }

read (fd,&ptype,2);        /** read printertype **/
read (fd,&xcpp,2);         /** read xinch **/
read (fd,&ycpp,2);         /** read yinch **/
read (fd,&xcpp,2);         /** read xinch **/
read (fd,&ycpp,2);         /** read yinch **/

#if 0
if (ptype != printerprefs.printertype ||
      xcpp != xinch || ycpp != yinch) {
   i=telluser(clipboard->WScreen,"File created with different resolution.",
         "Continue Load?");
   if (!i) {
      close(fd);
      return(CANCEL);
      }
   }
#endif

delete_page(DEFAULTEVEN);
delete_page(DEFAULTODD);

i=loadpageinfo(fd, FALSE);
if (i==AOK) {
   i = alloc_ptr_adjust();
   if (i == AOK) {
      i = loadobjects(fd, FALSE);
      correct_default_ptrs();      /** alters top_link, next, first **/
      free_ptr_adjust();
      }
   }
return(i);
}

/*******************************************/
/*** save all of the paginfo structures ****/
/*******************************************/

savepageinfo(fd, start_page, end_page)

int fd;
int start_page, end_page;

{
extern struct pageinfo page[MAXPAGES];
LONG i,k;

putid(fd,PAGE);

k=0;
for (i=start_page; i < end_page; i++)
   if (page[i].flags&PAGEINUSE)
      k++;

if (k==0)
   return(CANCEL);

write(fd,&k,4);

for (i=start_page; i < end_page; i++)  {
   if (page[i].flags&PAGEINUSE)   {
      if (write (fd,&page[i],sizeof(struct pageinfo)) !=
          sizeof(struct pageinfo))
         return(DISKERROR);
      }
   }
return(AOK);
}


/*******************************************/
/*** load all of the paginfo structures ****/
/*******************************************/

loadpageinfo(fd,append)

int fd;
int append; /** TRUE means we are appending **/
{
extern struct pageinfo page[MAXPAGES];
struct pageinfo tp;
LONG i,k;

if (append)
   offset = last_page();
else
   offset = 0;

read(fd,&i,4);
if (i!=PAGE)
   return(BADCD);

if (read(fd,&k,4)!=4)
   return(DISKERROR);

if (k > MAXPAGES)
   return(BADCD);

for (i=0; i < k ; i++)  {
   if (read (fd,&tp,sizeof(struct pageinfo))!=sizeof(struct pageinfo))
      return(BADCD);
   if (tp.pn >= MAXPAGES)
      return(BADCD);
   if (!(append && tp.pn > LASTPAGE)) {/** if appending don't load defaut pgs **/
      copypage(&tp,&page[tp.pn + offset]);
      }
   }
return(AOK);
}

/*****************************/
/*** save all them objects ***/
/*****************************/

saveobjects(fd, start_page, end_page)

int fd;
int start_page, end_page;

{
extern struct pageinfo page[MAXPAGES];
LONG i;
int stat;

for (i=start_page; i < end_page; i++)  {
   if (page[i].flags&PAGEINUSE)   {
 /* printf("gonna save page %d\n",i); */
      stat=savepage(fd,&page[i]);
      if (stat!=AOK)
         return(stat);
      }
   }
return(AOK);
}

/*******************/
/** save one page **/
/*******************/

savepage(fd,page)

int fd;
struct pageinfo *page;

{
struct Node *node;
struct object *obj;
struct graphic *g;
struct text_object *t;
ULONG len;
LONG i;
extern struct Window *clipboard;


for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
/* printf("on object %x\n",obj); */
   if (!(obj->type ==TEXT && obj->first != obj)) {
      putid(fd,OBJT);
      putid(fd,obj);    /** write object address **/
      if (write (fd,obj,sizeof (struct object))!=sizeof(struct object))
         return(DISKERROR);
      }

   if (obj->type==GRAPHIC) {
      putid(fd,GRAF);
      write(fd,obj->data,sizeof(struct graphic));
      putid(fd,RAWD);
      g=(struct graphic *)obj->data;
      len = RASSIZE(g->w,g->h);
      for (i=0; i < g->numplanes; i++)
         if (write(fd, g->planes[i], len) != len)
            return(DISKERROR);
      }

   if (obj->type==RULE) {
      putid(fd,ROBJ);
      write(fd,obj->data,sizeof(struct rule));
      }

   if (obj->type==TEXT && obj->first==obj) {
      putid(fd,TOBJ);
      len=strlen(tobj_text(obj))+1;
      write(fd,&len,4);                /** write length of text **/
      if (write(fd,tobj_text(obj),len)!=len)      /** write actual text **/
         return(DISKERROR);

      t=(struct text_object *)obj->data;
      savetextobj(fd,t);
      if (obj->flags&IRREGULAR)
         save_text_lines(fd,t);

      while (t->next != 0) {     /** write a chain **/
         putid(fd,OBJT);
         putid(fd,obj);
         obj=t->next;
         write (fd,obj,sizeof (struct object));
         t=(struct text_object *)obj->data;
         putid(fd,TXTC);         /** text chain id **/
         write(fd,t,sizeof(struct text_object));
         if (obj->flags&IRREGULAR)
            save_text_lines(fd,t);
         }
      }
   }
return (AOK);
}

/*****************************/
/*** load all them objects ***/
/*****************************/

loadobjects(fd,append)

int fd;
int append;    /** TRUE if objs should be append to the end **/

{
extern struct pageinfo page[MAXPAGES];
struct object *obj,*prev,*createobject();
struct graphic *g;
struct text_object *t;
ULONG len;
USHORT font_len,i;
char fontname[40];
char *text;
ULONG id;
struct object tobj;
struct macro_text *mt;

prev=0;     /** get rid of compiler warning **/

while (TRUE) {
   len=read(fd,&id,4);
   if (len!=4)
      return(AOK);

   /* printf("got obj %c%c%c%c\n", (id>>24)&0xFF,(id>>16)&0xFF,
            (id>>8)&0xFF,id&0xFF); */

   len=read(fd,&id,4);        /** this objects ptr address **/
      if (len!=4)
         return(BADCD);

   len=read(fd,&tobj,sizeof(struct object));    /** read next object **/
   if (len != sizeof(struct object))
      return(BADCD);

   if (tobj.pn < 0 || tobj.pn >= MAXPAGES)
      return(BADCD);

   if (append && tobj.pn >= DEFAULTODD)
      return(AOK);

   tobj.pn += offset;

   obj=createobject(&page[tobj.pn].objects);
   if (obj==0)
      return (OUTOFMEM);
   add_ptr(id, obj);
   copyobj(&tobj,obj);
   obj->flags |= JUST_LOADED;             /** see correct_ptrs **/
   obj->mag=1;

   if (obj->type==GRAPHIC) {              /** read graphic data **/
      read(fd,&id,4);
      if (id!=GRAF)  {
         Remove(obj);
         FreeMem(obj,sizeof(struct object));
         return(BADCD);
         }

      obj->data=(APTR)AllocMem(sizeof(struct graphic),0);
      if (obj->data==0) {
         Remove(obj);
         FreeMem(obj,sizeof(struct object));
         return(OUTOFMEM);
         }

      read(fd,obj->data,sizeof(struct graphic));
      g=(struct graphic *)obj->data;

      read(fd,&id,4);         /** image type **/
      if (id !=RAWD) {
         FreeMem(g,sizeof(struct graphic));
         Remove(obj);
         FreeMem(obj,sizeof(struct object));
         return(BADCD);
         }

      len = RASSIZE(g->w, g->h);
/* printf("reading graphic len %d numplanes \n",len,g->numplanes); */
      for (i=0; i < g->numplanes; i++) {
         g->planes[i]=(UBYTE *)AllocMem(len, 0);
         if (g->planes[i]==0) {
            FreeMem(g,sizeof(struct graphic));
            Remove(obj);
            FreeMem(obj,sizeof(struct object));
            return(OUTOFMEM);
            }
         read(fd, g->planes[i], len);
         }
      }  /** close graphic loop **/

   if (obj->type==RULE) {
      read(fd,&id,4);
      if (id!=ROBJ)  {
         Remove(&obj->node);
         FreeMem(obj,sizeof(struct object));
         return(BADCD);
         }

      obj->data=(APTR)AllocMem(sizeof(struct rule),0);
      if (obj->data==0) {
         Remove(&obj->node);
         FreeMem(obj,sizeof(struct object));
         return(OUTOFMEM);
         }

      read(fd,obj->data,sizeof(struct rule));
      }

   if (obj->type==TEXT) {
      read(fd,&id,4);
      if (id!=TOBJ && id!=TXTC) {
         Remove(&obj->node);
         FreeMem(obj,sizeof(struct object));
         return(BADCD);
         }

      if (id==TOBJ) {
         read(fd,&len,4);
         text=(char *)AllocMem(len,0);
         if (text==0) {
            Remove(&obj->node);
            FreeMem(obj,sizeof(struct object));
            return(OUTOFMEM);
            }
         read(fd,text,len);
         }

      t=(struct text_object *)AllocMem(sizeof(struct text_object),0);
      if (t==0) {
         FreeMem(text,len);
         Remove(&obj->node);
         FreeMem(obj,sizeof(struct object));
         return(OUTOFMEM);
         }

      read(fd,t,sizeof(struct text_object));
      t->line=0;
      t->next=0;
      t->endmacro=0;
      t->state.objptr = obj;
      t->endstate.objptr = obj;   /** probably not need, but just be safe */
      t->state.savefont = 0;
      t->endstate.savefont = 0;   /** probably not needed **/

      obj->data=(APTR)t;
      if (id==TOBJ) {         /** first text object **/
         mt = AllocMem(sizeof(struct macro_text), MEMF_CLEAR);
         if (mt==0) {
            FreeMem(text,len);
            Remove(&obj->node);
            FreeMem(obj,sizeof(struct object));
            FreeMem(t, sizeof(struct text_object));
            return(OUTOFMEM);
            }

         mt->start = text;
         mt->end = text + strlen(text)+1;
         mt->flags = MTEXT_FREEMTEXT|MTEXT_FREETEXT;  /** auto free on **/
         t->macro = (struct macro *)start_macro(0, mt, 0);

         read(fd,&font_len,2);
         read(fd,fontname,font_len);
         t->state.lfont = NULL;
         t->state.font = NULL;
         my_open_font(fontname, &t->state, NULL);
         if (t->state.font == NULL) {
            t->state.font = defaultstate.font;
            }
         if (obj->flags&IRREGULAR)
            if (load_text_lines(fd,t)!=AOK)
               return(OUTOFMEM);
         }
      else {
         t->macro = copy_macro_chain(
                    ((struct text_object*)(prev->data))->endmacro);
         t->state = ((struct text_object *)(prev->data))->endstate;
         ((struct text_object *)(prev->data))->next=obj;
         obj->first=prev->first;
         if (obj->flags&IRREGULAR)
            if (load_text_lines(fd,t)!=AOK)
               return(OUTOFMEM);
         }
      if ((obj->flags&IRREGULAR)==0)
         format(obj,FALSE);
      }
   prev=obj;            /** remember previoust object **/
   }  /** close while (TRUE) .... **/
}  /** end of subroutine **/


/*****************************************/
/*** write a 4 byte id to disk file fd ***/
/*****************************************/

putid(fd,id)

int fd;
ULONG id;

{
write(fd,&id,4);
}

/**************************************/
/*** Copy object to another object ****/
/**************************************/

copyobj(s,d)

struct object *s,*d;

{
d->numplanes  = s->numplanes;
d->type       = s->type;
d->flags      = s->flags;
d->x          = s->x;
d->y          = s->y;
d->h          = s->h;
d->w          = s->w;
d->angle      = s->angle;
d->dpi        = s->dpi;
d->density    = s->density;

d->top_link   = s->top_link;
d->next       = s->next;
d->first      = s->first;
d->pn         = s->pn;
}

savetextobj(fd,t)

int fd;
struct text_object *t;

{
WORD len;

write(fd,t,sizeof(struct text_object));

if (t->state.lfont) {   /** get fontname and size **/
   len=strlen(t->state.lfont->fontname)+1;
   write(fd,&len,2);
   write(fd,t->state.lfont->fontname,len);
   }

else {
   len=strlen(t->state.font->tf_Message.mn_Node.ln_Name)+1;
   write(fd,&len,2);
   write(fd,t->state.font->tf_Message.mn_Node.ln_Name,len);
   }
}

save_text_lines(fd,t)

int fd;
struct text_object *t;

{
struct text_line *tlp;

/* printf("entered save text lines\n"); */

tlp = t->line;

while (tlp) {
   putid(fd,TXTL);
   write(fd,tlp,sizeof(struct text_line));
   tlp = tlp->next;
   }
putid(fd,EOTL);
}

load_text_lines(fd,t)

int fd;
struct text_object *t;

{
struct text_line *tlp, *tlp_prev;
ULONG id;

/* printf("entered load text lines\n"); */

tlp_prev = 0;
read(fd,&id,4);

while (id==TXTL) {
   tlp = AllocMem(sizeof(struct text_line),0);
   if (tlp==0) {
      return(OUTOFMEM);
      }
   if (tlp_prev==0)     /** first pass **/
      t->line = tlp;
   else
      tlp_prev->next = tlp;
   read(fd, tlp, sizeof(struct text_line));
   tlp_prev = tlp;
   read(fd,&id,4);
   }
process_text_lines(t);
return(AOK);
}

alloc_ptr_adjust()
{
extern long *old_ptr;
extern long *new_ptr;
extern unsigned short ptr_kount;

ptr_kount = 0;

old_ptr = (long *)AllocMem(ARRAY_SIZE, 0);
if (old_ptr==0)
   return(OUTOFMEM);

new_ptr = (long *)AllocMem(ARRAY_SIZE, 0);
if (new_ptr==0) {
   FreeMem(old_ptr, ARRAY_SIZE);
   return(OUTOFMEM);
   }

return(AOK);
}

free_ptr_adjust()
{
FreeMem(old_ptr, ARRAY_SIZE);
FreeMem(new_ptr, ARRAY_SIZE);
return(AOK);
}

add_ptr(old, new)

long old, new;
{
extern unsigned short ptr_kount;

if (ptr_kount >= ARRAY_SIZE/4)
   return(OUTOFMEM);

new_ptr[ptr_kount] = new;
old_ptr[ptr_kount] = old;
ptr_kount++;
return(AOK);
}

find_new_ptr(old)

long old;

{
unsigned short i;
extern unsigned short ptr_kount;

if (old==0)
   return(0);

for (i=0; i < ptr_kount; i++)
   if (old_ptr[i] == old)
      return(new_ptr[i]);

/* printf("savepage: can't find pointer!\n"); */
return(0);
}

correct_ptrs(append)
int append;
{
extern struct pageinfo page[MAXPAGES];
unsigned short pg;
struct object *obj;
struct Node *node;
short num_pages, start_page;

num_pages = MAXPAGES;
start_page = 0;

if (append) {
   num_pages = num_pages-3;
   start_page = offset+1;
   }

for (pg=start_page; pg < num_pages; pg++) {
   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;

      if (obj->flags & JUST_LOADED) {
         obj->flags &= (~JUST_LOADED);
         obj->top_link = (struct object *)find_new_ptr(obj->top_link);
         obj->next  =    (struct object *)find_new_ptr(obj->next);
         obj->first =    (struct object *)find_new_ptr(obj->first);
         if (obj->top_link) {
            if (obj->top_link->next==0) {
               obj->top_link = 0;
               obj->next = 0;
               /* printf("fixed error on load\n"); */
               }
            }
         }
      }
   }
return(AOK);
}

correct_default_ptrs()
{
extern struct pageinfo page[MAXPAGES];
unsigned short pg;
struct object *obj;
struct Node *node;
short num_pages;

for (pg=DEFAULTODD; pg < MAXPAGES; pg++) {
   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;
      if (obj->flags & JUST_LOADED) {
         obj->flags &= (~JUST_LOADED);
         obj->top_link = (struct object *)find_new_ptr(obj->top_link);
         obj->next  =    (struct object *)find_new_ptr(obj->next);
         obj->first =    (struct object *)find_new_ptr(obj->first);
         if (obj->top_link) {
            if (obj->top_link->next==0) {
               obj->top_link = 0;
               obj->next = 0;
               /* printf("def: fixed error on load\n"); */
               }
            }
         }
      }
   }
return(AOK);
}

