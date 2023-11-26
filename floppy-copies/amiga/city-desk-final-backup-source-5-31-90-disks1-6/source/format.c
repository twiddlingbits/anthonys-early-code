#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "text.h"

#define debug 0
#define ndebug 0
#define hdebug 0

#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

extern SHORT texterrors;
extern SHORT autohyphenate;

extern short text_change;
COORD opt_char_height, opt_char_depth;
extern struct text_state *first_state;

static struct text_ctrl tc, trial_tc, save_tc, wid1_tc, wid2_tc, line_tc;
static char ch, trialch;
static COORD rmarg,savermarg,wid1rmarg,wid2rmarg;

static int trialblank;
static short contains_softhyphen;
static short hyphen_ok;

short this_page;    /** used so error msg's can print page # */

short force_end = 0;   /** TAKE THIS OUT AND PASS STATUS   **/
                                 /** FROM FORMAT !!! **/

VOID free_text_object();
VOID get_word();
int get_softhyphen_word(), get_autohyphen_word(), get_autohyphen();
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
short irregular_fits, uses_irregular;
SHORT savetexterrors;
struct macro *copy_macro_chain();
COORD temptm, tempbm, templm, temprm;
SCOORD tempindent, tempirreg_margin, tempirreg_minwidth;
UWORD tempflags;
short valid;
COORD x_left1, x_left2, x_right1, x_right2;

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
tc.unget_okay = FALSE;

#if debug
printf("format string='%s'\n", tc.m->current);
#endif
#if ndebug
printf("format: obj->h=%x, obj->w=%x\n", obj->h, obj->w);
#endif

tobj->height = 0;
tobj->width = 0;
endheight = 0;
prevlead = FALSE;
line_fits = TRUE;
end_of_text = FALSE;
first_line = TRUE;
first_or_second_line = TRUE;
in_nobreak = FALSE;
irregular_fits = TRUE;
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

#if debug
   printf("format new line\n");
#endif

   tc.y = tobj->height;
   if (!prevlead)
      tc.y += opt_char_height;   /* add in cur font height for baseline */
   tc.line.ypos = tc.y;

#if ndebug
   printf("line y pos = %d\n", tc.y);
#endif

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
   tempirreg_margin = tc.state.irreg_margin;
   tempirreg_minwidth = tc.state.irreg_minwidth;

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

   if ((!(tc.line.flags&TEXTL_PARSTART)) || (tempflags&TEXTS_RAGLEFT))
      tempindent = 0;               /* don't use indent */

   /* Do irregular flow around graphics.  Must check both top and bottom */
   /* of line.  Since we don't know the true size of the line, we guess  */
   /* based on the size of the current font.                             */

   get_bounds(obj, tc.y-opt_char_height, &x_left1, &x_right1,
              tempirreg_margin);
   get_bounds(obj, tc.y+opt_char_depth, &x_left2, &x_right2,
              tempirreg_margin);
   x_left1 = MAX(x_left1, x_left2);
   x_right1 = MAX(x_right1, x_right2);

   tc.x = MAX(x_left1, templm);        /* max of margin and irreg margin */
   rmarg = MAX(x_right1, temprm);
   uses_irregular = FALSE;
   if (tc.x != templm || rmarg != temprm)
      uses_irregular = TRUE;
   tc.x += tempindent;

   /* If it fits before irregular adjustment, but not after, then set */
   /* irregular_fits to FALSE so we will stop flowing.  The check for */
   /* fitting before is so we will do at least one char every line if */
   /* the line extends the whole column.  This avoids infinite loops. */
   /* This check is only performed if we actually have an irregular   */
   /* flow for this line.                                             */

#if ndebug
printf("tc.x=%x, rmarg=%x, temp_irreg_minwidth=%x, obj->w=%x\n", tc.x,rmarg,tempirreg_minwidth,obj->w);
printf("templm=%x, temprm=%x, tempindent=%x\n", templm, temprm, tempindent);
#endif
   if ((tc.x + rmarg + tempirreg_minwidth >= obj->w) &&
       (templm + temprm + tempindent < obj->w) && uses_irregular)
      irregular_fits = FALSE;

   tc.line.left = tc.x;       /* save for drawtext: irreg and greeking */
   tc.line.right = rmarg;

