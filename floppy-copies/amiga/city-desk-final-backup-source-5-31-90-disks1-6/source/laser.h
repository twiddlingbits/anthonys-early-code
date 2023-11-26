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
#define EXTENDED_FONTON 8

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
#define VPAGE 3      /** C'ltd virtual page **/

/** flags **/

#define USE_PAR   1        /** LaserJet is on parallel port **/
#define USE_SER   2        /** laserJet is on serial port **/
#define USE_FILE  4        /** print to a file **/
#define PROOF     8        /** Do not print graphics **/
#define DITHER_HALFTONE  16   /** use halftone dithering **/
#define DITHER_LIGHT     32   /** causes dither to use 0-7 blk lvls **/
#define STRIP_PRINT      64   /** print in several passes to save mem **/


/** Printer Preferences structure **/

struct printerprefs {
   SHORT printertype;      /** HP, Post, or preferences? **/
   SHORT dotsperinch;      /** Default dpi for HP LaserJet Graphics **/
   SHORT offset;           /** added to page number before printing **/
   SHORT flags;
   SHORT copies;           /** Number of copies to print of each page **/
   };

/* Kern structure */

struct text_kern {
   UWORD *pairs;     /* array of (first,second) character pairs */
   COORD *value;     /* parallel array of kern values (<0 = move left) */
   int num;          /* number of kern pairs */
};

#endif
