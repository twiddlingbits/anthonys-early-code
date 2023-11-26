/*###################################################################*/
/* cmd.c                                                             */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"
#include "math.h"

#define debug 0

#define MIN(x,y)  (((x) < (y)) ? (x) : (y))

/* CS_ are Command Sections, used to group functionally related         */
/* commands.  This way, commands can be added later and still stay with */
/* functional group.  The case statement should be in the same order    */
/* that the CMD_ (commands) are defined, to keep things readable.       */
/* If you extend a command section, make sure to adjust the immediately */
/* following CS_ definition to reflect the commands you added!          */

#define  ARG_NOCHECK    0x00010000
#define  ARG_0          0x00020000
#define  ARG_1          0x00040000
#define  ARG_1_2        0x00080000
#define  ARG_2          0x00100000
#define  ARG_2_3        0x00200000
#define  ARG_3_4        0x00400000

#define CS_JUSTIFY         1
#define CMD_LJ             CS_JUSTIFY+0   |  ARG_0
#define CMD_RJ             CS_JUSTIFY+1   |  ARG_0
#define CMD_FJ             CS_JUSTIFY+2   |  ARG_0
#define CMD_CJ             CS_JUSTIFY+3   |  ARG_0
#define CMD_NOCJ           CS_JUSTIFY+4   |  ARG_0

#define CS_MARGIN          CS_JUSTIFY+5
#define CMD_LM             CS_MARGIN+0    |  ARG_1
#define CMD_RM             CS_MARGIN+1    |  ARG_1
#define CMD_TM             CS_MARGIN+2    |  ARG_1
#define CMD_BM             CS_MARGIN+3    |  ARG_1
#define CMD_INDENT         CS_MARGIN+4    |  ARG_1
#define CMD_IRRMARGIN      CS_MARGIN+5    |  ARG_1

#define CS_PARAMETER       CS_MARGIN+6
#define CMD_LEADING        CS_PARAMETER+0 |  ARG_1
#define CMD_SPACING        CS_PARAMETER+1 |  ARG_1
#define CMD_PARSPACE       CS_PARAMETER+2 |  ARG_1
#define CMD_SENTSPACE      CS_PARAMETER+3 |  ARG_1
#define CMD_SPACESTRETCH   CS_PARAMETER+4 |  ARG_1
#define CMD_AUTOTHRESH     CS_PARAMETER+5 |  ARG_1
#define CMD_IRRMINWIDTH    CS_PARAMETER+6 |  ARG_1
#define CMD_HYPHENOFF      CS_PARAMETER+7 |  ARG_0
#define CMD_HYPHENON       CS_PARAMETER+8 |  ARG_0

#define CS_SKIPS           CS_PARAMETER+9
#define CMD_HSKIP          CS_SKIPS+0     |  ARG_1
#define CMD_VSKIP          CS_SKIPS+1     |  ARG_1
#define CMD_TABSET         CS_SKIPS+2     |  ARG_NOCHECK
#define CMD_TAB            CS_SKIPS+3     |  ARG_0
#define CMD_VCENTER        CS_SKIPS+4     |  ARG_0

#define CS_BREAK           CS_SKIPS+5
#define CMD_NOBREAK        CS_BREAK+0     |  ARG_0
#define CMD_BREAK          CS_BREAK+1     |  ARG_0
#define CMD_WIDOWON        CS_BREAK+2     |  ARG_0
#define CMD_WIDOWOFF       CS_BREAK+3     |  ARG_0
#define CMD_NEWPAGE        CS_BREAK+4     |  ARG_0
#define CMD_NEWCOLUMN      CS_BREAK+5     |  ARG_0

#define CS_MACRO           CS_BREAK+6
#define CMD_DEFINE         CS_MACRO+0     |  ARG_2
#define CMD_SET            CS_MACRO+1     |  ARG_2

#define CS_PRINT           CS_MACRO+2
#define CMD_PRINT          CS_PRINT+0     |  ARG_1
#define CMD_PRINTINT       CS_PRINT+1     |  ARG_1_2
#define CMD_PRINTDOL       CS_PRINT+2     |  ARG_1_2
#define CMD_PRINTDEC       CS_PRINT+3     |  ARG_2_3
#define CMD_PRINTROM       CS_PRINT+4     |  ARG_1
#define CMD_PRINTROMLC     CS_PRINT+5     |  ARG_1

#define CS_COND            CS_PRINT+6
#define CMD_IFEQ           CS_COND+0      |  ARG_3_4
#define CMD_IFNE           CS_COND+1      |  ARG_3_4
#define CMD_IFLT           CS_COND+2      |  ARG_3_4
#define CMD_IFLE           CS_COND+3      |  ARG_3_4
#define CMD_IFGT           CS_COND+4      |  ARG_3_4
#define CMD_IFGE           CS_COND+5      |  ARG_3_4