#if ndebug
printf("tc.line.left=%x, tc.line.right=%x\n", tc.line.left,tc.line.right);
#endif

   tc.newx = tc.x;

   tc.maxw = MAX(tc.x,tc.maxw);

   if (tempflags&TEXTS_NOBREAK && !in_nobreak) {
      save_line();                        /* start nobreak section */
      in_nobreak = TRUE;
   }
   if (!(tempflags&TEXTS_NOBREAK) && in_nobreak) {
      in_nobreak = FALSE;
#if debug
      printf("discard save_tc (end nobreak)\n");
#endif
      discard_text_ctrl(&save_tc);
   }
   if (check_widow) {
      if (tempflags&TEXTS_NOWIDOW) {       /* turned off */
#if debug
         printf("discard wid1_tc (check_widow)\n");
#endif
         discard_text_ctrl(&wid1_tc);
#if debug
         printf("discard wid2_tc (check_widow)\n");
#endif
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
#if debug
         printf("end of text found\n");
#endif
         end_of_text = TRUE;
      }

      if (ch == '\0' || ch == '\n') {
#if debug
         printf("eot or eol\n");
#endif
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

#if debug
         printf("before final unget ch=%c (%x)\n", tc.ch, tc.ch);
#endif
         unget_ch(&tc);
#if debug
         printf("after final unget ch=%c (%x)\n", tc.ch, tc.ch);
#endif

         first_word = (tc.line.num_chars == 0);
         contains_softhyphen = 0;
         get_word(first_word);

         if (trial_tc.x + rmarg <= obj->w)      /* it fits on line */
            append_trial();
         else {
            valid = TRUE;
            if (contains_softhyphen) {
               discard_trial();                 /* hyphenate last word */
               valid = get_softhyphen_word(first_word, obj->w - rmarg,
                                           contains_softhyphen);
            }
            else if (autohyphenate && hyphen_ok &&
                     obj->w - rmarg - tc.x > tc.state.autohyph_threshhold) {
               discard_trial();
               valid = get_autohyphen_word(first_word, obj->w - rmarg);
            }
            if (trial_tc.x + rmarg <= obj->w && valid)
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

#if ndebug
printf("end of line: tc.maxw=%x, maxh=%x, maxd=%x, tobj->h=%x\n", tc.maxw,tc.line.maxh,tc.line.maxd,tobj->height);
#endif
#if debug
   printf("processing line\n");
#endif
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

#if ndebug
printf("th=%x, obj->h-botmarg=%x, force_end=%x, irreg_fits=%x\n",th,obj->h-botmarg, force_end, irregular_fits);
#endif
      if (th <= obj->h - botmarg && !force_end && irregular_fits) {
                                                         /* hook line in */
#if debug
         printf("hooking line in\n");
#endif
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
         if (uses_irregular)        /* if this line is irregular, */
            tobj->width = obj->w;      /* then don't shorten it!  */

         lineptr->next = NULL;
         tc.line.next = lineptr;    /* save ptr to this line for later */

         tobj->height += tc.line.maxh;
         lineptr->ypos = tobj->height;    /* true y position */
#if ndebug
printf("hooking:tobj->width=%x, tobj->height=%x\n", tobj->width, tobj->height);
#endif
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

         lineptr->flags &= ~(TEXTL_RAGLEFT | TEXTL_RAGRIGHT);
         if (tempflags & TEXTS_RAGLEFT)   /* save justify stat for greek */
            lineptr->flags |= TEXTL_RAGLEFT;
         if (tempflags & TEXTS_RAGRIGHT)
            lineptr->flags |= TEXTL_RAGRIGHT;

#if ndebug
printf("end hook: tobj->height=%x, endheight=%x\n", tobj->height,endheight);
#endif

      }
      else {
#if debug+ndebug
         printf("discarding line\n");
#endif
         backed_up = FALSE;
         if (in_nobreak) {       /* breaking a nobreak section */
            backed_up = TRUE;
            backup_line(tobj);
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
#if debug
                  printf("backup_widow\n");
#endif
                  backup_widow(tobj);
               }
            }
         }
         line_fits = FALSE;      /* line doesn't fit */
         if (!backed_up)         /* so reset to start of line */
            ch = replace_text_ctrl(&line_tc, &tc, NULL);
      }
   }
   /* else - line discarded, so ignore it - no resetting necessary */
   if (!first_line)
      first_or_second_line = FALSE;
   first_line = FALSE;
