#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "text.h"

#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

extern SHORT texterrors;

extern short text_change;
COORD opt_char_height, opt_char_depth;
extern struct text_state *first_state;

static struct text_ctrl tc, trial_tc, save_tc, wid1_tc, wid2_tc;
static char ch, trialch;
static COORD rmarg,savermarg,wid1rmarg,wid2rmarg;

static int trialblank;
static short contains_softhyphen;

short this_page;    /** used so error msg's can print page # */

short force_end = 0;   /** TAKE THIS OUT AND PASS STATUS   **/
                                 /** FROM FORMAT !!! **/

VOID free_text_object();
VOID get_word(), get_hyphenated_word();
VOID force_hyphenation(), get_one_letter();
VOID start_trial(), append_trial(), discard_trial();
VOID save_line(), save_widow(), rotate_widow();
VOID backup_line(), backup_widow();
VOID do_char(), force_do_char(), strip_leading_blanks();
VOID inc_space(), copy_text_ctrl(), discard_text_ctrl();
char replace_text_ctrl();

/*********************************************************************/
/* full = format(obj, flowdown);                                     */
/*                                                                   */
/* Takes text pointed to by obj->data->textstart and formats it into */
/* object obj.  Obj->data must point to an existing and initialized  */
/* text_object.  Returns TRUE if box overflowed, FALSE if            */
/* everything fit.  If flowdown==TRUE, then the top of the line is   */
/* used to check for box overflow, so the text may actually hang     */
/* down below the height in obj.  If flowdown==FALSE, everything is  */
/* as expected, i.e. the bottom of the line is used to check for     */
/* overflow.                                                         */
/*********************************************************************/