#define CS_SETCHAR         CS_COND+6
#define CMD_SETSTARTCHAR   CS_SETCHAR+0   |  ARG_1
#define CMD_SETENDCHAR     CS_SETCHAR+1   |  ARG_1
#define CMD_SETSHYPHCHAR   CS_SETCHAR+2   |  ARG_1
#define CMD_SETTABCHAR     CS_SETCHAR+3   |  ARG_1
#define CMD_SETTIECHAR     CS_SETCHAR+4   |  ARG_1
#define CMD_SETPSTARTCHAR  CS_SETCHAR+5   |  ARG_1
#define CMD_SETPENDCHAR    CS_SETCHAR+6   |  ARG_1

#define CS_MAGNIFY         CS_SETCHAR+7
#define CMD_MAG            CS_MAGNIFY+0   |  ARG_1
#define CMD_MAGX           CS_MAGNIFY+1   |  ARG_1
#define CMD_MAGY           CS_MAGNIFY+2   |  ARG_1

#define CS_STYLE           CS_MAGNIFY+3
#define CMD_BOLD           CS_STYLE+0     |  ARG_0
#define CMD_NOBOLD         CS_STYLE+1     |  ARG_0
#define CMD_ITALIC         CS_STYLE+2     |  ARG_0
#define CMD_NOITALIC       CS_STYLE+3     |  ARG_0
#define CMD_UNDERLINED     CS_STYLE+4     |  ARG_0
#define CMD_NOUNDERLINED   CS_STYLE+5     |  ARG_0
#define CMD_COMPRESS       CS_STYLE+6     |  ARG_0
#define CMD_NOCOMPRESS     CS_STYLE+7     |  ARG_0
#define CMD_PLAIN          CS_STYLE+8     |  ARG_0
#define CMD_DEFAULT        CS_STYLE+9     |  ARG_0

#define CS_FONT            CS_STYLE+10
#define CMD_FSIZE          CS_FONT+0      |  ARG_1
#define CMD_FONT           CS_FONT+1      |  ARG_1_2
#define CMD_SAVEFONT       CS_FONT+2      |  ARG_0
#define CMD_RESTOREFONT    CS_FONT+3      |  ARG_0

struct symbol command_table[] = {
   {"autothresh",    CMD_AUTOTHRESH},
   {"bd",            CMD_BOLD},
   {"bm",            CMD_BM},
   {"bold",          CMD_BOLD},
   {"break",         CMD_BREAK},
   {"cj",            CMD_CJ},
   {"compress",      CMD_COMPRESS},
   {"def",           CMD_DEFINE},
   {"default",       CMD_DEFAULT},
   {"define",        CMD_DEFINE},
   {"fj",            CMD_FJ},
   {"font",          CMD_FONT},
   {"fsize",         CMD_FSIZE},
   {"hskip",         CMD_HSKIP},
   {"hyphenoff",     CMD_HYPHENOFF},
   {"hyphenon",      CMD_HYPHENON},
   {"ifeq",          CMD_IFEQ},
   {"ifge",          CMD_IFGE},
   {"ifgt",          CMD_IFGT},
   {"ifle",          CMD_IFLE},
   {"iflt",          CMD_IFLT},
   {"ifne",          CMD_IFNE},
   {"indent",        CMD_INDENT},
   {"irrmargin",     CMD_IRRMARGIN},
   {"irrminwidth",   CMD_IRRMINWIDTH},
   {"it",            CMD_ITALIC},
   {"italic",        CMD_ITALIC},
   {"leading",       CMD_LEADING},
   {"lj",            CMD_LJ},
   {"lm",            CMD_LM},
   {"mag",           CMD_MAG},
   {"magx",          CMD_MAGX},
   {"magy",          CMD_MAGY},
   {"newcolumn",     CMD_NEWCOLUMN},
   {"newpage",       CMD_NEWPAGE},
   {"nobd",          CMD_NOBOLD},
   {"nobold",        CMD_NOBOLD},
   {"nobreak",       CMD_NOBREAK},
   {"nocj",          CMD_NOCJ},
   {"nocompress",    CMD_NOCOMPRESS},
   {"noit",          CMD_NOITALIC},
   {"noitalic",      CMD_NOITALIC},
   {"noun",          CMD_NOUNDERLINED},
   {"nounder",       CMD_NOUNDERLINED},
   {"nounderlined",  CMD_NOUNDERLINED},
   {"p",             CMD_PLAIN},
   {"parspace",      CMD_PARSPACE},
   {"pl",            CMD_PLAIN},
   {"plain",         CMD_PLAIN},
   {"print",         CMD_PRINT},
   {"printdec",      CMD_PRINTDEC},
   {"printdol",      CMD_PRINTDOL},
   {"printint",      CMD_PRINTINT},
   {"printrom",      CMD_PRINTROM},
   {"printromlc",    CMD_PRINTROMLC},
   {"restorefont",   CMD_RESTOREFONT},
   {"rj",            CMD_RJ},
   {"rm",            CMD_RM},
   {"savefont",      CMD_SAVEFONT},
   {"sentspace",     CMD_SENTSPACE},
   {"set",           CMD_SET},
   {"setendchar",    CMD_SETENDCHAR},
   {"setpendchar",   CMD_SETPENDCHAR},
   {"setpstartchar", CMD_SETPSTARTCHAR},
   {"setsofthyphenchar", CMD_SETSHYPHCHAR},
   {"setstartchar",  CMD_SETSTARTCHAR},
   {"settabchar",    CMD_SETTABCHAR},
   {"settiechar",    CMD_SETTIECHAR},
   {"spacestretch",  CMD_SPACESTRETCH},
   {"spacing",       CMD_SPACING},
   {"tab",           CMD_TAB},
   {"tabset",        CMD_TABSET},
   {"tm",            CMD_TM},
   {"un",            CMD_UNDERLINED},
   {"under",         CMD_UNDERLINED},
   {"underlined",    CMD_UNDERLINED},
   {"vcenter",       CMD_VCENTER},
   {"vskip",         CMD_VSKIP},
   {"widowoff",      CMD_WIDOWOFF},
   {"widowon",       CMD_WIDOWON}
};