#if debug
   printf("end line loop\n");
#endif
}


if (!flowdown)
   tobj->height = MIN(endheight + botmarg, obj->h);

tobj->endstate = tc.state;
tobj->endmacro = copy_macro_chain(tc.m);
tobj->endcount = tc.count;

if (tc.state.flags&TEXTS_VCENTER)
   tobj->flags |= VERTCENTER;

texterrors = savetexterrors;

#if debug
printf("discarding line_tc at end\n");
#endif
#if ndebug
printf("tobj->height=%x, flowdown=%x, end_of_text=%x\n", tobj->height, flowdown, end_of_text);
#endif
discard_text_ctrl(&tc);                /* cleanup */
discard_text_ctrl(&trial_tc);
discard_text_ctrl(&save_tc);
discard_text_ctrl(&wid1_tc);
discard_text_ctrl(&wid2_tc);
discard_text_ctrl(&line_tc);


#if debug
printf("exit format, final ch=%c (%x)\n", tc.ch, tc.ch);
#endif
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
int firstchar;

firstchar = TRUE;

#if debug
printf("in get_word\n");
#endif
start_trial();

hyphen_ok = TRUE;
if (trial_tc.state.flags&TEXTS_NOHYPHEN)
   hyphen_ok = FALSE;

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
   if (trial_tc.state.flags & TEXTS_NOHYPHEN)
      hyphen_ok = FALSE;
   if (firstchar) {
      firstchar = FALSE;
      if (!(trial_tc.state.flags & TEXTS_NOHYPHEN))
         hyphen_ok = TRUE;
   }
   do_char();
   while (trialch == CHAR_SOFTHYPHEN) {
      contains_softhyphen++;
      trialch = get_ch(&trial_tc);
   }
   trialblank = FALSE;
}
unget_ch(&trial_tc);
}


/*********************************************************************/
/* valid = get_softhyphen_word(first, max, num_hyphs);               */
/*                                                                   */
/* Gets a word, defined as a group of nonblank characters with       */
/* leading blanks, into the trial variables.  If first==TRUE, then   */
/* don't include the leading blanks.  The word will be terminated by */
/* a soft hyphen.  If there are more than one soft hyphens, the last */
/* one that will fit is used.  While parsing the word, if we go too  */
/* far, then we reset and recursively call get_softhyphen_word, with */
/* the num_hyphs reduced to match the number that will fit.          */
/* Returns TRUE if the trial variables are valid, FALSE if not (if   */
/* the line was too short for even one fragment).                    */
/*********************************************************************/

int get_softhyphen_word (first, max, num_hyphs)
int first;
COORD max;
int num_hyphs;
{
int hyphno;

#if debug
printf("get_softhyphen_word\n");
#endif
hyphno = 0;
start_trial();

if (num_hyphs <= 0)
   return FALSE;

while (hyphno < num_hyphs && trial_tc.x <= max) {

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

   if (trialch == CHAR_SOFTHYPHEN)        /* found a soft hyphen */
      hyphno++;
}

if (hyphno >= num_hyphs && trial_tc.x <= max) {    /* it fits */
   unget_ch(&trial_tc);
   force_do_char(HYPHENCHAR);
   trial_tc.line.flags |= TEXTL_HYPHEN;
   if (trial_tc.x <= max)                    /* still fits after - added */
      return TRUE;
}

discard_trial();           /* this break is too big... back up some */
if (hyphno >= num_hyphs)
   return get_softhyphen_word(first, max, hyphno-1);
else
   return get_softhyphen_word(first, max, hyphno);

}