int format (obj, flowdown)
struct object *obj;
int flowdown;
{
struct text_object *tobj;
struct text_line *lineptr;
short discard_line, line_fits, end_of_text, line_done, first_word, prevlead;
/* short force_end; */
short in_nobreak;
COORD endheight, botmarg;
COORD th;
SCOORD char_height(),char_depth();
short first_line, check_widow, first_or_second_line, backed_up;
SHORT savetexterrors;
struct text_ctrl line_tc;     /* in case entire line gets discarded */
struct macro *copy_macro_chain();
COORD temptm, tempbm, templm, temprm;
SCOORD tempindent;
UWORD tempflags;


first_state = &((struct text_object *)(obj->first->data))->state;
this_page = obj->pn;
savetexterrors = texterrors;

free_text_object(obj);

tobj = (struct text_object *) obj->data;
tobj->state.flags &= ~(TEXTS_FORCECOLEND|TEXTS_FORCEPAGEEND);

tc.state = tobj->state;
tc.m = copy_macro_chain(tobj->macro);
tc.x = 0;
tc.y = 0;
tc.maxw = 0;
tc.newx = 0;
tc.newy = 0;
tc.startx = 0;
tc.itkern = 0;
tc.ch = 0;
tc.prevch = 0;
tc.unget_macro = NULL;
tc.unget_active = FALSE;

tobj->height = 0;
tobj->width = 0;
endheight = 0;
prevlead = FALSE;
line_fits = TRUE;
end_of_text = FALSE;
first_line = TRUE;
first_or_second_line = TRUE;
in_nobreak = FALSE;
tc.count = 0;
                     /* All text_line structures that are imbedded in a */
                     /* text_ctrl have their 'next' field pointing to   */
                     /* the PREVIOUS line structure that is linked into */
                     /* the chain.  All structures that are linked into */
                     /* the chain have their 'next' field pointing to   */
                     /* the NEXT struct in the chain.                   */
tc.line.next = NULL;
trial_tc.m = NULL;            /* mark other tc's as unused */
trial_tc.unget_macro = NULL;
save_tc.m = NULL;
save_tc.unget_macro = NULL;
wid1_tc.m = NULL;
wid1_tc.unget_macro = NULL;
wid2_tc.m = NULL;
wid2_tc.unget_macro = NULL;
line_tc.m = NULL;
line_tc.unget_macro = NULL;

check_widow = tobj->flags & CHECKWIDOW;

text_change = FALSE;
opt_tptoc(&tc.state);
opt_char_height = char_height(' ', &tc.state);
opt_char_depth = char_depth(' ', &tc.state);

while (line_fits && !end_of_text) {

   copy_text_ctrl(&tc, &line_tc);   /* save in case we discard whole line */

   tc.line.maxh = 0;                         /* initialize new line */
   tc.line.maxd = 0;
   tc.line.num_chars = 0;
   tc.line.stretch_chars = 0;
   tc.line.num_spaces = 0;
   tc.line.flags = 0;

   tc.itkern = 0;
   tc.maxw = 0;
   tc.x = 0;
   tc.ch = '\0';
   discard_line = FALSE;
   line_done = FALSE;

   tc.prevch = tc.ch;

   ch = get_ch(&tc);

   strip_leading_blanks();

   if (ch == '\n') {           /* end of paragraph */
      tc.line.flags |= TEXTL_PARSTART;
      ch = get_ch(&tc);
      tc.line.startcnt = tc.count;
      if (tobj->flags&STRIPNL && first_line) {
         while (ch == '\n') {
            ch = get_ch(&tc);
         }
      }
      if (first_or_second_line) {
         tc.state.flags &= ~(TEXTS_FORCECOLEND|TEXTS_FORCEPAGEEND);
      }
   }
   while (ch == CHAR_SOFTHYPHEN) {
      ch = get_ch(&tc);
   }

   strip_leading_blanks();

   /* Save these so we can set them at the start of a line and still have */
   /* them take effect in that line.  unget_ch() restores these values in */
   /* the state struct to what they were at the end of the previous line, */
   /* which would make it miss any changes in the current line.           */

   temptm = tc.state.top_margin;
   tempbm = tc.state.bottom_margin;
   templm = tc.state.left_margin;
   temprm = tc.state.right_margin;
   tempindent = tc.state.paragraph_indent;
   tempflags = tc.state.flags;

   unget_ch(&tc);

   tc.x = 0;
   tc.maxw = 0;
   tc.itkern = 0;

   tc.line.startcnt = tc.count;

   if (first_line) {
      tobj->height += temptm;
      tc.y += temptm;
      endheight = tobj->height;
      botmarg = tempbm;
   }

   tc.x += templm;
   tc.newx += templm;
   rmarg = temprm;
   if ((tc.line.flags&TEXTL_PARSTART) && !(tempflags&TEXTS_RAGLEFT)) {
      tc.x += tempindent;
      tc.newx += tempindent;
   }
   tc.maxw = MAX(tc.x,tc.maxw);

   if (tempflags&TEXTS_NOBREAK && !in_nobreak) {
      save_line();                        /* start nobreak section */
      in_nobreak = TRUE;
   }
   if (!(tempflags&TEXTS_NOBREAK) && in_nobreak) {
      in_nobreak = FALSE;
      discard_text_ctrl(&save_tc);
   }
   if (check_widow) {
      if (tempflags&TEXTS_NOWIDOW) {       /* turned off */
         discard_text_ctrl(&wid1_tc);
         discard_text_ctrl(&wid2_tc);
      }
      else {                           /* widow/orphan checking turned on */
         rotate_widow();
         save_widow();
      }
   }

   while (!line_done) {

      ch = get_ch(&tc);

      if (ch == '\n') {       /* end of paragraph */
         tc.line.flags |= TEXTL_PAREND;
      }

      if (ch == '\0') {        /* end of text */
         end_of_text = TRUE;
      }

      if (ch == '\0' || ch == '\n') {
         line_done = TRUE;
         if (tc.line.num_chars == 0) {             /* empty line */
            if (ch == '\0')
               discard_line = TRUE;             /* we got nowhere */
            else {                              /* purposeful blank line */
               tc.line.maxh = MAX(tc.line.maxh, opt_char_height);
               tc.line.maxd = MAX(tc.line.maxd, opt_char_depth);
            }
         }
         unget_ch(&tc);          /* reuse \n for start of next paragraph */
      }
      else {

         unget_ch(&tc);

         first_word = (tc.line.num_chars == 0);
         contains_softhyphen = FALSE;
         get_word(first_word);

         if (trial_tc.x + rmarg <= obj->w)      /* it fits on line */
            append_trial();
         else {
            if (contains_softhyphen) {
               discard_trial();
               get_hyphenated_word(first_word);    /* hyphenate last word */
            }
            if (trial_tc.x + rmarg <= obj->w)
               append_trial();
            else {
               discard_trial();
               if (first_word) {                /* word longer than line */
                  force_hyphenation(first_word, obj->w - rmarg);
                  if (!trialblank)
                     append_trial();
                  else {                        /* line shorter than one */
                     discard_trial();
                     get_one_letter();          /* letter + hyphen!!     */
                     append_trial();
                  }
               }
            }
            line_done = TRUE;
         }
      }
   }

   /* Note: Leading blanks in the line must be ignored (by drawtext) */

   if (!discard_line) {
      if (prevlead) {
         if (tobj->height <= tc.line.maxh)
            tobj->height = 0;
         else
            tobj->height -= tc.line.maxh;
      }
      if (flowdown)
         th = tobj->height;
      else
         th = tobj->height + tc.line.maxh + tc.line.maxd;

      force_end = tc.state.flags&(TEXTS_FORCECOLEND|TEXTS_FORCEPAGEEND);
      if (first_line) {
         force_end = FALSE;
         tc.state.flags &= ~(TEXTS_FORCECOLEND|TEXTS_FORCEPAGEEND);
      }

      if (th <= obj->h - botmarg && !force_end) {    /* hook line in */

         lineptr = (struct text_line *)AllocMem(sizeof(struct text_line),
                                                MEMF_PUBLIC);
         *lineptr = tc.line;
         lineptr->w = tc.maxw;         /* save max length */

         if (tc.line.next == NULL) {   /* line.next is ptr to prev line */
            tobj->line = lineptr;      /* until it gets linked into chain */
            tobj->width = tc.maxw;     /* only one line, so shorten width */
         }
         else {
            (tc.line.next)->next = lineptr;
            tobj->width = obj->w;   /* multiple lines, box w = obj w */
         }
         lineptr->next = NULL;
         tc.line.next = lineptr;    /* save ptr to this line for later */

         tobj->height += tc.line.maxh;
         if (tc.state.flags & TEXTS_USELEADING) {
            endheight = MAX(endheight, tobj->height + tc.line.maxd);
            tobj->height += tc.state.leading;
            prevlead = TRUE;
         }
         else {
            endheight = MAX(endheight, tobj->height + tc.line.maxd);
            tobj->height += tc.line.maxd + tc.state.spacing;
            prevlead = FALSE;
         }
         if (tc.line.flags & TEXTL_PAREND)
            tobj->height += tc.state.paragraph_space;
         if (!flowdown)
            tobj->height = MIN(tobj->height, obj->h - botmarg);
      }
      else {
         backed_up = FALSE;
         if (in_nobreak) {       /* breaking a nobreak section */
            backed_up = TRUE;
            backup_line();
         }
         else {
            if (check_widow && !(tc.state.flags&TEXTS_NOWIDOW)) {
                        /* end of par is by itself at top */
               if (((tc.line.flags & TEXTL_PAREND) &&
                   !(tc.line.flags & TEXTL_PARSTART) &&
                   !(force_end))
               ||
                        /* start of par is by itself at bottom */
                        /* (tc.line.next points to prev line)  */
                   (((tc.line.next)->flags&TEXTL_PARSTART) &&
                   !((tc.line.next)->flags&TEXTL_PAREND))) {

                  backed_up = TRUE;
                  backup_widow();
               }
            }
         }
         line_fits = FALSE;      /* line doesn't fit */
         if (!backed_up)         /* so reset to start of line */
            {
            ch = replace_text_ctrl(&line_tc, &tc);
             }
      }
   }
   /* else - line discarded, so ignore it - no resetting necessary */
   if (!first_line)
      first_or_second_line = FALSE;
   first_line = FALSE;
}


if (!flowdown)
   tobj->height = MIN(endheight + botmarg, obj->h);

tobj->endstate = tc.state;
tobj->endmacro = copy_macro_chain(tc.m);

if (tc.state.flags&TEXTS_VCENTER)
   tobj->flags |= VERTCENTER;

texterrors = savetexterrors;


discard_text_ctrl(&tc);                /* cleanup */
discard_text_ctrl(&trial_tc);
discard_text_ctrl(&save_tc);
discard_text_ctrl(&wid1_tc);
discard_text_ctrl(&wid2_tc);
discard_text_ctrl(&line_tc);


if (end_of_text)
   return(FALSE);       /* no overflow */
else
   return(TRUE);        /* overflow */

}


