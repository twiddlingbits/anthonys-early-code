*/* === text.i ===                          */
*/* Definitions for text related structures */
*
*
*struct text_state {
*   struct TextFont *font;              /*  I  */
*   struct laserfont *lfont;            /*  I  */
*   COORD font_size;                    /*  I  */
*   UWORD style;                        /*  I  */
*   FIXED magx, magy;                   /*  I  */
*   COORD sentence_space                /*  I  */
*   COORD rel_vskip;                    /*  I  */
*   COORD tabs[NUM_TABS];               /*  I  */
*   COORD max_space_stretch;            /*  L  */
*   COORD leading, spacing;             /*  L  */
*   COORD paragraph_indent;             /*  L  */
*   COORD paragraph_space;              /*  L  */   /* adds to leading */
*   COORD left_margin, right_margin;    /*  L  */
*   COORD top_margin, bottom_margin;    /*  L  */
*   UWORD flags;                        /*  L  */
*   char  escstart,escend;              /*  I  */   /* defaults are in */
*   char  softhyphen,tabchar;           /*  I  */   /* ... main.c      */
*   char  tiechar;                      /*  I  */
*};
*
*
st_font        equ   0
st_lfont       equ   4
st_font_size   equ   8
st_style       equ   10
st_magx        equ   12
st_magy        equ   16
st_sent_space  equ   20

