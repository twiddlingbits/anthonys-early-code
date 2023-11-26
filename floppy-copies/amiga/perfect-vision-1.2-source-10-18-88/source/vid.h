
#define BUFSIZE 64000
#define MENUHEIGHT 12
#define MENUTOP 9
#define MAXFRAMES 200

#define NORMAL 0
#define QUARTER 1

#define BUSY     2
#define ZERO     3

/** Modes that can be used to display data **/

#define ASM 0
#define BLIT 1
#define NICEBLIT 2

/** Define menu screen's colors **/

#define WHITE 2

/* Error Codes */

#define AOK            0
#define OUTOFMEM       1
#define DISKERROR      2
#define CANTOPENFILE   3
#define WRONGFILETYPE  4
#define BADIFF         5
#define NOFRAMES       6
#define ONLY_ONE_FRAME 7

/** some macros **/

#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))


/** cs_mode flags in ham_menu.c **/

#define RED 0
#define GREEN 1
#define BLUE 2