/************************************************************************/
/* free_text_object(obj);                                               */
/*                                                                      */
/* Frees memory allocated to text_lines that are pointed to by obj.     */
/* This may be called independently, but format_text calls it before    */
/* re-formatting.                                                       */
/************************************************************************/

VOID free_text_object(obj)
struct object *obj;
{
struct text_line *l, *nl;

nl = NULL;
l = ((struct text_object *)obj->data)->line;
while (l != NULL) {
   nl = l->next;
   FreeMem(l, sizeof(struct text_line));
   l = nl;
}
((struct text_object *)obj->data)->line = NULL;
}


/*********************************************************************/
/* get_word(first);                                                  */
/*                                                                   */
/* Gets a word, defined as a group of nonblank characters with       */
/* leading blanks, into the trial variables.  If first==TRUE, then   */
/* don't include the leading blanks.                                 */
/*********************************************************************/

VOID get_word (first)
int first;
{

start_trial();

trialch = get_ch(&trial_tc);

while (discardable_spacechar(trialch)) {
   do_char();
   inc_space();
}

while (spacechar(trialch)) {
   do_char();
   inc_space();
}

while (wordchar(trialch)) {
   do_char();
   while (trialch == CHAR_SOFTHYPHEN) {
      contains_softhyphen = TRUE;
      trialch = get_ch(&trial_tc);
   }
   trialblank = FALSE;
}
unget_ch(&trial_tc);
}


