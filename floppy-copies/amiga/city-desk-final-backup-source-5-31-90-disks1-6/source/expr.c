/*###################################################################*/
/* expr.c                                                            */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

#define debug 0

extern struct symtab res_table;
extern struct symtab var_table;

extern SHORT texterrors;
extern struct Window *clipboard;

extern UWORD xinch, yinch;

int int_expr(m)
struct macro **m;
{
SFIXED evaluate_expr();
int sign;

return ((evaluate_expr(m, NUM_DEF_NONE, &sign, NULL, NUM_COORD)
         + (1<<15)) >> 16);
}


SFIXED evaluate_expr(m, def, sign, tc, units)
struct macro **m;
int def;                /* NUM_DEF_INCH, NUM_DEF_POINT, or NUM_DEF_NONE */
int *sign;              /* TRUE iff leading sign (+/-) explicitly given */
struct text_ctrl *tc;   /* for param calls and vars, NULL if not used   */
int units;              /* NUM_COORD_X, NUM_COORD_Y, or NUM_COORD       */
{
char ch, op;
SFIXED val;
SFIXED num;
SFIXED get_num(), get_var(), get_param_expr();
short simple;           /* TRUE until we get an oper, var, or unit */
short unary;
int unitgiven;
char text1[80];

val = 0;
simple = TRUE;
unary = FALSE;

ch = M_GETCH(*m);

if (ch == '-' || ch == '+')
   *sign = TRUE;
else
   *sign = FALSE;

if (ch == '+') {
   M_NEXT(*m);
   ch = M_GETCH(*m);
}
if (ch == '-') {
   M_NEXT(*m);
   ch = M_GETCH(*m);
   unary = TRUE;
}

if (isdigit(ch) || ch == '.') {
   val = get_num(m, &unitgiven);
   if (unitgiven)
      simple = FALSE;
}
else if (isalpha(ch)) {
   val = get_var(m, tc);
   simple = FALSE;
}
else if (tc != NULL && ch == tc->state.paramstart) {
   val = get_param_expr(m, &tc->state, tc);
   simple = FALSE;
}
else {
   sprintf(text1, "near '%.15s'", (*m)->current);
   TEXTERROR("Invalid expression operand", text1);
   return 0;
}

if (unary)
   val = - val;


ch = M_GETCH(*m);

while (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
   simple = FALSE;
   op = ch;
   M_NEXT(*m);
   ch = M_GETCH(*m);
   num = 0;
   if (isdigit(ch) || ch == '.')
      num = get_num(m, &unitgiven);
   else if (isalpha(ch))
      num = get_var(m, tc);
   else if (tc != NULL && ch == tc->state.paramstart)
      num = get_param_expr(m, &tc->state, tc);
   else {
      sprintf(text1, "near '%.15s'", (*m)->current);
      TEXTERROR("Invalid expression operand", text1);
      return 0;
   }
   ch = M_GETCH(*m);
   switch (op) {
      case '+':
         val += num;
         break;
      case '-':
         val -= num;
         break;
      case '*':
         val = (SFIXED) (((double)val * (double)num) / 65536.0);
         break;
      case '/':
         val = (SFIXED) (((double)val / (double)num) * 65536.0);
         break;
   }
}

if (simple && def == NUM_DEF_INCH)
   val *= (72*8);
if (simple && def == NUM_DEF_POINT)
   val *= 8;

#if debug
printf("in evaluate_expr, before units conversion val=%d\n", val);
printf("val>>9=%d, yinch=%d, *yinch=%d, */576=%d\n",val>>9, yinch, (val>>9)*(int)yinch, ((val>>9)*(int)yinch)/576);
#endif
if (units == NUM_COORD_X)
   val = (((val>>9)*(int)xinch) / 576) << 9;    /* convert to X pixels */
else if (units == NUM_COORD_Y)
   val = (((val>>9)*(int)yinch) / 576) << 9;
#if debug
printf("in evaluate_expr, after units conversion val=%d\n", val);
#endif


return val;
}