struct symtab cmd_table = {command_table,
                           sizeof(command_table)/sizeof(struct symbol),
                           sizeof(command_table)/sizeof(struct symbol),
                           0
                          };

struct symtab sym_table = {NULL, 0, 0,
               SYM_DYNAMIC | SYM_REDEF_WARN | SYM_FREEKEY | SYM_FREEVALUE};

struct symtab var_table = {NULL, 0, 0,
                              SYM_DYNAMIC | SYM_REDEF_WARN | SYM_FREEKEY};

extern struct symtab res_table;     /* declared in resvar.c */

struct symtab font_table = {NULL, 0, 0, SYM_DYNAMIC};

struct symtab lfont_table = {NULL, 0, 0, SYM_DYNAMIC};

struct text_state *first_state;  /** points to this chain's default state */

extern short text_change;     /* set to TRUE on font, mag, etc. changes */

extern COORD opt_char_height, opt_char_depth;
COORD char_height(), char_depth();

extern SHORT texterrors;
extern struct Window *clipboard;


/************************************************************************/
/* process_cmd(tc, sym, param);                                         */
/*                                                                      */
/* Processes a command, given the current text_ctrl struct (which       */
/* includes text_state), the command, and the parameter list.           */
/* Returns either '\0' or a special char such as CHAR_HSKIP.            */
/************************************************************************/