/*********************************************************************/
/* valid = get_autohyphen_word(first, max);                          */
/*                                                                   */
/* Gets a auto-hyphenated word.  First, the word is collected into   */
/* a buffer.  Then, the auto_hyphen routine is called.  The number of*/
/* possible hyphen chars is counted, and the algorithm works like    */
/* softhyphen routine above (possibly doing a recursive call).       */
/* Returns TRUE if the trial variables are valid, FALSE if not (if   */
/* the line was too short for even one fragment).                    */
/*********************************************************************/

int get_autohyphen_word (first, max)
int first;
COORD max;
{
int num_hyphs;
char word[256];
int wi;
char *hyp;
char *auto_hyphenate();

#if debug
printf("get_autohyphen_word\n");
#endif
wi = 0;

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
   word[wi++] = trialch;
   do_char();
   trialblank = FALSE;
}

word[wi] = '\0';

#if hdebug
printf("about to hyphenate '%s'\n", word);
#endif
hyp = auto_hyphenate(word);
#if hdebug
printf("resultant string = '");
#endif

num_hyphs = 0;
for (wi=0; wi<strlen(word); wi++) {
#if hdebug
   printf("%d",hyp[wi]);
#endif
   if (hyp[wi]&1)
      num_hyphs++;
}
#if hdebug
printf("\n");
#endif

if (num_hyphs <= 0)           /* no place to hyphenate! */
   return FALSE;              /* so the trial is invalid */

discard_trial();
return get_autohyphen(max, num_hyphs, hyp);

}


/*********************************************************************/
/* valid = get_autohyphen(max, num_hyphs, hyp);                      */
/*                                                                   */
/* Gets as much of a word as it can, up to the last hyphen that will */
/* fit.  While parsing the word, if we go too far, then reset and    */
/* recursively call get_autohyphen with num_hyphs reduced to match   */
/* the number that will fit.                                         */
/* Returns TRUE if the trial variables are valid, FALSE if not (if   */
/* the line was too short for even one fragment).                    */
/*********************************************************************/

