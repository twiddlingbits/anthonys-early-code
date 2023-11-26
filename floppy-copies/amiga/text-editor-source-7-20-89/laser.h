/*******************************************************/
/** laser.h  laser printer font structure definitions **/
/*******************************************************/

#ifndef LASER_H
#define LASER_H

#ifndef GRAPHICS_TEXT_H
#include <graphics/text.h>
#endif

/** flags used to define a type style **/

#define ITALIC    0x0001
#define BOLD      0x0002
#define UNDERLINE 0x0004
#define COMPRESS  0x0008


/** font flags (place var.) **/

#define RESIDENT 1
#define PSFONT 2
#define HPFONT 4

/** the screen font structure **/

#define FONTNAMESIZE 30

struct screenfontnode {
   struct screenfontnode *next;
   struct TextFont *font;
   COORD size;
   struct TextAttr ta;
   char name[FONTNAMESIZE];
   UBYTE style;
};

/** the laser font structure **/

struct laserfont {
struct laserfont *next;
struct text_kern *kern;  /** Pointer to kern structure **/
char fontname[30];       /** The name for this font **/
char fonton[30];         /** The ESC sequence user to turn this font on **/
COORD widthtable[256];   /** width of each character in laser dots **/
UBYTE style;             /** Style for this font. Bold, Italic, etc **/
BYTE place;              /** Where is this font? RESIDENT CART_A, etc **/
COORD height;            /** font height in COORDs above baseline **/
COORD depth;             /** font depth below base line **/
COORD size;              /** Size USER thinks this font is  **/
struct TextFont *screenfont;  /** font to print to screen with **/
};

/** flags for below -- printertype **/

#define HPLASER 0
#define PREFERENCES 1
#define POSTSCRIPT 2

/** flags **/

#define USE_PAR   1        /** LaserJet is on parallel port **/
#define USE_SER   2        /** laserJet is on serial port **/

/** Printer Preferences structure **/

struct printerprefs {
WORD printertype;       /** laser or preferences? **/
WORD dotsperinch;       /** for laser graphics. 75, 150, 300 **/
struct printoptions opts;   /** defined in desk.h for some wierd reason */
WORD flags;
};

/* Kern structure */

struct text_kern {
   UWORD *pairs;     /* array of (first,second) character pairs */
   COORD *value;     /* parallel array of kern values (<0 = move left) */
   int num;          /* number of kern pairs */
};

#endif