char process_cmd(tc, sym, param)
struct text_ctrl *tc;
struct symbol *sym;
struct parameter *param;
{
short newfont;
struct text_state *state;
int i;
int sign;
char ch;
char fontname[FONTNAMESIZE];
int len;
char *key;
struct macro_text *mt;
COORD val;
SFIXED fval, fval2;
float floatval;
int width, prec;
char text1[256];
struct symbol *symbol;
struct symbol *search_table();
struct macro *start_macro();
char *AllocMem();
COORD get_num_arg();
FIXED get_fixed_arg();
SFIXED get_sfixed_arg();
VOID get_string_arg();
char get_char_arg();
VOID print_roman();

ch = '\0';
newfont = FALSE;
fontname[0] = '\0';

state = &tc->state;

if (sym->value & ARG_0) {           /* no args allowed */
   if (param != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 0");
   }
}
if (sym->value & ARG_1) {           /* one and only one arg */
   if (param == NULL) {
      sprintf(text1, "Command '%s' has too few arguments", sym->key);
      TEXTERROR(text1, "Should have 1");
      return ch;
   }
   if (param->next != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 1");
   }
}
if (sym->value & ARG_1_2) {         /* one or two args only */
   if (param == NULL) {
      sprintf(text1, "Command '%s' has too few arguments", sym->key);
      TEXTERROR(text1, "Should have 1 or 2");
      return ch;
   }
   if (param->next != NULL && param->next->next != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 1 or 2");
   }
}
if (sym->value & ARG_2) {           /* two args only */
   if (param == NULL || param->next == NULL) {
      sprintf(text1, "Command '%s' has too few arguments", sym->key);
      TEXTERROR(text1, "Should have 2");
      return ch;
   }
   if (param->next->next != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 2");
   }
}
if (sym->value & ARG_2_3) {         /* two or three args only */
   if (param == NULL || param->next == NULL) {
      sprintf(text1, "Command '%s' has too few arguments", sym->key);
      TEXTERROR(text1, "Should have 2 or 3");
      return ch;
   }
   if (param->next->next != NULL && param->next->next->next != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 2 or 3");
   }
}
if (sym->value & ARG_3_4) {         /* three or four args only */
   if (param == NULL || param->next == NULL || param->next->next == NULL) {
      sprintf(text1, "Command '%s' has too few arguments", sym->key);
      TEXTERROR(text1, "Should have 3 or 4");
      return ch;
   }
   if (param->next->next->next != NULL && param->next->next->next->next != NULL) {
      sprintf(text1, "Command '%s' has too many arguments", sym->key);
      TEXTERROR(text1, "Should have 3 or 4");
   }
}

switch (sym->value) {

/********** Justification commands **********/

case CMD_LJ:
   state->flags &= ~TEXTS_RAGLEFT;
   state->flags |= TEXTS_RAGRIGHT;
   break;

case CMD_RJ:
   state->flags &= ~TEXTS_RAGRIGHT;
   state->flags |= TEXTS_RAGLEFT;
   break;

case CMD_FJ:
   state->flags &= ~(TEXTS_RAGRIGHT|TEXTS_RAGLEFT);
   break;

case CMD_CJ:
   state->flags &= ~(TEXTS_SAVERAGR|TEXTS_SAVERAGL);  /* clear save */
   if (state->flags & TEXTS_RAGRIGHT)              /* save justify mode */
      state->flags |= TEXTS_SAVERAGR;
   if (state->flags & TEXTS_RAGLEFT)
      state->flags |= TEXTS_SAVERAGL;
   state->flags |= TEXTS_RAGRIGHT|TEXTS_RAGLEFT;      /* set cj */
   break;

case CMD_NOCJ:
   state->flags &= ~(TEXTS_RAGRIGHT|TEXTS_RAGLEFT);   /* clear j mode */
   if (state->flags & TEXTS_SAVERAGR)           /* restore justify mode */
      state->flags |= TEXTS_RAGRIGHT;
   if (state->flags & TEXTS_SAVERAGL)
      state->flags |= TEXTS_RAGLEFT;
   break;

/********** Margin commands **********/

case CMD_LM:
   state->left_margin =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_RM:
   state->right_margin =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_BM:
   state->bottom_margin =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
   break;

case CMD_TM:
   state->top_margin =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
   break;

case CMD_INDENT:
   state->paragraph_indent =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_IRRMARGIN:
   state->irreg_margin =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

/********** Parameter commands **********/

case CMD_LEADING:
   state->leading =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
   state->flags |= TEXTS_USELEADING;
   break;

case CMD_SPACING:
   state->spacing =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
   state->flags &= ~TEXTS_USELEADING;
   break;

case CMD_PARSPACE:
   state->paragraph_space =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
   break;

case CMD_SENTSPACE:
   state->sentence_space =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_SPACESTRETCH:
   state->max_space_stretch =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_AUTOTHRESH:
   state->autohyph_threshhold =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_IRRMINWIDTH:
   state->irreg_minwidth =
               get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   break;

case CMD_HYPHENOFF:
   state->flags |= TEXTS_NOHYPHEN;
   break;

case CMD_HYPHENON:
   state->flags &= ~TEXTS_NOHYPHEN;
   break;

/********** Skip commands **********/

case CMD_HSKIP:
   val = get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
   if (sign) {
      tc->x += val;              /* relative skip */
      if (tc->x < 0)
         tc->x = 0;
      ch = CHAR_HSKIP;
   }
   else {
      tc->x = val;               /* absolute skip */
      ch = CHAR_HPOS;
   }
   break;

case CMD_VSKIP:
   val = get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_Y);
#if debug
   printf("vskip val=%d\n", val);
#endif
   if (sign)
      state->rel_vskip -= val;      /* negative so up is positive to user */
   else
      state->rel_vskip = -val;
#if debug
   printf("vskip rel_vskip = %d\n", state->rel_vskip);
#endif
   text_change = TRUE;
   ch = CHAR_VSKIP;
   break;

case CMD_TABSET:
   for (i=0; i<NUM_TABS; i++)
      state->tabs[i] = 0;
   i = 0;
   while (param != NULL) {
      val = get_num_arg(param, NUM_DEF_INCH, &sign, tc, NUM_COORD_X);
      if (i < NUM_TABS)
         state->tabs[i++] = val;
      param = param->next;
   }
   break;

case CMD_TAB:
   ch = CHAR_TAB;
   break;

case CMD_VCENTER:
   state->flags |= TEXTS_VCENTER;
   break;

/********** Break commands **********/

case CMD_NOBREAK:
   state->flags |= TEXTS_NOBREAK;
   break;

case CMD_BREAK:
   state->flags &= ~TEXTS_NOBREAK;
   break;

case CMD_WIDOWON:
   state->flags &= ~TEXTS_NOWIDOW;
   break;

case CMD_WIDOWOFF:
   state->flags |= TEXTS_NOWIDOW;
   break;

case CMD_NEWPAGE:
   state->flags |= TEXTS_FORCEPAGEEND;
   break;

case CMD_NEWCOLUMN:
   state->flags |= TEXTS_FORCECOLEND;
   break;

/********** Macro commands **********/

case CMD_DEFINE:
   len = param->mtext.end - param->mtext.start;    /* get key */
   key = AllocMem(len+1, 0);
   if (key == NULL) {
      TEXTERROR("Out of memory", "Can't define new macro");
      break;
   }
   strncpy(key, param->mtext.start, len);
   key[len] = '\0';

