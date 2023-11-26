#include "exec/types.h"

SHORT sav1,sav2,sav3,sav4,sav5,sav6,sav7;

saveloadstat()

{
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

sav1=strip_spaces;
sav2=double_newlines;
sav3=dot_commands;
sav4=scribble_style;
sav5=ansi_escapes;
sav6=real_ascii;
sav7=wp_style;
}

loadloadstat()

{
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

strip_spaces=sav1;
double_newlines=sav2;
dot_commands=sav3;
scribble_style=sav4;
ansi_escapes=sav5;
real_ascii=sav6;
wp_style=sav7;
}

/** sets load paramaters for a NotePad load **/


setansi()

{

extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

double_newlines=FALSE;
dot_commands=FALSE;
scribble_style=FALSE;
ansi_escapes=TRUE;
real_ascii=FALSE;
wp_style=FALSE;
}

/** sets load paramaters for a ascii1 load **/


setascii1()

{

extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

double_newlines=FALSE;
dot_commands=FALSE;
scribble_style=FALSE;
ansi_escapes=FALSE;
real_ascii=TRUE;
wp_style=FALSE;
}

/** sets load paramaters for a ascii2 load **/


setascii2()

{

extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

double_newlines=TRUE;
dot_commands=FALSE;
scribble_style=FALSE;
ansi_escapes=FALSE;
real_ascii=TRUE;
wp_style=FALSE;
}

/** sets load paramaters for a scribble! load **/


setscribble()

{

extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

double_newlines=FALSE;
dot_commands=TRUE;
scribble_style=TRUE;
ansi_escapes=FALSE;
real_ascii=FALSE;
wp_style=FALSE;
}

/** set load paramaters for a wordperfect load **/

setwp()

{

extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

double_newlines=FALSE;
dot_commands=FALSE;
scribble_style=FALSE;
ansi_escapes=FALSE;
real_ascii=FALSE;
wp_style=TRUE;
}


