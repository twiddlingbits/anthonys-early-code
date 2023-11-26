#ifndef GRAPHICS_GFX_H
#define GRAPHICS_GFX_H
/****** gfx.h **************************************************/
/*                                                             */
/*               Commodore-Amiga, Inc.                         */
/*                                                             */
/*      Modification History                                   */
/*  date    :author  :Comments                                 */
/*-------------------------------------------------------------*/
/*  8-24-84 Dale     added this header file                    */
/*  Feb 85  Dale     added Rectangle,BitMap structures         */
/***************************************************************/

/* general include file for application programs */
#define BITSET  0x8000
#define BITCLR  0

#define AGNUS
#ifdef AGNUS
#define TOBB(a)      ((long)(a))
#else
#define TOBB(a)      ((long)(a)>>1)  /* convert Chip adr to Bread Board Adr */
#endif

struct Rectangle
{
    SHORT   MinX,MinY;
    SHORT   MaxX,MaxY;
};

typedef struct tPoint
{
    WORD x,y;
} Point;

typedef UBYTE *PLANEPTR;

struct BitMap
{
    UWORD   BytesPerRow;
    UWORD   Rows;
    UBYTE   Flags;
    UBYTE   Depth;
    UWORD   pad;
    PLANEPTR Planes[8];
};

#define RASSIZE(w,h)    ((h)*( (w+15)>>3&0xFFFE))

#endif