/*********************************************************************/
/* get_hypenated_word(first);                                        */
/*                                                                   */
/* Gets a word, defined as a group of nonblank characters with       */
/* leading blanks, into the trial variables.  If first==TRUE, then   */
/* don't include the leading blanks.  A soft hyphen will delimit the */
/* end of the word.                                                  */
/*********************************************************************/

VOID get_hyphenated_word (first)
int first;
{

start_trial();

trialch = get_ch(&trial_tc);

while (discardable_spacechar(trialch)) {
   do_char();
   inc_space();
}

while (spacechar(trialch)) {
   do_char();
   inc_space();
}

while (wordchar(trialch) && trialch != CHAR_SOFTHYPHEN) {
   do_char();
   trialblank = FALSE;
}

if (trialch == CHAR_SOFTHYPHEN) {      /* found a soft hyphen */

   while (trialch == CHAR_SOFTHYPHEN)     /* skip any more soft hyphens */
      trialch = get_ch(&trial_tc);
   unget_ch(&trial_tc);

   force_do_char(HYPHENCHAR);
   trial_tc.line.flags |= TEXTL_HYPHEN;
}
else
   unget_ch(&trial_tc);

}


/*********************************************************************/
/* force_hypenation(first, wmax);                                    */
/*                                                                   */
/* Gets a word, defined as a group of nonblank characters with       */
/* leading blanks, into the trial variables.  If first==TRUE, then   */
/* don't include the leading blanks.  Forces a hyphenation when the  */
/* line is full.                                                     */
/*********************************************************************/

