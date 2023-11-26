#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )

#define CDID MakeID('C','T','Y','C')   /** City Desk Project ID **/
#define OBJT MakeID('O','B','J','T')   /** Object follows ***/
#define GRAF MakeID('G','R','A','F')   /** Graphic obj data follows **/
#define RAWD MakeID('R','A','W','D')   /** Grap. Obj. image data follows **/
#define ROBJ MakeID('R','U','L','E')   /** Rule object follows **/
#define TXTC MakeID('T','X','T','C')   /** Text Chain follows **/
#define TOBJ MakeID('T','E','X','T')   /** First. Text object follows **/
#define PAGE MakeID('P','A','G','E')   /** page info follows **/

#define ARRAY_SIZE 800

extern FIXED cpertextpixx,cpertextpixy;
extern struct printerprefs printerprefs;
extern struct text_state defaultstate;

char fname[34]={0};
char drawer[35]={0};

static long *old_ptr;
static long *new_ptr;
static unsigned short ptr_kount;

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
write (fd,&cpertextpixx,4);         /** X printer resolution **/
write (fd,&cpertextpixy,4);         /** Y printer resolution **/
cancel=savepageinfo(fd);
if (cancel==AOK) {
   cancel=saveobjects(fd);
   unghost_save();
   }

close(fd);
return(cancel);
}

/*************************/
/** load entire project **/
/*************************/

loadall()

{
extern struct Window *clipboard;
extern struct pageinfo page[];
char path[80];
int cancel;
int fd,i;
ULONG temp;
FIXED xcpp,ycpp;
WORD ptype;

#if GERMAN
cancel=get_fname(clipboard,clipboard->WScreen,"Projekt einlesen",fname,drawer);

#else

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
read (fd,&xcpp,4);         /** read cpertextpixx **/
read (fd,&ycpp,4);         /** read cpertextpixy **/

if (ptype != printerprefs.printertype ||
      xcpp != cpertextpixx || ycpp != cpertextpixy) {
   i=telluser(clipboard->WScreen,"File created with different printer.",
         "Continue Load?");
   if (!i) {
      close(fd);
      return(CANCEL);
      }
   }

newpointer(clipboard->WScreen,BUSY);

for (i=1; i<MAXPAGES; i++) {
   delete_page(i);
   }

i=loadpageinfo(fd);
if (i==AOK) {
   i = alloc_ptr_adjust();
   if (i == AOK) {
      i = loadobjects(fd);
      unghost_save();
      correct_ptrs();      /** alters top_link, next, first **/
      free_ptr_adjust();
      }
   }
close(fd);
return(i);
}

/*******************************************/
/*** save all of the paginfo structures ****/
/*******************************************/

savepageinfo(fd)

int fd;

{
extern struct pageinfo page[MAXPAGES];
LONG i,k;

putid(fd,PAGE);

k=0;
for (i=0; i < MAXPAGES; i++)
   if (page[i].flags&PAGEINUSE)
      k++;

if (k==0)
   return(CANCEL);

write(fd,&k,4);

for (i=0; i < MAXPAGES; i++)  {
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

loadpageinfo(fd)

int fd;

{
extern struct pageinfo page[MAXPAGES];
struct pageinfo tp;
LONG i,k;

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
   copypage(&tp,&page[tp.pn]);
   }
return(AOK);
}

/*****************************/
/*** save all them objects ***/
/*****************************/

saveobjects(fd)

int fd;

{
extern struct pageinfo page[MAXPAGES];
LONG i;
int stat;

for (i=0; i < MAXPAGES; i++)  {
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

      while (t->next != 0) {     /** write a chain **/
         putid(fd,OBJT);
         putid(fd,obj);
         obj=t->next;
         write (fd,obj,sizeof (struct object));
         t=(struct text_object *)obj->data;
         putid(fd,TXTC);         /** text chain id **/
         write(fd,t,sizeof(struct text_object));
         }

      }
   }
return (AOK);
}

/*****************************/
/*** load all them objects ***/
/*****************************/

loadobjects(fd)

int fd;

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

   len=read(fd,&id,4);
      if (len!=4)
         return(BADCD);

   len=read(fd,&tobj,sizeof(struct object));    /** read next object **/
   if (len != sizeof(struct object))
      return(BADCD);

   if (tobj.pn < 0 || tobj.pn >= MAXPAGES)
      return(BADCD);

   obj=createobject(&page[tobj.pn].objects);
   if (obj==0)
      return (OUTOFMEM);
   add_ptr(id, obj);
   obj->mag=1;
   obj->pn=tobj.pn;
   obj->first=obj;
   copyobj(&tobj,obj);

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
         }
      else {
         t->macro = copy_macro_chain(
                    ((struct text_object*)(prev->data))->endmacro);
         t->state = ((struct text_object *)(prev->data))->endstate;
         ((struct text_object *)(prev->data))->next=obj;
         obj->first=prev->first;
         }
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
if (new_ptr==0)
   return(OUTOFMEM);

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

for (i=0; i < ptr_kount; i++)
   if (old_ptr[i] == old)
      return(new_ptr[i]);

/* printf("savepage: can't find pointer!\n"); */
return(0);
}

correct_ptrs()
{
extern struct pageinfo page[MAXPAGES];
unsigned short pg;
struct object *obj;
struct Node *node;

for (pg=0; pg < MAXPAGES; pg++) {
   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;
      obj->top_link = (struct object *)find_new_ptr(obj->top_link);
      obj->next  =    (struct object *)find_new_ptr(obj->next);
      obj->first =    (struct object *)find_new_ptr(obj->first);
      }
   }
return(AOK);
}

