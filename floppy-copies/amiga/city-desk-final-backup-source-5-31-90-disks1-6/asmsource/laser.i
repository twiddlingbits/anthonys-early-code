*/*******************************************************/
*/** laser.i  laser printer font structure definitions **/
*/*******************************************************/
*
*struct laserfont {
*struct laserfont *next*
*struct text_kern *kern*  /** Pointer to kern structure **/
*char fontname[30]*       /** The name for this font **/
*char fonton[30]*         /** The ESC sequence user to turn this font on **/
*COORD widthtable[256]*   /** width of each character in laser dots **/
*UBYTE style*             /** Style for this font. Bold, Italic, etc **/
*BYTE place*              /** Where is this font? RESIDENT CART_A, etc **/
*COORD height*            /** font height in COORDs above baseline **/
*COORD depth*             /** font depth below base line **/
*COORD size*              /** Size USER thinks this font is  **/
*struct TextFont *screenfont*  /** font to print to screen with **/
*}*
*
*
lf_next  equ   0
lf_kern  equ   4
* ...
*
*
*/* Kern structure */
*
*struct text_kern {
*   UWORD *pairs*     /* array of (first,second) character pairs */
*   COORD *value*     /* parallel array of kern values (<0 = move left) */
*   int num*          /* number of kern pairs */
*}*
*
*
kern_pairs  equ   0
kern_value  equ   4
kern_num    equ   8