VOID force_hyphenation (first, wmax)
int first;
COORD wmax;
{
int full;
struct text_ctrl hyphbad_tc, hyphgood_tc;

hyphbad_tc.m = NULL;
hyphbad_tc.unget_macro = NULL;
hyphgood_tc.m = NULL;
hyphgood_tc.unget_macro = NULL;

start_trial();

trialch = get_ch(&trial_tc);

/* This leading space stuff doesn't matter since this is only called */
/* at the beginning of a line (when first = TRUE)                    */

while (discardable_spacechar(trialch)) {
   do_char();
   inc_space();
}

while (spacechar(trialch)) {
   do_char();
   inc_space();
}

full = FALSE;
while (wordchar(trialch) && trialch != CHAR_SOFTHYPHEN && !full) {

   copy_text_ctrl(&trial_tc, &hyphbad_tc);      /* w/o this character */

   do_char();

   copy_text_ctrl(&trial_tc, &hyphgood_tc);     /* with this char, w/o - */

   force_do_char(HYPHENCHAR);                   /* now with hyphen */

   if (trial_tc.x > wmax) {
      full = TRUE;                                 /* don't use this char */
      trialch = replace_text_ctrl(&hyphbad_tc, &trial_tc);
      discard_text_ctrl(&hyphgood_tc);
   }
   else {                                          /* use this char */
      trialch = replace_text_ctrl(&hyphgood_tc, &trial_tc);
      discard_text_ctrl(&hyphbad_tc);
      trialblank = FALSE;
   }
}

unget_ch(&trial_tc);

force_do_char(HYPHENCHAR);

trial_tc.line.flags |= TEXTL_HYPHEN;
}


/*********************************************************************/
/* get_one_letter();                                                 */
/*                                                                   */
/* Gets a single letter into the trial variables.  This is for the   */
/* extremely rare (hopefully!) case where the line is shorter than a */
/* single letter plus a hyphen.                                      */
/*********************************************************************/

VOID get_one_letter ()
{

start_trial();

trialch = get_ch(&trial_tc);

trialblank = FALSE;

do_char();

trial_tc.line.flags &= ~TEXTL_HYPHEN;      /* turn off hyphen (just in case) */

unget_ch(&trial_tc);
}


/*********************************************************************/
/* start_trial();                                                    */
/*                                                                   */
/* Initializes the trial variables to the current real variables for */
/* the start of a new trial.                                         */
/*********************************************************************/

VOID start_trial()
{

copy_text_ctrl(&tc, &trial_tc);
trialch = ch;

trialblank = TRUE;
text_change = FALSE;

}


/*********************************************************************/
/* append_trial();                                                   */
/*                                                                   */
/* Accept the trial by setting the real variables to the trial vars  */
/*********************************************************************/

VOID append_trial()
{

ch = replace_text_ctrl(&trial_tc, &tc);

#if 0                   /**** Probably obsolete but kept just in case ****/
if (trialblank)         /* ignore totally blank words (at end of line) */
   line.text = trialline.text;      /* rare case - empty line */
else
   line = trialline;
#endif

}


/*********************************************************************/
/* discard_trial();                                                  */
/*                                                                   */
/* Discard the trial - for now, only need to reset the optimization  */
/* parameters for textcoord.                                         */
/*********************************************************************/

VOID discard_trial()
{

discard_text_ctrl(&trial_tc);
if (text_change) {
   opt_tptoc(&tc.state);
   opt_char_height = char_height(' ', &tc.state);
   opt_char_depth = char_depth(' ', &tc.state);
}
}


/*********************************************************************/
/* save_line();                                                      */
/*                                                                   */
/* Save the line variables so we can back up to this point later.    */
/*********************************************************************/

VOID save_line()
{

copy_text_ctrl(&tc, &save_tc);
savermarg = rmarg;

text_change = FALSE;

}


/*********************************************************************/
/* save_widow();                                                     */
/*                                                                   */
/* Save the line variables so we can back up to this point later to  */
/* remove a widow or orphan line.                                    */
/*********************************************************************/

VOID save_widow()
{

copy_text_ctrl(&tc, &wid1_tc);
wid1rmarg = rmarg;

text_change = FALSE;

}

