/*###################################################################*/
/* getch.c                                                           */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"
#include <stdio.h>

#define debug 0

#define MAX(a,b)  (((a)>(b)) ? (a) : (b))

extern struct symtab sym_table;
extern struct symtab cmd_table;
extern struct symtab font_table;
extern struct symtab lfont_table;

extern short this_page;
extern SHORT texterrors;
extern struct Window *clipboard;

/************************************************************************/
/* ch = get_ch(&tc);                                                    */
/*                                                                      */
/* Gets a character from the current text_ctrl struct.  Interprets      */
/* macros, commands, expressions, etc.                                  */
/************************************************************************/

char get_ch(tc)
struct text_ctrl *tc;
{
char ch;
char key[256];
char fontname[FONTNAMESIZE];
char text1[80], text2[80];
char *dum;
int i;
int parnum;
char delim;
char escend;
short tabbed;
char *sp, *ep, *macstart;
char cmd_ch;
struct symbol *sym;
struct parameter *param;
struct macro *original_macro;
struct macro *end_macro(), *start_macro(), *copy_macro_chain();
struct symbol *search_table();
struct parameter *add_param();

if (tc->unget_macro != NULL) {
   free_macro_chain(tc->unget_macro);
   tc->unget_macro = NULL;
}

tc->unget_okay = TRUE;
tc->unget_prevch = tc->prevch;      /* save so we can back up later */
tc->unget_x = tc->x;
tc->unget_maxw = tc->maxw;

tc->prevch = tc->ch;
tc->ch = '\0';                   /* in case we return early */

/* Check to see if we can back up in unget_ch by simply decrementing the  */
/* pointer.  If not, we need to copy the macro chain into tc->unget_macro */
/* in order to be able to back up.  We can't just decrement the pointer   */
/* if there's anything unusual in the text:  the end of text (or end of a */
/* macro), a parameter, an escaped character (\), or a macro (command)    */
/* call.                                                                  */

if ((tc->m == NULL) || ((ch=M_GETCH(tc->m)) == '\0') ||
    (ch == tc->state.paramstart) || (ch == '\\') ||
    (tc->m->flags & MACRO_INCALL) || (ch == tc->state.escstart)) {

   if (tc->unget_macro != NULL)  /* should be NULL, but just in case... */
      free_macro_chain(tc->unget_macro);
   tc->unget_macro = copy_macro_chain(tc->m);
   tc->unget_state = tc->state;
   tc->unget_prevch = tc->prevch;
   tc->unget_x = tc->x;
   tc->unget_maxw = tc->maxw;
   tc->unget_okay = FALSE;
}

escend = tc->state.escend;       /* in case it changes */

getch:

original_macro = tc->m;

if (tc->m == NULL)
   return '\0';

ch = M_GETCH(tc->m);

if (ch == '\0') {                         /* end of macro level */
   tc->m = end_macro(tc->m);
   goto getch;
}

if (ch == tc->state.paramstart) {                     /* parameter */
   M_NEXT(tc->m);
   parnum = int_expr(&tc->m) - 1;
   if (parnum < 0)
      parnum = 0;
   ch = M_GETCH(tc->m);
   if (ch != tc->state.paramend) {
      sprintf(text1, "near '%.15s'", tc->m->current);
      TEXTERROR("Invalid parameter reference", text1);
      goto getch;
   }
   else
      M_NEXT(tc->m);

/* If already inside a parameter, then any params encountered must be   */
/* expanded in the context of the calling macro, using param->context.  */

   if (tc->m->macrotext->flags & MTEXT_PARAMETER) {
      param = ((struct parameter *)(tc->m->macrotext))->context;
   }
   else {
      param = tc->m->param;
   }

   for (i=0; i<parnum; i++) {
      if (param != NULL)
         param = param->next;
      else
         break;
   }
   if (param == NULL) {
      sprintf(text1, "Parameter number %d is too big", parnum);
      sprintf(text2, "near '%.15s'", tc->m->current);
      TEXTERROR(text1, text2);
      goto getch;
   }
   if (param != NULL) {
      tc->m = start_macro(tc->m, &param->mtext, NULL);
   }
   goto getch;
}


if ((tc->m->flags & MACRO_INCALL) || ch == tc->state.escstart) {
   cmd_ch = '\0';                               /* start macro */

   if (!(tc->m->flags & MACRO_INCALL)) {
      M_NEXT(tc->m);
      ch = M_GETCH(tc->m);
   }
   macstart = tc->m->current;

   if (isalpha(ch)) {               /* macro name, command, or variable */
      i = 0;
      while (isalnum(ch)) {
         key[i++] = ch;
         M_NEXT(tc->m);
         ch = M_GETCH(tc->m);
      }
      key[i] = '\0';
      param = NULL;
      if (ch == ' ') {                       /* get the parameters */
         while (ch == ' ') {
            M_NEXT(tc->m);
            ch = M_GETCH(tc->m);
         }
         sp = tc->m->current;
         ep = sp;
         while (ch != '\0' && ch != escend && ch != ';' && ch != ',') {
            if (ch == '\"' || ch == '\'') {          /* delimited string */
               delim = ch;
               M_NEXT(tc->m);
               ch = M_GETCH(tc->m);
               sp = tc->m->current;
               while (ch != delim && ch != '\0') {
                  M_NEXT(tc->m);
                  ch = M_GETCH(tc->m);
               }
               ep = tc->m->current;
               if (ch == delim) {
                  M_NEXT(tc->m);
                  ch = M_GETCH(tc->m);
               }
            }
            else {                              /* no delimiter */
               sp = tc->m->current;
               while (ch != escend && ch != ';' && ch != ',' &&
                      ch != ' ' && ch != '\0') {
                  M_NEXT(tc->m);
                  ch = M_GETCH(tc->m);
               }
               ep = tc->m->current;
            }
            param = add_param(param, sp, ep, tc->m->param);
            while (ch == ' ') {
               M_NEXT(tc->m);
               ch = M_GETCH(tc->m);
            }
         }
      }

      /* Done parsing macro, now figure out what to do with it */

      sym = search_table(&sym_table, key);
      if (sym != NULL) {
         tc->m = start_macro(tc->m, sym->value, param);
      }
      else {
         sym = search_table(&cmd_table, key);
         if (sym != NULL) {
            cmd_ch = process_cmd(tc, sym, param);
            if (param != NULL)
               free_param_list(param);
         }
         else {
            strncpy(fontname, key, FONTNAMESIZE);
            if (stcpm(fontname, ".font", &dum) == 0)
              strncat(fontname, ".font", FONTNAMESIZE-strlen(fontname)-1);
            sym = search_table(&font_table, fontname);
            if (sym == NULL)
               sym = search_table(&lfont_table, key);
            if (sym != NULL) {
               process_font_cmd(tc, key, param);
               if (param != NULL)
                  free_param_list(param);
            }
            else {
               sprintf(text1, "'%.15s'", key);
               TEXTERROR("Command or macro not found:", text1);
            }
         }
      }
   }                          /* end if isalpha */
   else {
      do_expr:
      sprintf(text1, "near '%.15s'", original_macro->current);
      TEXTERROR("Invalid command or macro call", text1);
   }

   /* check for proper macro ending */

   ch = M_GETCH(original_macro);
   if (ch == escend) {
      M_NEXT(original_macro);
      original_macro->flags &= ~MACRO_INCALL;
      escend = tc->state.escend;       /* in case it changed */
   }
   else if (ch == ';' || ch == ',') {
      M_NEXT(original_macro);
      ch = M_GETCH(original_macro);
      while (ch == ' ') {                    /* skip leading blanks */
         M_NEXT(original_macro);
         ch = M_GETCH(original_macro);
      }
      original_macro->flags |= MACRO_INCALL;
   }
   else {
      sprintf(text1, "near '%.15s'", original_macro->current);
      TEXTERROR("Bad ending to macro call", text1);
      original_macro->flags &= ~MACRO_INCALL;
   }

   if (cmd_ch == '\0')  /* no special chars */
      goto getch;
   else {
      ch = cmd_ch;
      goto done;        /* special char because of cmd (CHAR_HSKIP,etc.) */
   }
}

if (ch == tc->state.softhyphen)
   ch = CHAR_SOFTHYPHEN;
if (ch == tc->state.tabchar)
   ch = CHAR_TAB;
if (ch == tc->state.tiechar)
   ch = CHAR_TIE;

if (ch == '\\') {                      /* escaped character */
   M_NEXT(tc->m);
   ch = get_quoted_char(tc->m);
}
else
   M_NEXT(tc->m);                         /* normal character */

done:

if (ch == CHAR_TAB) {
   tabbed = FALSE;
   for (i=0; i<NUM_TABS; i++) {           /* search the tab table */
      if (tc->state.tabs[i] > tc->x) {          /* found it */
         tc->x = tc->state.tabs[i];
         tabbed = TRUE;
         break;
      }
   }
   if (!tabbed) {
      sprintf(text1,"Tab ignored. Pg %d",this_page);
      TEXTERROR("Not enough tab stops", text1);
   }
}

italic_adjust(ch, &tc->state, &tc->x, &tc->itkern);
apply_kern(tc->prevch, ch, &tc->state, &tc->x);
tc->startx = tc->x;
tc->x += char_width(ch, &tc->state);
tc->maxw = MAX(tc->maxw, tc->x);
tc->count++;
tc->ch = ch;

#if debug
printf("getch: ch=%c (%x)\n", ch, ch);
#endif
return ch;

}


