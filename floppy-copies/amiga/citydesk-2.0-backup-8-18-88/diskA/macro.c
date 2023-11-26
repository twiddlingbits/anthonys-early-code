/*###################################################################*/
/* macro.c                                                           */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

extern SHORT texterrors;
extern struct Window *clipboard;

/*
Macros are controlled by two structures, macro and macro_text.

Macro_text points to a piece of text, for example the main text, the
replacement text for a macro, a temporary string for the date, etc.
There is one of these for each piece of text in the system.  The
ref_count field counts the number of macro structure that point at this
macro_text.  When the ref_count reaches zero, the macro_text structure
is deallocated, and the text it points to is disposed of.  This could
mean deallocating the memory if it's a temporary string, or doing nothing
if it's a macro expansion.

The macro structure controls one instance of a macro_text.  They are chained
together backwards, i.e. each one points to it's parent.  The macro pointer
in the text_state structure points to the lowest level of the expansion.
For example, the text_state might point at the structure controlling the
date, which points to a macro that included the date, which points to the
macro that controls the main text.

Often, we need to save the entire text state so we can restart later in
the middle.  This occurs anytime the text is split over two blocks, and
it occurs many times during the format process (so we can back up words
that don't fit on the line).  To do this, the macro chain is copied.
The reference count is incremented for each macro_text pointed to so
that things don't get deallocated out from under us!  Then when the copy
is no longer needed, the chain is deallocated and all reference_counts
are decremented.

This scheme allows unlimited nesting of macros, as well as stopping and
re-starting the expansion at any point.
*/


/************************************************************************/
/* chain = start_macro(chain, mtext, param);                            */
/*                                                                      */
/* Creates and returns a new, filled-in macro structure given an        */
/* existing macro_text structure and the current top level macro.       */
/************************************************************************/

struct macro *start_macro(chain, mt, param)
struct macro *chain;
struct macro_text *mt;
struct parameter *param;
{
struct macro *m;

m = (struct macro *)AllocMem(sizeof(struct macro), 0L);
if (m == NULL) {
   TEXTERROR("Out of memory", "while processing macros");
   return chain;
}

m->parent = chain;
m->macrotext = mt;
m->current = mt->start;
m->param = param;
if (param != NULL)               /* bump ref count for this param chain */
   param->mtext.ref_count++;     /* so other macro chains can use it    */
m->flags = 0;
mt->ref_count++;

return m;
}


/************************************************************************/
/* chain = end_macro(chain);                                            */
/*                                                                      */
/* Ends a macro level by deallocating the macro structure, and maybe    */
/* it's macro_text structure if the ref_count is 0 and the flags are    */
/* right.  Returns the previous macro up the chain, or NULL if this is  */
/* the last one.                                                        */
/************************************************************************/

struct macro *end_macro(chain)
struct macro *chain;
{
struct macro *m;
struct macro_text *mt;
VOID free_param_list();

if (chain == NULL)
   return NULL;

if (chain->param != NULL)              /* un-reference this param chain */
   chain->param->mtext.ref_count--;    /* (bumped by start_macro() )    */

free_param_list(chain->param);

mt = chain->macrotext;

mt->ref_count--;
if (mt->ref_count <= 0) {
   if (mt->flags & MTEXT_FREETEXT) {
 /* printf("freeeing text!\n"); !!!! */
      FreeMem(mt->start, mt->end - mt->start);
      }
   if (mt->flags & MTEXT_FREEMTEXT)
      FreeMem(mt, sizeof(struct macro_text));
}

m = chain->parent;
FreeMem(chain, sizeof(struct macro));

return m;
}


/************************************************************************/
/* chain = copy_macro_chain(chain);                                     */
/*                                                                      */
/* Copies a macro chain, incrementing ref_count for all macro_text      */
/* structures pointed to.                                               */
/************************************************************************/

struct macro *copy_macro_chain(chain)
struct macro *chain;
{
struct macro *p, *prev, *new, *head;

prev = NULL;
head = NULL;
for (p=chain; p!=NULL; p=p->parent) {
   new = start_macro(p, p->macrotext, p->param);
   new->flags = p->flags;
   new->current = p->current;
   if (prev == NULL)
      head = new;
   else
      prev->parent = new;
   prev = new;
}
prev->parent = NULL;

return head;
}


/************************************************************************/
/* free_macro_chain(chain);                                             */
/*                                                                      */
/* Frees an entire macro chain, maybe disposing of the macro_text       */
/* structures as well.                                                  */
/************************************************************************/

VOID free_macro_chain(chain)
struct macro *chain;
{
struct macro *m;

m = chain;
while (m != NULL)
   m = end_macro(m);
}



/************************************************************************/
/* param = add_param(param, start, end, context);                       */
/*                                                                      */
/* Adds a new parameter pointed to by (start, end) to the parameter     */
/* list, and returns a pointer to the head of the list.  Context points */
/* to the parameter list of the caller, for use if this parameter       */
/* contains a parameter reference.                                      */
/************************************************************************/

struct parameter *add_param(param, start, end, context)
struct parameter *param;
char *start;
char *end;
struct parameter *context;
{
struct parameter *p, *p2;

p = (struct parameter *)AllocMem(sizeof(struct parameter), 0);
if (p == NULL) {
   TEXTERROR("Out of memory", "while processing macros");
   return param;
}
p->mtext.start = start;
p->mtext.end = end;
p->mtext.ref_count = 0;
p->mtext.flags = MTEXT_PARAMETER;
p->next = NULL;
p->context = context;

for (p2 = param; p2 != NULL && p2->next != NULL; p2 = p2->next)
   ;
if (p2 != NULL)
   p2->next = p;

if (param == NULL)
   return p;
else
   return param;
}


/************************************************************************/
/* chain = free_param_list(param);                                      */
/*                                                                      */
/* Frees a list of parameters, if possible.  First, check the           */
/* ref_counts for each parameter.  If any are non-zero, exit without    */
/* doing anything, as a copy of this macro is still open somewhere.     */
/* If all are zero, then free the list.                                 */
/************************************************************************/

VOID free_param_list(param)
struct parameter *param;
{
struct parameter *p, *p2;

for (p=param; p!=NULL; p=p->next)
   if (p->mtext.ref_count > 0)
      return;

for (p = param; p != NULL; p = p2) {   /* all ref_counts 0, so free list */
   p2 = p->next;
   FreeMem(p, sizeof(struct parameter));
}

}

