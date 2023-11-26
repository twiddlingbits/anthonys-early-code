#ifndef GRAPHICS_SPRITE_H
#define GRAPHICS_SPRITE_H
/********************************************************************/
/*               Commodore-Amiga, Inc.                              */
/*               sprite.h                                           */
/********************************************************************/

#define SPRITE_ATTACHED 0x80

struct SimpleSprite
{
    UWORD *posctldata;
    UWORD height;
    UWORD   x,y;    /* current position */
    UWORD   num;
};
#endif
