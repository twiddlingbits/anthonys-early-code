/** WARNING: you must compile this module with -ad and no -b option **/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"

/*****************************/
/*** change pointer **********/
/*****************************/

busy_ptr(w)

struct Window *w;

{
static unsigned short busy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x380, 0x380,
   0x1FE0, 0x1FE0,
   0x3FFC, 0x3FFC,
   0x3FFE, 0x31FE,
   0x3FFE, 0x3DFE,
   0x7FFE, 0x7B1E,
   0x7FFE, 0x77DE,
   0x7FFC, 0x71BC,
   0x7FFC, 0x7F7C,
   0x3FFC, 0x3F1C,
   0x7FF8, 0x7FF8,
   0x7F00, 0x7F00,
   0x3BC0, 0x3BC0,
   0x1C0, 0x1C0,
   0x80, 0x80,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -1 */

SetPointer(w,busy,16,16,-2,-2);

/* setall(screen,busy, 16, 16, -2, -1); */
}

normal_ptr(w)

struct Window *w;

{
ClearPointer(w);
}
