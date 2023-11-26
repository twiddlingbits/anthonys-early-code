/*###################################################################*/
/* resvar.c                                                          */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include "desk.h"
#include "text.h"

#define RES_AUTOTHRESH     1
#define RES_BM             2
#define RES_FSIZE          3
#define RES_INDENT         4
#define RES_IRRMARGIN      5
#define RES_IRRMINWIDTH    6
#define RES_LEADING        7
#define RES_LM             8
#define RES_MAGX           9
#define RES_MAGY           10
#define RES_PAGE           11
#define RES_PARSPACE       12
#define RES_RM             13
#define RES_SENTSPACE      14
#define RES_SPACESTRETCH   15
#define RES_SPACING        16
#define RES_TM             17
#define RES_X0             18
#define RES_X1             19
#define RES_X2             20
#define RES_X3             21
#define RES_X4             22
#define RES_X5             23
#define RES_X6             24
#define RES_X7             25
#define RES_X8             26
#define RES_X9             27

struct symbol resvar_table[] = {
   {"autothresh",       RES_AUTOTHRESH},
   {"bm",               RES_BM},
   {"fsize",            RES_FSIZE},
   {"indent",           RES_INDENT},
   {"irrmargin",        RES_IRRMARGIN},
   {"irrminwidth",      RES_IRRMINWIDTH},
   {"leading",          RES_LEADING},
   {"lm",               RES_LM},
   {"magx",             RES_MAGX},
   {"magy",             RES_MAGY},
   {"page",             RES_PAGE},
   {"parspace",         RES_PARSPACE},
   {"rm",               RES_RM},
   {"sentspace",        RES_SENTSPACE},
   {"spacestretch",     RES_SPACESTRETCH},
   {"spacing",          RES_SPACING},
   {"tm",               RES_TM},
   {"x0",               RES_X0},
   {"x1",               RES_X1},
   {"x2",               RES_X2},
   {"x3",               RES_X3},
   {"x4",               RES_X4},
   {"x5",               RES_X5},
   {"x6",               RES_X6},
   {"x7",               RES_X7},
   {"x8",               RES_X8},
   {"x9",               RES_X9}
};


struct symtab res_table = {resvar_table,
                           sizeof(resvar_table)/sizeof(struct symbol),
                           sizeof(resvar_table)/sizeof(struct symbol),
                           0
                          };


extern SHORT texterrors;
extern struct Window *clipboard;

extern UWORD xinch, yinch;

/* convert COORD to SFIXED */
#define CTOSF(x) ((SFIXED)(((SFIXED)(x)) << 16))
#define CTOSFX(x) ((SFIXED)(((((SFIXED)(x))<<6) * 576 / xinch) << 10))
#define CTOSFY(x) ((SFIXED)(((((SFIXED)(x))<<6) * 576 / yinch) << 10))

/* convert SFIXED to COORD */
#define SFTOCX(x) ((COORD)((((x)>>9) * xinch / 576) >> 7))
#define SFTOCY(x) ((COORD)((((x)>>9) * yinch / 576) >> 7))

/************************************************************************/
/* get_res_value(tc, sym);                                              */
/*                                                                      */
/* Gets a value for a reserved variable and returns it as the function  */
/* return.                                                              */
/************************************************************************/