SFIXED get_num(m, unitgiven)
struct macro **m;
int *unitgiven;
{
SFIXED val;
int decplace, decpt;
char ch, ch1, ch2;

*unitgiven = TRUE;

ch = M_GETCH(*m);
val = 0;
decplace = 10;
decpt = FALSE;
while (isdigit(ch) || ch == '.') {
   if (isdigit(ch)) {
      if (decpt) {
         val += (((ch)-'0') << 16) / decplace;
         decplace *= 10;
      }
      else {
         val = val * 10 + (((ch)-'0') << 16);
      }
   }
   if (ch == '.')
      decpt = TRUE;
   M_NEXT(*m);
   ch = M_GETCH(*m);
}

if (isalpha(ch) || ch == '\'' || ch == '\"') {     /* dimension */
   ch1 = ch;
   M_NEXT(*m);
   ch = M_GETCH(*m);
   if (ch1 != '\'' && ch1 != '\"' && isalpha(ch)) {
      ch2 = ch;
      M_NEXT(*m);
      ch = M_GETCH(*m);
   }
   else
      ch2 = 0;

   switch ((ch1<<8) | ch2) {
      case ('p'<<8 | 't'):                   /* points */
         val *= 8;
         break;
      case ('p'<<8 | 'c'):                   /* picas */
         val *= (12*8);
         break;
      case ('\"'<<8):                        /* inches */
      case ('i'<<8 | 'n'):
         val *= (72*8);
         break;
      case ('\''<<8):                        /* feet (!) */
      case ('f'<<8 | 't'):
         val *= (12*72*8);
         break;
      case ('c'<<8 | 'b'):                   /* cubits :-) */
         val *= (18*72*8);
         break;
      case ('c'<<8 | 'm'):                   /* centimeters */
         val *= (227);
         break;
      case ('m'<<8 | 'm'):                   /* millimeters */
         val *= (23);
         break;
      case ('n'<<8 | 'o'):                   /* no unit (number) */
         break;
      default:
         *unitgiven = FALSE;
   }
   while (isalpha(ch)) {               /* in case there's extra chars */
      M_NEXT(*m);
      ch = M_GETCH(*m);
   }
}
else {
   *unitgiven = FALSE;
}

return val;
}


SFIXED get_var(m, tc)
struct macro **m;
struct text_ctrl *tc;            /* for the true variables */
{
char key[256];
int i;
char ch;
struct symbol *sym;
struct symbol *search_table();
SFIXED get_res_value();

ch = M_GETCH(*m);
i = 0;
while (isalnum(ch)) {
   key[i++] = ch;
   M_NEXT(*m);
   ch = M_GETCH(*m);
}
key[i] = '\0';

sym = search_table(&res_table, key);
if (sym != NULL && tc != NULL)
   return get_res_value(tc, sym);
else {
   sym = search_table(&var_table, key);
   if (sym != NULL)
      return (SFIXED)(sym->value);
   else
      return 0;                        /*!!!!*/
}
}


SFIXED get_param_expr(m, state, tc)
struct macro **m;
struct text_state *state;
struct text_ctrl *tc;
{
char ch;
char text1[80], text2[80];
int i, sign;
int parnum;
struct parameter *param;
SFIXED val;
SFIXED evaluate_expr();
struct macro *start_macro(), *end_macro();

ch = M_GETCH(*m);             /* better be start parameter char ({) */
M_NEXT(*m);

parnum = int_expr(m) - 1;
if (parnum < 0)
   parnum = 0;

ch = M_GETCH(*m);
if (state == NULL || ch != state->paramend) {
   sprintf(text1, "near '%.15s'", (*m)->current);
   TEXTERROR("Invalid parameter reference", text1);
   return 0;
}

M_NEXT(*m);

/* If already inside a parameter, then any params encountered must be   */
/* expanded in the context of the calling macro, using param->context.  */

if ((*m)->macrotext->flags & MTEXT_PARAMETER) {
   param = ((struct parameter *)((*m)->macrotext))->context;
}
else {
   param = (*m)->param;
}

for (i=0; i<parnum; i++) {
   if (param != NULL)
      param = param->next;
   else
      break;
}
if (param == NULL) {
   sprintf(text1, "Parameter number %d is too big", parnum);
   sprintf(text2, "near '%.15s'", (*m)->current);
   TEXTERROR(text1, text2);
   return 0;
}

*m = start_macro(*m, &param->mtext, NULL);
val = evaluate_expr(m, NUM_DEF_NONE, &sign, tc, NUM_COORD);
*m = end_macro(*m);

return val;

}

