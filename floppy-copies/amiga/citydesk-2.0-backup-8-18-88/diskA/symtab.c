/*###################################################################*/
/* symtab.c                                                          */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

extern SHORT texterrors;
extern struct Window *clipboard;

/************************************************************************/
/* sym = search_table(table, key);                                      */
/*                                                                      */
/* Searches symbol table 'table' for the entry matching the key 'key'.  */
/* Returns the symbol entry if present, or NULL if not present.         */
/************************************************************************/

struct symbol *search_table(table, string)
struct symtab *table;
char *string;
{
int index;
int min, max;
int res;

min = 0;
max = table->n - 1;

while (max >= min) {
   index = (min+max) / 2;
   res = stricmp(string, table->entries[index].key);
   if (res == 0)              /* match */
      return &table->entries[index];
   if (res < 0)
      max = index-1;
   else
      min = index+1;
}
return NULL;
}


/************************************************************************/
/* delete_symbol(table, key);                                           */
/*                                                                      */
/* Deletes the symbol with the given key from the given table.  Will    */
/* deallocate memory for the key and value if the table->flags are set  */
/* appropriately.                                                       */
/************************************************************************/

VOID delete_symbol(table, string)
struct symtab *table;
char *string;
{
int index, i;
int min, max;
int res;
VOID free_symbol_entry();

min = 0;
max = table->n - 1;

if (!(table->flags & SYM_DYNAMIC)) {
   TEXTERROR("Internal program error!",
             "Attempt to delete symbol from static table");
   return;
}
while (max >= min) {
   index = (min+max) / 2;
   res = stricmp(string, table->entries[index].key);
   if (res == 0) {                        /* match, so delete the entry */
      free_symbol_entry(&table->entries[index], table->flags);
      for (i = index+1; i < table->n; i++)
         table->entries[i-1] = table->entries[i];
      table->n--;
      return;
   }
   if (res < 0)
      max = index-1;
   else
      min = index+1;
}
return;
}


/************************************************************************/
/* add_symbol(table, key, value);                                       */
/*                                                                      */
/* Adds the symbol with the given key to the given table, using the     */
/* given value.  If the symbol already exists, the user may be warned   */
/* if the definition changes.                                           */
/************************************************************************/

VOID add_symbol(table, string, value)
struct symtab *table;
char *string;
ULONG value;
{
struct symbol *new;
int newsize;
int index, i;
int min, max;
int res;
short warn;
struct macro_text *mt1, *mt2;
char text1[80], text2[80];
struct symbol *AllocMem();
VOID free_symbol_entry();

if (!(table->flags & SYM_DYNAMIC)) {
   TEXTERROR("Internal program error!",
             "Attempt to add symbol to static table");
   return;
}
min = 0;
max = table->n - 1;

while (max >= min) {
   index = (min+max) / 2;
   res = stricmp(string, table->entries[index].key);

   if (res == 0) {                  /* match, so just replace the value */

      if (table->flags & SYM_REDEF_WARN) {  /* check to see if replace ok */
         warn = FALSE;
         if (table->flags & SYM_FREEVALUE) { /* means it's a macro_text */
            mt1 = (struct macro_text *)table->entries[index].value;
            mt2 = (struct macro_text *)value;
            if (strncmp(mt1->start, mt2->start, mt1->end - mt1->start) != 0)
               warn = TRUE;
         }
         else {
            if (table->entries[index].value != value)
               warn = TRUE;
         }
         if (warn) {
            sprintf(text1, "Warning: '%s' has been changed", string);
            sprintf(text2, "Please reflow anything that uses '%s'", string);
            TEXTERROR(text1, text2);
         }
      }

      free_symbol_entry(&table->entries[index], table->flags);
      table->entries[index].key = string;
      table->entries[index].value = value;
      return;
   }

   if (res < 0)
      max = index-1;
   else
      min = index+1;
}
if (table->n >= table->tbl_size) {     /* not enough room */
   newsize = table->tbl_size * 2;
   if (newsize == 0)
      newsize = SYMTAB_DEFSIZE;

   new = AllocMem(newsize * sizeof(struct symbol), 0);
   if (new == NULL) {
      newsize = table->tbl_size + 2;
      new = AllocMem(newsize * sizeof(struct symbol), 0);
      if (new == NULL) {
         TEXTERROR("Out of memory", "Can't add new symbol");
         return;
      }
   }
   if (table->entries != NULL) {
      for (i=0; i < table->tbl_size; i++)
         new[i] = table->entries[i];
      FreeMem(table->entries, table->tbl_size * sizeof(struct symbol));
   }
   table->tbl_size = newsize;
   table->entries = new;
}

/* New value goes between max and min (since max < min) */

for (i=table->n; i > min; i--)
   table->entries[i] = table->entries[i-1];

table->entries[min].key = string;
table->entries[min].value = value;

table->n++;

return;
}


/************************************************************************/
/* free_symbol_table(table);                                            */
/*                                                                      */
/* Frees up all the entries and all memory associated with the given    */
/* symbol table.                                                        */
/************************************************************************/

VOID free_symbol_table(table)
struct symtab *table;
{
int i;
VOID free_symbol_entry();

if (table->flags & SYM_DYNAMIC) {
   if (table->entries != NULL)
      for (i=0; i < table->n; i++)
         free_symbol_entry(&table->entries[i], table->flags);
      FreeMem(table->entries, table->tbl_size * sizeof(struct symbol));

   table->tbl_size = 0;
   table->entries = NULL;
}
}


/************************************************************************/
/* free_symbol_entry(symbol, table->flags);                             */
/*                                                                      */
/* Frees up a single symbol table entry.  Flags come from the symbol    */
/* table structure.  If SYM_FREEVALUE is set, the value *must* be a     */
/* macro_text pointer.                                                  */
/************************************************************************/

VOID free_symbol_entry(symbol, flags)
struct symbol *symbol;
UWORD flags;
{
struct macro_text *mt;

if (flags & SYM_FREEKEY)
   FreeMem(symbol->key, strlen(symbol->key)+1);

if (flags & SYM_FREEVALUE) {

   mt = (struct macro_text *)symbol->value;

   mt->ref_count--;
   if (mt->ref_count <= 0) {
      FreeMem(mt->start, mt->end - mt->start);
      FreeMem(mt, sizeof(struct macro_text));
   }
   else                    /* if there are outstanding refs, make sure it */
      mt->flags |= MTEXT_FREETEXT | MTEXT_FREEMTEXT;  /* gets cleaned up */
}

symbol->key = NULL;              /* just in case... */
symbol->value = NULL;

}

