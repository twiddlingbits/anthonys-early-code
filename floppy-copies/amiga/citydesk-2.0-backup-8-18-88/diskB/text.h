/* === text.h ===                          */
/* Definitions for text related structures */

#ifndef LASER_H
#include "laser.h"
#endif

#define HYPHENCHAR '-'

#define CHAR_HSKIP      1
#define CHAR_VSKIP      2
#define CHAR_HPOS       3
#define CHAR_TAB        4
#define CHAR_SOFTHYPHEN 5
#define CHAR_TIE        6
#define CHAR_MAX        6

#define NUM_DEF_INCH    0
#define NUM_DEF_POINT   1
#define NUM_DEF_NONE    2

#define NUM_TABS  10

/*** text_state flags ***/

#define TEXTS_STARTPAR     0x0001
/*                 Justification:    left  right center fill   */
#define TEXTS_RAGLEFT      0x0002  /*  0     1     1     0     */
#define TEXTS_RAGRIGHT     0x0004  /*  1     0     1     0     */
#define TEXTS_USELEADING   0x0008
#define TEXTS_INESC        0x0010
#define TEXTS_VCENTER      0x0020
#define TEXTS_SAVERAGL     0x0040   /* save justification mode for   */
#define TEXTS_SAVERAGR     0x0080   /* cj and nocj.                  */
#define TEXTS_NOBREAK      0x0100
#define TEXTS_NOWIDOW      0x0200
#define TEXTS_FORCECOLEND  0x0400
#define TEXTS_FORCEPAGEEND 0x0800


/* Style flags are in laser.h */

/*** text_line flags ***/
#define TEXTL_PARSTART     1
#define TEXTL_PAREND       2
#define TEXTL_HYPHEN       4

/*** text_object flags ***/

#define FIRSTOBJECT  0x01 /* first text_object marker. Also start of text */
#define NEEDSFLOWING 0x02 /* Set on last text_object when more text left  */
#define VERTCENTER   0x04 /* Set if this object is to be vert. centered   */
#define STRIPNL      0x08 /* Set if we strip leading newlines             */
#define CHECKWIDOW   0x10 /* Set if we check for widow and orphan lines   */

#ifndef CB
typedef ULONG FIXED;    /* Fixed point number (16 bit int, 16 bit fract) */
#endif

#if 0             /* should be obsolete */
struct text_tab {
   SCOORD rel_hskip;
   COORD abs_hskip;
};
#endif

/* Note: some items take effect immediately (I); others take effect at */
/*       the beginning of the next line (L)                            */

struct text_state {
   struct TextFont *font;              /*  I  */
   struct laserfont *lfont;            /*  I  */
   COORD font_size;                    /*  I  */
   UWORD style;                        /*  I  */
   FIXED magx, magy;                   /*  I  */
   COORD sentence_space;               /*  I  */
   COORD rel_vskip;                    /*  I  */
   COORD tabs[NUM_TABS];               /*  I  */
   COORD max_space_stretch;            /*  L  */
   COORD leading, spacing;             /*  L  */
   COORD paragraph_indent;             /*  L  */
   COORD paragraph_space;              /*  L  */   /* adds to leading */
   COORD left_margin, right_margin;    /*  L  */
   COORD top_margin, bottom_margin;    /*  L  */
   UWORD flags;                        /*  L  */
   char  escstart,escend;              /*  I  */   /* defaults are in */
   char  softhyphen,tabchar;           /*  I  */   /* ... main.c      */
   char  tiechar;                      /*  I  */
};

struct text_object {
   struct object *next;
   struct text_line *line;
   char *textstart;     /* obsolete */
   char *textend;       /* obsolete */
   COORD height, width;
   struct text_state state, endstate;
   UBYTE flags;
   struct macro *macro;
   struct macro *endmacro;    /* for use by the next obj in text chain */
};

struct text_line {
   struct text_line *next;
   int   startcnt;         /* # of chars from beg of obj to printing text */
   int pad;             /* remove later!!! */
   COORD w, maxh, maxd;
   SHORT num_chars, stretch_chars;
   UBYTE num_spaces;
   UBYTE flags;
};

struct text_ctrl {
   struct macro *m;
   int   count;
   COORD x, y;
   COORD newx, newy;
   COORD startx, maxw;
   COORD itkern;
   struct text_state state;
   struct text_line line;
   char  ch, prevch;
   struct macro *unget_macro;
   struct text_state unget_state;
   COORD unget_x, unget_maxw;
   char unget_char, unget_prevch, unget_active;
};

#define INRANGE(p,m)    ((p) < ((m)->macrotext->end))
#define M_GETCH(m) ((INRANGE(((m)->current),m)) ? (*((m)->current)) : '\0')
#define M_NEXT(m) ((m)->current++)

#define MTEXT_FREETEXT     0x0001
#define MTEXT_FREEMTEXT    0x0002
#define MTEXT_PARAMETER    0x0004

#define MACRO_INCALL       0x0001

struct macro_text {
   char *start;
   char *end;           /* points to first INVALID char (past the end) */
   WORD ref_count;
   UWORD flags;
};

struct parameter {
   struct macro_text mtext;
   struct parameter *next;
   struct parameter *context;    /* for params expanded inside a param */
};

struct macro {
   struct macro *parent;
   struct macro_text *macrotext;
   char *current;
   struct parameter *param;
   UWORD flags;
};


struct symbol {
   char *key;
   ULONG value;
};

struct symtab {
   struct symbol *entries;
   int n;
   int tbl_size;
};

#define SYMTAB_DEFSIZE  100