int get_autohyphen (max, num_hyphs, hyp)
COORD max;
int num_hyphs;
char *hyp;                    /* array marking hyph locations */
{
int hyphno;
int wi;

#if debug
printf("in get_autohyphen\n");
#endif
#if hdebug
printf("in get_autohyphen, num_hyph = %d\n", num_hyphs);
#endif
hyphno = 0;
wi = 0;

start_trial();

if (num_hyphs <= 0)
   return FALSE;                    /* trial is invalid... no chars! */

trialch = get_ch(&trial_tc);

while (discardable_spacechar(trialch)) {
   do_char();
   inc_space();
}

while (spacechar(trialch)) {
   do_char();
   inc_space();
}

while (hyphno < num_hyphs && trial_tc.x <= max) {
   while (wordchar(trialch) && ((hyp[wi]&1) == 0)) {
      do_char();
      trialblank = FALSE;
      wi++;
   }

   if (hyp[wi]&1) {                 /* found a hyphen location */
      do_char();
      trialblank = FALSE;
      wi++;
      hyphno++;
   }
}

if (hyphno >= num_hyphs && trial_tc.x <= max) {    /* it fits */
   unget_ch(&trial_tc);
   force_do_char(HYPHENCHAR);
   trial_tc.line.flags |= TEXTL_HYPHEN;
   if (trial_tc.x <= max)                    /* still fits after - added */
      return TRUE;
}

discard_trial();           /* this break is too big... back up some */
#if hdebug
printf("recursive hyphen call!\n");
#endif
if (hyphno >= num_hyphs)
   return get_autohyphen(max, hyphno-1, hyp);
else
   return get_autohyphen(max, hyphno, hyp);

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
struct text_ctrl hyph_tc;

#if debug
printf("in force_hyphenation\n");
#endif
hyph_tc.m = NULL;
hyph_tc.unget_macro = NULL;

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

   copy_text_ctrl(&trial_tc, &hyph_tc);         /* w/o hyphen */

   force_do_char(HYPHENCHAR);                   /* now with hyphen */

#if debug
   printf("force_hyphenation: trial_tc.x=%x, wmax=%x\n",trial_tc.x,wmax);
#endif
   if (trial_tc.x > wmax) {
      full = TRUE;                                 /* don't use this char */
      trialch = replace_text_ctrl(&hyph_tc, &trial_tc, NULL);
   }
   else {                                          /* use this char */
      trialch = replace_text_ctrl(&hyph_tc, &trial_tc, NULL);
      do_char();
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

#if debug
printf("in get_one_letter\n");
#endif
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

ch = replace_text_ctrl(&trial_tc, &tc, NULL);

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

copy_text_ctrl(&line_tc, &save_tc);
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

#if debug
printf("called save_widow\n");
#endif
copy_text_ctrl(&line_tc, &wid1_tc);
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
/* backup_line(tobj);                                                */
/*                                                                   */
/* Restore the line status from the save variables, freeing all      */
/* text_lines after the current line.  Tobj is needed by replace_    */
/* text_ctrl in case the lines being discarded are the first ones in */
/* the object, in order to reset the line list head pointer.         */
/*********************************************************************/

VOID backup_line(tobj)
struct text_object *tobj;
{

ch = replace_text_ctrl(&save_tc, &tc, tobj);
rmarg = savermarg;

}


/*********************************************************************/
/* backup_widow(tobj);                                               */
/*                                                                   */
/* Restore the line status from the widow variables, freeing all     */
/* text_lines after the current line.  Tobj is needed by replace_    */
/* text_ctrl in case the lines being discarded are the first ones in */
/* the object, in order to reset the line list head pointer.         */
/*********************************************************************/

VOID backup_widow(tobj)
struct text_object *tobj;
{

ch = replace_text_ctrl(&wid2_tc, &tc, tobj);
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
#if debug
printf("in strip_leading_blanks, ch=%c (%x)\n", ch, ch);
#endif
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
#if debug
printf("in copy_text_ctrl, final ch=%c (%x), loc=%c (%x)\n", to->ch, to->ch, *to->m->current, *to->m->current);
#endif

}


/************************************************************************/
/* ch = replace_text_ctrl(from, to, tobj);                              */
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
/* are freed starting at this point (the linked current line).  If the  */
/* previous line is NULL, then this is either the first line in a chain */
/* that has been linked, or it is totally unlinked.  If tobj is NULL,   */
/* we assume the line has never been linked and ignore the freeing.     */
/* If tobj is non-null, we assume it's the first of a chain that's been */
/* linked, so we free everything starting at tobj->line and zero out    */
/* the line list head pointer (tobj->line).  Returns the ch from the    */
/* text_ctrl struct which should be assigned to the corresponding ch    */
/* var, if any.  This is a convenience only.                            */
/************************************************************************/

char replace_text_ctrl(from, to, tobj)
struct text_ctrl *from, *to;
struct text_object *tobj;
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
#if debug
      printf("freeing line structure %x (len=%x)\n", p, sizeof(struct text_line));
#endif
      FreeMem(p, sizeof(struct text_line));
   }
   prev->next = NULL;
}
else if (prev == NULL && tobj != NULL && tobj->line != NULL) {
   for (p=tobj->line; p!=NULL; p=pnext) {    /* free any lines past where */
      pnext = p->next;                       /* this one will go          */
#if debug
      printf("freeing line structure %x (len=%x)\n", p, sizeof(struct text_line));
#endif
      FreeMem(p, sizeof(struct text_line));
   }
   tobj->line = NULL;
}
#if debug
printf("in replace_text_ctrl, final ch=%c (%x), loc=%c (%x)\n", to->ch, to->ch, *to->m->current, *to->m->current);
#endif
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