   mt = (struct macro_text *)AllocMem(sizeof(struct macro_text), 0);
   if (mt == NULL) {
      FreeMem(key, strlen(key)+1);
      TEXTERROR("Out of memory", "Can't define new macro");
      break;
   }

   len = param->next->mtext.end - param->next->mtext.start;
   mt->start = AllocMem(len+1, 0);                    /* get definition */
   if (mt->start == NULL) {
      FreeMem(key, strlen(key));
      FreeMem(mt, sizeof(*mt));
      TEXTERROR("Out of memory", "Can't define new macro");
      break;
   }
   strncpy(mt->start, param->next->mtext.start, len);
   mt->end = mt->start + len;
   mt->ref_count = 0;
   mt->flags = 0;

   add_symbol(&sym_table, key, mt);

   break;

case CMD_SET:
   len = param->mtext.end - param->mtext.start;    /* get key */
   key = AllocMem(len+1, 0);
   if (key == NULL) {
      TEXTERROR("Out of memory", "Can't set variable");
      break;
   }
   strncpy(key, param->mtext.start, len);
   key[len] = '\0';

   fval = get_sfixed_arg(param->next, tc);

   symbol = search_table(&res_table, key);
   if (symbol == NULL)                       /* not a reserved var */
      add_symbol(&var_table, key, fval);
   else
      set_res_value(tc, symbol, fval);       /* reserved var */

   break;

/********** Print commands **********/

case CMD_PRINT:
   fval = get_sfixed_arg(param, tc);
   if ((fval & 0xFFFF) == 0)
      sprintf(text1, "%d", (fval+(1<<15))>>16);    /* integer */
   else
      sprintf(text1, "%f", (float)fval / 65536.);  /* float */

#if debug
   printf("val=%d, string='%s'\n", fval, text1);
#endif

   goto printcmd;

case CMD_PRINTINT:
   fval = get_sfixed_arg(param, tc);
   fval = (fval+(1<<15))>>16;          /* round off the integer */

   if (param->next != NULL) {          /* allow optional maximum size */
      width = get_num_arg(param->next, NUM_DEF_NONE, &sign, tc, NUM_COORD);
      sprintf(text1, "%*d", width, (int)fval);
   }
   else
      sprintf(text1, "%d", (int)fval);

#if debug
   printf("val=%d, width=%d, string='%s'\n", fval, width, text1);
#endif

   goto printcmd;

case CMD_PRINTDOL:
   fval = get_sfixed_arg(param, tc);
   floatval = (float)fval / 65536.;    /* float */
   floatval = ((int)(floatval*100.0+0.51)) / 100.0; /* round to 2 dec plc */

   if (param->next != NULL) {          /* allow optional maximum size */
      width = get_num_arg(param->next, NUM_DEF_NONE, &sign, tc, NUM_COORD);
      sprintf(text1, "%*.2f", width, floatval);
   }
   else
      sprintf(text1, "%.2f", floatval);

#if debug
   printf("val=%f, width=%d, string='%s'\n", (float)fval/65536., width, text1);
#endif

   goto printcmd;

case CMD_PRINTDEC:
   fval = get_sfixed_arg(param, tc);
                                                            /* precision */
   prec = get_num_arg(param->next, NUM_DEF_NONE, &sign, tc, NUM_COORD);
   floatval = (float)fval / 65536.;    /* float */
   floatval = ((int)(floatval*pow(10.0,(double)prec)+0.51)) /
                              pow(10.0,(double)prec);          /* round */

   if (param->next->next != NULL) {       /* allow optional maximum size */
      width = get_num_arg(param->next->next, NUM_DEF_NONE, &sign, tc,
                                                            NUM_COORD);
      sprintf(text1, "%*.*f", width, prec, floatval);
   }
   else
      sprintf(text1, "%.*f", prec, floatval);

#if debug
   printf("val=%f, width=%d, prec=%d string='%s'\n", (float)fval/65536., width, prec, text1);
#endif

   goto printcmd;

case CMD_PRINTROM:
   fval = get_sfixed_arg(param, tc);         /* capital Roman numeral */
   fval = (fval+(1<<15))>>16;          /* round off the integer */