/************************************************************************/
/* force_get_ch(&tc, ch);                                               */
/*                                                                      */
/* Fake a get_ch() and accept_ch() call by replacing 'ch' instead of    */
/* the real last character.  Good for replacing soft hyphens with real  */
/* hyphens.                                                             */
/************************************************************************/

force_get_ch(tc, ch)
struct text_ctrl *tc;
char ch;
{

#if debug
printf("force_get_ch, ch=%c (%x)\n", ch, ch);
#endif
tc->ch = ch;
italic_adjust(ch, &tc->state, &tc->x, &tc->itkern);
apply_kern(tc->prevch, ch, &tc->state, &tc->x);
tc->startx = tc->x;
tc->x += char_width(ch, &tc->state);
tc->maxw = MAX(tc->maxw, tc->x);

return ch;

}


/************************************************************************/
/* unget_ch(&tc);                                                       */
/*                                                                      */
/* Undoes a call to get_ch, up to one character back.  If the last char */
/* gotten was a simple character, then just save that char away to use  */
/* later.  If it's not (i.e., there was a macro involved), then we      */
/* restore the state and macro pointer to where it was before this char */
/* was gotten (saved by get_ch).                                        */
/************************************************************************/

VOID unget_ch(tc)
struct text_ctrl *tc;
{

#if debug
printf("unget: original ch=%c (%x)\n", tc->ch, tc->ch);
#endif
if (tc->unget_macro != NULL) {
   free_macro_chain(tc->m);
   tc->m = tc->unget_macro;
   tc->unget_macro = NULL;
   tc->state = tc->unget_state;
}
else if (tc->unget_okay) {
   tc->m->current--;          /* back up pointer */
}
else
   return;                    /* whoops, can't backup! */

tc->unget_okay = FALSE;       /* can't unget again */
tc->ch = tc->prevch;
tc->prevch = tc->unget_prevch;
tc->count--;
tc->x = tc->unget_x;
tc->maxw = tc->unget_maxw;
#if debug
printf("unget: final ch=%c (%x)\n", tc->ch, tc->ch);
#endif
}


/*********************************************************************/
/* ch = get_quoted_char(m);                                          */
/*                                                                   */
/* Gets a character quoted by a backslash from the current macro     */
/* chain.  The backslash has already been eaten, i.e. the macro      */
/* points past it.  On exit, the macro points to the first char past */
/* the quoted chars.                                                 */
/*********************************************************************/

get_quoted_char(m)
struct macro *m;
{
char ch;
char buf[6];
UBYTE newch;

ch = M_GETCH(m);
if (ch == 'n')
   ch = '\n';
else if (ch == 't')
   ch = '\t';
else if (ch >= '0' && ch <= '9') {
   newch = 0;
   while (ch >= '0' && ch <= '9') {
      newch = newch*10 + (ch-'0');
      M_NEXT(m);
      ch = M_GETCH(m);
   }
   if (newch <= CHAR_MAX) {
      sprintf(buf, "\\%d", newch);
      TEXTERROR("Invalid character code:", buf);
      newch = '?';
   }
   return((int)newch);
}
M_NEXT(m);
return((int)ch);
}

