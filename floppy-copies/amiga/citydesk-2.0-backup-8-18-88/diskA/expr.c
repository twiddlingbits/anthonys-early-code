/*###################################################################*/
/* expr.c                                                            */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

extern struct symtab res_table;
extern struct symtab var_table;

int int_expr(m)
struct macro **m;
{
SFIXED evaluate_expr();
int sign;

return ((evaluate_expr(m, NUM_DEF_NONE, &sign) + (1<<15)) >> 16);
}


SFIXED evaluate_expr(m, def, sign)
struct macro **m;
int def;                /* NUM_DEF_INCH, NUM_DEF_POINT, or NUM_DEF_NONE */
int *sign;              /* TRUE iff leading sign (+/-) explicitly given */
{
char ch, op;
SFIXED val;
SFIXED num;
SFIXED get_num(), get_var();
short simple;           /* TRUE until we get an oper, var, or unit */
short unary;
int unitgiven;

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
   val = get_var(m);
   simple = FALSE;
}
else
   printf("bad expr!!!\n");         /*!!!!*/

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
      num = get_var(m);
   else
      printf("bad expr2!!!!\n");                   /*!!!!*/
   switch (op) {
      case '+':
         val += num;
         break;
      case '-':
         val -= num;
         break;
      case '*':
         val = (val>>8) * (num>>8);             /*!!!!*/
         break;
      case '/':
         val = (val / (num>>8)) << 8;           /*!!!!*/
         break;
   }
}

if (simple && def == NUM_DEF_INCH)
   val *= (72*8);
if (simple && def == NUM_DEF_POINT)
   val *= 8;

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


SFIXED get_var(m)
struct macro **m;
{
char key[256];
int i;
char ch;
struct symbol *sym;
struct symbol *search_table();

ch = M_GETCH(*m);
i = 0;
while (isalnum(ch)) {
   key[i++] = ch;
   M_NEXT(*m);
   ch = M_GETCH(*m);
}
key[i] = '\0';

sym = search_table(&res_table, key);
if (sym != NULL)
   return (SFIXED)(sym->value);
else {
   sym = search_table(&var_table, key);
   if (sym != NULL)
      return (SFIXED)(sym->value);
   else
      return 0;                        /*!!!!*/
}
}





/* return((COORD) ((val+(1<<15)) >> 16));*/
  /** added one to fix rounding error **/
                                 /** AW 2-24-87 **/