   print_roman((int)fval, text1);

#if debug
   printf("val=%f, string='%s'\n", fval,text1);
#endif

   goto printcmd;

case CMD_PRINTROMLC:
   fval = get_sfixed_arg(param, tc);         /* lower case Roman numeral */
   fval = (fval+(1<<15))>>16;          /* round off the integer */

   print_roman(fval, text1);
   strlwr(text1);                      /* convert to lower case */

#if debug
   printf("val=%f, string='%s'\n", fval,text1);
#endif

   goto printcmd;

printcmd:

   mt = (struct macro_text *)AllocMem(sizeof(struct macro_text),0);
   if (mt == NULL) {
      TEXTERROR("Out of memory", "can't print expression");
      break;
   }
   mt->ref_count = 0;
   mt->flags = MTEXT_FREETEXT | MTEXT_FREEMTEXT;
   mt->start = (char *)AllocMem(strlen(text1)+1, 0);
   if (mt->start == NULL) {
      FreeMem(mt, sizeof(struct macro_text));
      TEXTERROR("Out of memory", "can't print expression");
      break;
   }
   strcpy(mt->start, text1);
   mt->end = mt->start + strlen(text1) + 1;

   tc->m = start_macro(tc->m, mt, NULL);

   break;

/********** Conditional commands **********/

case CMD_IFEQ:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval == fval2)
      goto ifthen;
   goto ifelse;

case CMD_IFNE:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval != fval2)
      goto ifthen;
   goto ifelse;

case CMD_IFLT:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval < fval2)
      goto ifthen;
   goto ifelse;

case CMD_IFLE:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval <= fval2)
      goto ifthen;
   goto ifelse;

case CMD_IFGT:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval > fval2)
      goto ifthen;
   goto ifelse;

case CMD_IFGE:
   fval = get_sfixed_arg(param, tc);         /* get first number */
   fval2 = get_sfixed_arg(param->next, tc);  /* get second number */
   if (fval >= fval2)
      goto ifthen;
   goto ifelse;

ifthen:
   tc->m = start_macro(tc->m, &param->next->next->mtext, NULL);
   break;

ifelse:
   if (param->next->next->next != NULL)
      tc->m = start_macro(tc->m, &param->next->next->next->mtext, NULL);
   break;


/********** Setchar commands **********/

case CMD_SETSTARTCHAR:
   state->escstart = get_char_arg(param);
   break;

case CMD_SETENDCHAR:
   state->escend = get_char_arg(param);
   break;

case CMD_SETSHYPHCHAR:
   state->softhyphen = get_char_arg(param);
   break;

case CMD_SETTABCHAR:
   state->tabchar = get_char_arg(param);
   break;

case CMD_SETTIECHAR:
   state->tiechar = get_char_arg(param);
   break;

case CMD_SETPSTARTCHAR:
   state->paramstart = get_char_arg(param);
   break;

case CMD_SETPENDCHAR:
   state->paramend = get_char_arg(param);
   break;

/********** Magnify commands **********/

case CMD_MAG:
   state->magx = get_fixed_arg(param, tc);
   state->magy = state->magx;
   text_change = TRUE;
   break;

case CMD_MAGX:
   state->magx = get_fixed_arg(param, tc);
   text_change = TRUE;
   break;

case CMD_MAGY:
   state->magy = get_fixed_arg(param, tc);
   text_change = TRUE;
   break;

/********** Style commands **********/

case CMD_BOLD:
   state->style |= BOLD;
   newfont = TRUE;
   break;

case CMD_NOBOLD:
   state->style &= ~BOLD;
   newfont = TRUE;
   break;

case CMD_ITALIC:
   state->style |= ITALIC;
   newfont = TRUE;
   break;

case CMD_NOITALIC:
   state->style &= ~ITALIC;
   newfont = TRUE;
   break;

case CMD_UNDERLINED:
   state->style |= UNDERLINE;
   newfont = TRUE;
   break;

case CMD_NOUNDERLINED:
   state->style &= ~UNDERLINE;
   newfont = TRUE;
   break;

case CMD_COMPRESS:
   state->style |= COMPRESS;
   newfont = TRUE;
   break;

case CMD_NOCOMPRESS:
   state->style &= ~COMPRESS;
   newfont = TRUE;
   break;

case CMD_PLAIN:
   state->style = 0;
   newfont = TRUE;
   break;

case CMD_DEFAULT:
   state->font_size = first_state->font_size;
   state->font = first_state->font;
   state->lfont = first_state->lfont;
   state->style = first_state->style;
   set_font(state);
   text_change = TRUE;
   break;

/********** Font commands **********/

case CMD_FSIZE:
   state->font_size =
                  get_num_arg(param, NUM_DEF_POINT, &sign, tc, NUM_COORD);
   newfont = TRUE;
   break;