/*********************************************************************/
/* rotate_widow();                                                   */
/*                                                                   */
/* Rotate the widow saved line variables so we can back up to this   */
/* point later.  Widow checking requires us to save two lines back,  */
/* thus this rotate.                                                 */
/*********************************************************************/

VOID rotate_widow()
{

copy_text_ctrl(&wid1_tc, &wid2_tc);
wid2rmarg = wid1rmarg;

text_change = FALSE;

}


/*********************************************************************/
/* backup_line(sl);                                                  */
/*                                                                   */
/* Restore the line status from the save variables, freeing all      */
/* text_lines after the current line.                                */
/*********************************************************************/

VOID backup_line()
{

ch = replace_text_ctrl(&save_tc, &tc);
rmarg = savermarg;

}


/*********************************************************************/
/* backup_widow(sl);                                                 */
/*                                                                   */
/* Restore the line status from the widow variables, freeing all     */
/* text_lines after the current line.                                */
/*********************************************************************/

VOID backup_widow()
{

ch = replace_text_ctrl(&wid2_tc, &tc);
rmarg = wid2rmarg;

}


/************************************************************************/
/* do_char();                                                           */
/*                                                                      */
/* Processes the current char and updates all state variables for it.   */
/************************************************************************/

VOID do_char()
{

trial_tc.line.maxh =
      MAX(trial_tc.line.maxh, opt_char_height);
trial_tc.line.maxd =
      MAX(trial_tc.line.maxd, opt_char_depth);
trial_tc.line.num_chars++;
if (trial_tc.ch != CHAR_SOFTHYPHEN)
   trial_tc.line.stretch_chars++;
if (trial_tc.ch == CHAR_TIE)
   inc_space();
trialch = get_ch(&trial_tc);

}

/************************************************************************/
/* force_do_char(tch);                                                  */
/*                                                                      */
/* Forces in a char (like a hyphen) and updates all state variables for */
/* it.  The difference is that the number of chars is not incremented.  */
/************************************************************************/

VOID force_do_char(tch)
char tch;
{

force_get_ch(&trial_tc, tch);
trial_tc.line.maxh =
      MAX(trial_tc.line.maxh, opt_char_height);
trial_tc.line.maxd =
      MAX(trial_tc.line.maxd, opt_char_depth);

}


/************************************************************************/
/* strip_leading_blanks();                                              */
/*                                                                      */
/* Strips leading blanks from a line.                                   */
/************************************************************************/

VOID strip_leading_blanks()
{
int x;

x = tc.x;                     /* starting x pos */

while (discardable_spacechar(ch)) {
   tc.x = x;
   ch = get_ch(&tc);
}

#if 0

trial_tc.line.startcnt = trial_tc.count;  /* skip to this point */

trial_tc.x = tc.x;
trial_tc.y = tc.y;
trial_tc.newx = tc.newx;                  /* fix up other state vars */
trial_tc.newy = tc.newy;
trial_tc.startx = tc.startx;
trial_tc.maxw = trial_tc.x;
trial_tc.itkern = 0;
trial_tc.prevch = '\0';
trial_tc.line.w = tc.line.w;
trial_tc.line.maxh = tc.line.maxh;
trial_tc.line.maxd = tc.line.maxd;
trial_tc.line.num_chars = tc.line.num_chars;
trial_tc.line.stretch_chars = tc.line.stretch_chars;
trial_tc.line.num_spaces = tc.line.num_spaces;
#endif

}


/************************************************************************/
/* result = wordchar(ch);                                               */
/*                                                                      */
/* Returns TRUE if ch is part of a word; FALSE otherwise.               */
/************************************************************************/

int wordchar(ch)
char ch;
{
   if (ch == CHAR_HSKIP) return(FALSE);
   if (ch == CHAR_HPOS) return(FALSE);
   if (ch == CHAR_TAB) return(FALSE);
   if (ch == ' ') return(FALSE);
   if (ch == '\n') return(FALSE);
   if (ch == '\0') return(FALSE);
   return(TRUE);
}


