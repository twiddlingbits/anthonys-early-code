#include <exec/types.h>
#include <exec/memory.h>
#include "/include/psound.h"

/*##
   NAME
      add_module - Create Module from NewModule, link to studioBase

   ARGS:
      1 pointer to a NewModule structure
   RETURNS:
      returns NULL if an error occured allocating memory, else
      returns a pointer to the new module struct
   FUNCTION:
      allocate memory for a new moudle struct and links it into the
      studioBase->first_module chain.  All data from NewModule is
      copied, including the name, so that NewModule may be
      discarded. If the module has an init_module specified, it is
      called.  If the flag, MOD_OPEN_AT_STARTUP is set, the modules
      open_window or OpenWindow() is called.
##*/


struct Module *add_module(nm)

struct NewModule *nm;
{
extern struct studioBase *studioBase;
char *name;
struct Module *create_module();
struct Module *new;
struct Module *p;

if (nm->name) {
   name = (char *)AllocMem(strlen(nm->name)+1,0);
   if (name==0) return(0);
   strcpy(name,nm->name);
   }
else {
   name = 0;
   }

new = create_module();
if (new) {
   new->init_module = nm->init_module;
   new->open_window = nm->open_window;
   new->do_event = nm->do_event;
   new->close_window = nm->close_window;
   new->cleanup_module = nm->cleanup_module;
   new->flags = nm->flags;
   new->name = name;
   p = studioBase->first_module;
   if (p==0) {
      studioBase->first_module = new;
      }
   else {
      while (p->next) p=p->next;
      p->next = new;
      }
   if (new->init_module)
      (*new->init_module)(new);
   if (new->flags&MOD_OPEN_AT_STARTUP)
      (*new->open_window)(new);
   }
return(new);
}

struct Module *create_module()
{
struct Module *m;

m = (struct Module *)AllocMem(sizeof(struct Module), MEMF_CLEAR);
return(m);
}

/*##
   NAME
      delete_module - unlink a struct Module and free it's memory

   ARGS:
      1  pointer to struct Module
   RETURNS:
      none
   FUNCTION:
      deallocates a module's memory and removes it from the module
      list.  The passed module must be linked using its next & prev
      parameters. If this module has a cleanup_module specified,
      it is called.  If a close_window call is specified, it is
      called, otherwise CloseWindow() is called.
##*/

void delete_module(m)
struct Module *m;
{
if (m->cleanup_module)
   (*m->cleanup_module)(m);

if (m->name)
   FreeMem(m->name, strlen(m->name)+1);

if (m->window) {
   if (m->close_window)
      (*m->close_window)(m);
   else
      CloseWindow(m->window);
   }

if (m->prev) {
   m->prev->next = m->next;
   m->next->prev = m->prev;
   }
else {
   studioBase->first_module = m->next;
   if (m->next)
      m->next->prev = 0;
   }

FreeMem(m,sizeof(struct Module));
}

/*##
   NAME
      find_module - given a name of a module, return a pointer
   ARGS:
      1 pointer to name to search for
   RETURN:
      pointer to struct Module or NULL
   FUNCTION:
      searchs the module list for a module with the passed name.
      Treats upper and lower case the same.
##*/

struct Module *find_module(name)
char *name;    /* module name to search for **/
{
extern struct studioBase *studioBase;
struct Module *m;

if (name==0)
   return(0);

for (m=studioBase->first_module; m; m=m->next)
   if (m->name)
      if (stricmp(m->name,name)==0)
         return(m);
return(0);
}