case CMD_FONT:
   get_string_arg(param, fontname, FONTNAMESIZE);
   if (param->next != NULL)            /* allow optional font size */
      state->font_size =
            get_num_arg(param->next, NUM_DEF_POINT, &sign, tc, NUM_COORD);
   newfont = TRUE;
   break;

case CMD_SAVEFONT:
   if (state->lfont) {
      state->savefont = state->lfont;
      state->flags |= TEXTS_SAVEFONTLAS;
   }
   else {
      state->savefont = (struct laserfont *)state->font;
      state->flags &= ~ TEXTS_SAVEFONTLAS;
   }
   break;

case CMD_RESTOREFONT:
   if (state->savefont == NULL) {
      TEXTERROR("Font can not be restored", "since it hasn't been saved!");
   }
   else {
      if (state->flags & TEXTS_SAVEFONTLAS) {
         state->lfont = state->savefont;
         state->font = state->lfont->screenfont;
      }
      else {
         state->lfont = NULL;
         state->font = (struct TextFont *)state->savefont;
      }
      text_change = TRUE;
   }
   break;

default:
   sprintf(text1, "Invalid command, code = %x", sym->value);
   TEXTERROR("Internal program error!", text1);
   break;

}

/* Maintain the rest of this function in parallel with the end of */
/* process_font_cmd()!!!                                          */

if (newfont) {
   text_change = TRUE;
   if (*fontname == '\0') {
      if (state->lfont)
         strncpy(fontname, state->lfont->fontname, FONTNAMESIZE);
      else
         strncpy(fontname,
                 state->font->tf_Message.mn_Node.ln_Name, FONTNAMESIZE);
   }
   my_open_font(fontname, state, 0);
}

if (text_change) {
   opt_tptoc(state);
   opt_char_height = char_height(' ', state);
   opt_char_depth = char_depth(' ', state);
#if debug
   printf("cmd text_change, char_height=%d, char_depth=%d\n", opt_char_height, opt_char_depth);
#endif
}

return ch;

}


/************************************************************************/
/* process_font_cmd(tc, fontname, param);                               */
/*                                                                      */
/* Processes a font command (given like [topaz 8], i.e. an implicit     */
/* command) given the current text_ctrl struct (which includes          */
/* text_state), the font name, and the parameter list (for the size).   */
/************************************************************************/

VOID process_font_cmd(tc, fontname, param)
struct text_ctrl *tc;
char *fontname;
struct parameter *param;
{
struct text_state *state;
int sign;
COORD get_num_arg();

state = &tc->state;

if (param != NULL) {                /* check for only 0 or 1 args */
   if (param->next != NULL) {
      TEXTERROR("Font change has too many arguments", "Should have 0 or 1");
   }
}

if (param != NULL)
   state->font_size =
               get_num_arg(param, NUM_DEF_POINT, &sign, tc, NUM_COORD);

/* Maintain the rest of this function in parallel with the end */
/* of process_cmd()!!                                          */

text_change = TRUE;

/* *fontname will never be null, as it's an explicit font command!    */
/* (which has already been searched for and found in the font tables) */

my_open_font(fontname, state, 0);

if (text_change) {
   opt_tptoc(state);
   opt_char_height = char_height(' ', state);
   opt_char_depth = char_depth(' ', state);
}

}


/*********************************************************************/
/* val = get_num_arg(param, def, &sign, tc, units);                  */
/*                                                                   */
/* Gets a single numeric argument with the given default units.      */
/* Returns the value in integer COORDs.  *sign is TRUE iff there was */
/* a leading sign (+/-) on the expression.  Units selects either     */
/* X, Y, or old coordinates.                                         */
/*********************************************************************/

COORD get_num_arg(param, def, sign, tc, units)
struct parameter *param;
int def;
int *sign;
struct text_ctrl *tc;
int units;                    /* NUM_COORD_X, NUM_COORD_Y, or NUM_COORD */
{
struct macro *m, *start_macro();
SFIXED val, evaluate_expr();

if (param != NULL) {
   m = start_macro(NULL, &param->mtext, NULL);

   val = evaluate_expr(&m, def, sign, tc, units);

   end_macro(m);
}
else
   val = 0;

return (COORD)((val+(1<<15)) >> 16);

}


/*********************************************************************/
/* val = get_fixed_arg(param, tc);                                   */
/*                                                                   */
/* Gets a single fixed-point argument.                               */
/*********************************************************************/

FIXED get_fixed_arg(param, tc)
struct parameter *param;
struct text_ctrl *tc;
{
struct macro *m, *start_macro();
FIXED val;
SFIXED evaluate_expr();
int sign;

if (param != NULL) {
   m = start_macro(NULL, &param->mtext, NULL);

   val = evaluate_expr(&m, NUM_DEF_NONE, &sign, tc, NUM_COORD);

   end_macro(m);
}
else
   val = 0;

return val;

}

