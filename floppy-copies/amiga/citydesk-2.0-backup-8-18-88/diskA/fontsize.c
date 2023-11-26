/*###################################################################*/
/* fontsize.c                                                        */
/*###################################################################*/
     
#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "text.h"
     
#define MAX(a,b)  ((a)>(b)?(a):(b))
     
extern SHORT texterrors;
extern UBYTE *translate;         /** points to translation table for **/
                                 /** non-ascii characters. ($80-$FF) **/
extern struct Window *clipboard;

     
/************************************************************************/
/* w = char_width(ch, state);                                           */
/*                                                                      */
/* Returns the width of a character in state->font or state->lfont      */
/************************************************************************/
     
SCOORD char_width(ch, state)
UBYTE ch;
struct text_state *state;
{
int cs;
UBYTE charoffset;
COORD tpmagtocx();
     
if (ch == CHAR_HSKIP)
   return(0);
     
if (ch == CHAR_VSKIP)
   return(0);

if (ch == '\n' || ch == CHAR_SOFTHYPHEN)
   return(0);
     
if (ch == CHAR_TIE)
   ch = ' ';            /* use width of a space */
     
if (state->lfont)
   return((SCOORD)state->lfont->widthtable[translate[ch]]);
     
if (ch < state->font->tf_LoChar)
   charoffset = state->font->tf_HiChar - state->font->tf_LoChar + 1;
else
   charoffset = ch - state->font->tf_LoChar;
     
if (state->font->tf_CharSpace) {
   cs = *(((WORD *)state->font->tf_CharSpace) + charoffset);
   if (state->font->tf_CharKern)
      cs += *(((WORD *)state->font->tf_CharKern) + charoffset);
}
else
   cs = state->font->tf_XSize;
     
/* If bold and font is not already bold, add in bold adjust */
     
if (state->style & BOLD && !(state->font->tf_Style & FSF_BOLD))
   cs += state->font->tf_BoldSmear;
     
/* If italic and font is not already italic, add in italic adjust */
     
if (state->style & ITALIC && !(state->font->tf_Style & FSF_BOLD))
   cs += state->font->tf_YSize >> 1;
     
return(tpmagtocx(cs, state->magx));
}
     
     
/************************************************************************/
/* w = char_base_width(ch, state, cbw);                                 */
/*                                                                      */
/* Returns the width of a character in font in state without regard to  */
/* bold or italic or interchar spacing (the base width).  For Amiga     */
/* fonts, this is the bit width of the char scaled appropriately.       */
/************************************************************************/
     
SCOORD char_base_width(ch, state, cbw)
UBYTE ch;
struct text_state *state;
WORD cbw;
{
COORD tpmagtocx();
     
if (ch == CHAR_HSKIP)
   return(0);
     
if (ch == CHAR_VSKIP)
   return(0);

if (ch == '\n' || ch == CHAR_SOFTHYPHEN)
   return(0);
  
if (ch == CHAR_TIE) {
   ch = 32;          /** use width of a space **/
   }
     
if (state->lfont)
   return((SCOORD)(MAX(1,state->lfont->widthtable[translate[ch]] - 8)));
     
return(tpmagtocx(cbw, state->magx));
}
     
     
/************************************************************************/
/* italic_adjust (ch, &state, &w, &itkern);                             */
/*                                                                      */
/* If we are currently in italics, adjust current x position (*w) by    */
/* *itkern (prev char overhang) and the amount of this character below  */
/* the baseline that has been shifted.                                  */
/************************************************************************/
     
italic_adjust (ch, state, w, itkern)
UBYTE ch;
struct text_state *state;
COORD *w, *itkern;
{
COORD k;
     
if (ch == CHAR_HSKIP || ch == CHAR_VSKIP || ch == CHAR_SOFTHYPHEN)
   return(0);
     
if (state->lfont) {
   *itkern = 0;
}
else {
   if (state->style & ITALIC) {
      k = *itkern + tpmagtocx((state->font->tf_YSize -
                               state->font->tf_Baseline) >> 1, state->magx);
      if (*w > k)
         *w -= k;
      else
         *w = 0;
      *itkern = tpmagtocx((state->font->tf_Baseline) >> 1, state->magx);
   }
}
}
     
/************************************************************************/
/* h = char_height(ch, state);                                          */
/*                                                                      */
/* Returns the height above the main baseline for character ch in       */
/* state->font.  This is usually a characteristic of the font, so the   */
/* actual character ch is irrelevant.  The vertical displacement is     */
/* taken into account.                                                  */
/************************************************************************/
     
COORD char_height(ch, state)
UBYTE ch;
struct text_state *state;
{
COORD tpmagtocy(), ht;
     
if (ch == CHAR_HSKIP || ch == CHAR_VSKIP)
   return(0);
     
if (state->lfont)
   return((COORD)MAX(0,state->lfont->height - state->rel_vskip));
else {
   ht = tpmagtocy(state->font->tf_Baseline+1, state->magy);
   return((COORD)MAX(0, ht - state->rel_vskip));
}
}
     
     
/************************************************************************/
/* d = char_depth(ch, state);                                           */
/*                                                                      */
/* Returns the maximum depth below the main baseline that a character   */
/* in state->font may descend.  This is usually a characteristic of     */
/* the font, so the actual character ch is irrelevant.  The vertical    */
/* displacement in tab is taken into account.                           */
/************************************************************************/
     
COORD char_depth(ch, state)
UBYTE ch;
struct text_state *state;
{
COORD tpmagtocy(), dep;
     
if (ch == CHAR_HSKIP || ch == CHAR_VSKIP)
   return(0);
     
if (state->lfont)
   return((COORD)MAX(0, state->lfont->depth + state->rel_vskip));
else {
   dep = tpmagtocy(state->font->tf_YSize - state->font->tf_Baseline - 1,
                   state->magy);
   return((COORD)MAX(0, dep + state->rel_vskip));
}
}
     
     
/************************************************************************/
/* h = char_real_height(state);                                         */
/*                                                                      */
/* Returns the height above the character baseline for this font.  The  */
/* vertical displacement is NOT taken into account.  This is relative   */
/* to the character only, and won't change with vskip's (unlike         */
/* char_height).                                                        */
/************************************************************************/
     
COORD char_real_height(state)
struct text_state *state;
{
COORD tpmagtocy();
     
if (state->lfont)
   return(state->lfont->height);
else
   return(tpmagtocy(state->font->tf_Baseline+1, state->magy));
}
     
/************************************************************************/
/* h = char_ysize(state);                                               */
/*                                                                      */
/* Returns the y size of a character in font state->font.  This is the  */
/* actual size of the character, without taking into account vskip's.   */
/************************************************************************/
     
COORD char_ysize(state)
struct text_state *state;
{
COORD tpmagtocy();
     
if (state->lfont)
   return((COORD)(state->lfont->height + state->lfont->depth));
else
   return(tpmagtocy(state->font->tf_YSize, state->magy));
}
     
/************************************************************************/
/* h = char_ybitsize(state);                                            */
/*                                                                      */
/* Returns the y size of a character in font state->font in actual      */
/* bits, ignoring coordinates and all that stuff.                       */
/************************************************************************/
     
COORD char_ybitsize(state)
struct text_state *state;
{
return((COORD)state->font->tf_YSize);
}
     