SFIXED get_res_value(tc, sym)
struct text_ctrl *tc;
struct symbol *sym;
{
struct text_state *state;
char text1[80];

state = &tc->state;

switch (sym->value) {

case RES_AUTOTHRESH:
   return CTOSFX(state->autohyph_threshhold);

case RES_BM:
   return CTOSFY(state->bottom_margin);

case RES_FSIZE:
   return CTOSF(state->font_size);

case RES_INDENT:
   return CTOSFX(state->paragraph_indent);

case RES_IRRMARGIN:
   return CTOSFX(state->irreg_margin);

case RES_IRRMINWIDTH:
   return CTOSFX(state->irreg_minwidth);

case RES_LEADING:
   if (state->flags & TEXTS_USELEADING)
      return CTOSFY(state->leading);
   else
      return 0;

case RES_LM:
   return CTOSFX(state->left_margin);

case RES_MAGX:
   return (SFIXED)state->magx;

case RES_MAGY:
   return (SFIXED)state->magy;

case RES_PAGE:
   if (state->objptr != NULL)
      return ((SFIXED)state->objptr->pn) << 16;
   else
      return 0;

case RES_PARSPACE:
   return CTOSFY(state->paragraph_space);

case RES_RM:
   return CTOSFX(state->right_margin);

case RES_SENTSPACE:
   return CTOSFX(state->sentence_space);

case RES_SPACESTRETCH:
   return CTOSFX(state->max_space_stretch);

case RES_SPACING:
   if (!(state->flags & TEXTS_USELEADING))
      return CTOSFY(state->spacing);
   else
      return 0;

case RES_TM:
   return CTOSFY(state->top_margin);

case RES_X0:
   return state->variables[0];

case RES_X1:
   return state->variables[1];

case RES_X2:
   return state->variables[2];

case RES_X3:
   return state->variables[3];

case RES_X4:
   return state->variables[4];

case RES_X5:
   return state->variables[5];

case RES_X6:
   return state->variables[6];

case RES_X7:
   return state->variables[7];

case RES_X8:
   return state->variables[8];

case RES_X9:
   return state->variables[9];

default:
   sprintf(text1, "Invalid resvar, code = %x", sym->value);
   TEXTERROR("Internal program error!", text1);
   return 0;
}
}


/************************************************************************/
/* set_res_value(tc, sym, value);                                       */
/*                                                                      */
/* Sets a reserved variable to the given value.                         */
/************************************************************************/

VOID set_res_value(tc, sym, value)
struct text_ctrl *tc;
struct symbol *sym;
SFIXED value;
{
struct text_state *state;
char text1[80];

state = &tc->state;

switch (sym->value) {

case RES_AUTOTHRESH:
   state->autohyph_threshhold = SFTOCX(value);
   break;

case RES_BM:
   state->bottom_margin = SFTOCY(value);
   break;

case RES_FSIZE:
   TEXTERROR("FSIZE is a read-only", "reserved variable");
   break;

case RES_INDENT:
   state->paragraph_indent = SFTOCX(value);
   break;

case RES_IRRMARGIN:
   state->irreg_margin = SFTOCX(value);
   break;

case RES_IRRMINWIDTH:
   state->irreg_minwidth = SFTOCX(value);
   break;

case RES_LEADING:
   state->leading = SFTOCY(value);
   state->flags |= TEXTS_USELEADING;
   break;

case RES_LM:
   state->left_margin = SFTOCX(value);
   break;

case RES_MAGX:
   state->magx = (FIXED)value;
   break;

case RES_MAGY:
   state->magy = (FIXED)value;
   break;

case RES_PAGE:
   TEXTERROR("PAGE is a read-only", "reserved variable");
   break;

case RES_PARSPACE:
   state->paragraph_space = SFTOCY(value);
   break;

case RES_RM:
   state->right_margin = SFTOCX(value);
   break;

case RES_SENTSPACE:
   state->sentence_space = SFTOCX(value);
   break;

case RES_SPACESTRETCH:
   state->max_space_stretch = SFTOCX(value);
   break;

case RES_SPACING:
   state->spacing = SFTOCY(value);
   state->flags &= ~TEXTS_USELEADING;
   break;

case RES_TM:
   state->top_margin = SFTOCY(value);
   break;

case RES_X0:
   state->variables[0] = value;
   break;

case RES_X1:
   state->variables[1] = value;
   break;

case RES_X2:
   state->variables[2] = value;
   break;

case RES_X3:
   state->variables[3] = value;
   break;

case RES_X4:
   state->variables[4] = value;
   break;

case RES_X5:
   state->variables[5] = value;
   break;

case RES_X6:
   state->variables[6] = value;
   break;

case RES_X7:
   state->variables[7] = value;
   break;

case RES_X8:
   state->variables[8] = value;
   break;

case RES_X9:
   state->variables[9] = value;
   break;

default:
   sprintf(text1, "Invalid resvar, code = %x", sym->value);
   TEXTERROR("Internal program error!", text1);
   break;
}
}