/*********************************************************************/
/* val = get_sfixed_arg(param, tc);                                  */
/*                                                                   */
/* Gets a single fixed-point signed argument.                        */
/*********************************************************************/

SFIXED get_sfixed_arg(param, tc)
struct parameter *param;
struct text_ctrl *tc;
{
struct macro *m, *start_macro();
SFIXED val;
SFIXED evaluate_expr();
int sign;

if (param != NULL) {
   m = start_macro(NULL, &param->mtext, NULL);

   val = evaluate_expr(&m, NUM_DEF_NONE, &sign, tc, NUM_COORD);

   end_macro(m);
}
else
   val = 0;

return val;

}


/*********************************************************************/
/* val = get_string_arg(param, str, maxlen);                         */
/*                                                                   */
/* Copies a single string argument with maximum length 'maxlen' into */
/* 'str'.                                                            */
/*********************************************************************/

VOID get_string_arg(param, str, maxlen)
struct parameter *param;
char *str;
int maxlen;
{
int len;

len = param->mtext.end - param->mtext.start;
len = MIN(len, maxlen-1);

strncpy(str, param->mtext.start, len);
str[len] = '\0';

}


/*********************************************************************/
/* val = get_char_arg(param);                                        */
/*                                                                   */
/* Gets a single-character argument.                                 */
/*********************************************************************/

char get_char_arg(param)
struct parameter *param;
{
struct macro *m, *start_macro();
char ch;
char get_quoted_char();

if (param != NULL) {
   m = start_macro(NULL, &param->mtext, NULL);

   ch = M_GETCH(m);
   if (ch == '\\') {                   /* escaped character */
      M_NEXT(m);
      ch = get_quoted_char(m);
   }

   end_macro(m);
}
else
   ch = '\0';

return ch;

}


/*********************************************************************/
/* val = get_fixed(cp);                                              */
/*                                                                   */
/* Gets a single fixed-point number given a double pointer to a      */
/* string.  Creates its on macro_text.  This routine is only for     */
/* compatibility with old stuff.  New routines should use their own  */
/* macro_text structs.  Units is NUM_COORD_X, NUM_COORD_Y, or        */
/* NUM_COORD to return X, Y, or old coordinates.                     */
/*********************************************************************/

FIXED get_fixed(cp, units)
char **cp;
int units;              /* NUM_COORD_X, NUM_COORD_Y, or NUM_COORD */
{
struct macro_text mt;
struct macro *m, *start_macro();
FIXED val;
SFIXED evaluate_expr();
int sign;

mt.start = *cp;
mt.end = *cp + strlen(*cp);
mt.ref_count = 0;
mt.flags = 0;

m = start_macro(NULL, &mt, NULL);

val = evaluate_expr(&m, NUM_DEF_NONE, &sign, NULL, units);

end_macro(m);

return val;

}


/*********************************************************************/
/* free_all_symtabs();                                               */
/*                                                                   */
/* This routine frees up all the symbol tables.  It is in this       */
/* module merely because this is where all the symbol tables are     */
/* declared.  It should probably really go in symtab.c.              */
/*********************************************************************/

VOID free_all_symtabs()
{
VOID free_symbol_table();

free_symbol_table(&cmd_table);
free_symbol_table(&sym_table);
free_symbol_table(&var_table);
free_symbol_table(&res_table);

/* font_table and lfont_table are cleaned up by close_fonts and   */
/* closelaserfonts, respectively.                                 */

}


/*********************************************************************/
/* print_roman(val, string);                                         */
/*                                                                   */
/* This routine takes a value up to 32768 and returns it as a Roman  */
/* numeral in capital letters in string.                             */
/*********************************************************************/

VOID print_roman(val, string)
int val;
char *string;
{
static char *roman_letters[][10] = {
   {"", "[un]X[noun]", "[un]XX[noun]", "[un]XXX[noun]", "BUG4", "BUG5",
            "BUG6", "BUG7", "BUG8", "BUG9"},
   {"", "M", "MM", "MMM", "MMMM", "[un]V[noun]", "[un]V[noun]M",
            "[un]V[noun]MM", "[un]V[noun]MMM", "M[un]X[noun]"},
   {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
   {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"},
   {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"}
};

*string = '\0';

if (val < 0) {
   strcat(string, "-");
   val = -val;
}

if (val == 0) {
   strcat(string, "0");
   return;
}

strcat(string, roman_letters[0][val/10000]);
val %= 10000;
strcat(string, roman_letters[1][val/1000]);
val %= 1000;
strcat(string, roman_letters[2][val/100]);
val %= 100;
strcat(string, roman_letters[3][val/10]);
val %= 10;
strcat(string, roman_letters[4][val]);

return;

}