/************************************************************************/
/* result = spacechar(ch);                                              */
/*                                                                      */
/* Returns TRUE if ch is a space character; FALSE otherwise.            */
/************************************************************************/

int spacechar(ch)
char ch;
{
   if (ch == ' ') return(TRUE);
   if (ch == CHAR_HSKIP) return(TRUE);
   if (ch == CHAR_HPOS) return(TRUE);
   if (ch == CHAR_TAB) return(TRUE);
   return(FALSE);
}


/************************************************************************/
/* result = discardable_spacechar(ch);                                  */
/*                                                                      */
/* Returns TRUE if ch is a space character that can be thrown away at   */
/* the beginning of a line (NOT tabs, hskips, etc.); FALSE otherwise.   */
/************************************************************************/

int discardable_spacechar(ch)
char ch;
{
   if (ch == ' ') return(TRUE);
   return(FALSE);
}


/************************************************************************/
/* inc_space();                                                         */
/*                                                                      */
/* Increment trial_tc.line.num_spaces unless trialch is a tab or abs    */
/* hskip character, in which case trial_tc.line.num_spaces and          */
/* trial_tc.line.stretch_chars are both set to 0.  This allows fill     */
/* justify on lines that have tabs.                                     */
/************************************************************************/

VOID inc_space()
{

trial_tc.line.num_spaces++;
if (trialch == CHAR_TAB || trialch == CHAR_HPOS) {
   trial_tc.line.num_spaces = 0;
   trial_tc.line.stretch_chars = 0;
}
}


/************************************************************************/
/* copy_text_ctrl(from, to);                                            */
/*                                                                      */
/* Copies the text_ctrl structure so we can back up to this point later */
/* if we want.                                                          */
/************************************************************************/

VOID copy_text_ctrl(from, to)
struct text_ctrl *from, *to;
{
struct macro *copy_macro_chain();

if (to->m != NULL || to->unget_macro != NULL)
   discard_text_ctrl(to);
*to = *from;
to->m = copy_macro_chain(from->m);
to->unget_macro = copy_macro_chain(from->unget_macro);

}


/************************************************************************/
/* ch = replace_text_ctrl(from, to);                                    */
/*                                                                      */
/* Replaces a text_ctrl structure from a previously saved one.  Note    */
/* that calling this function invalidates the copy, i.e. you must call  */
/* copy_text_ctrl again before restoring from it again.                 */
/* This function also frees any text_line structures linked in to the   */
/* chain past where we are backing up to.  From->next actually points   */
/* to the previous line which has been linked in, so from->next->next   */
/* points to the copy of the current line structure that has been       */
/* linked.  Since it has been linked, it's 'next' field really points   */
/* to the next struct instead of the previous one.  Text_line structs   */
/* are freed starting at this point (the linked current line).  Returns */
/* the ch from the text_ctrl struct which should be assigned to the     */
/* corresponding ch var, if any.  This is a convenience only.           */
/************************************************************************/

char replace_text_ctrl(from, to)
struct text_ctrl *from, *to;
{
struct text_line *prev, *p, *pnext;

if (to->m != NULL || to->unget_macro != NULL)
   discard_text_ctrl(to);

*to = *from;
from->m = NULL;               /* mark as invalid */
from->unget_macro = NULL;

prev = from->line.next;       /* previous line */
if (prev != NULL && prev->next != NULL) {
   for (p=prev->next; p!=NULL; p=pnext) {    /* free any lines past where */
      pnext = p->next;                       /* this one will go          */
      FreeMem(p, sizeof(struct text_line));
   }
   prev->next = NULL;
}
return to->ch;
}


/************************************************************************/
/* discard_text_ctrl(from);                                             */
/*                                                                      */
/* Discards a text_ctrl structure.                                      */
/************************************************************************/

VOID discard_text_ctrl(from)
struct text_ctrl *from;
{

if (from->m != NULL) {
   free_macro_chain(from->m);
   from->m = NULL;               /* mark as invalid */
}
if (from->unget_macro != NULL) {
   free_macro_chain(from->unget_macro);
   from->unget_macro = NULL;
}

}

