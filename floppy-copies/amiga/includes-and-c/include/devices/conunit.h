#ifndef DEVICES_CONUNIT_H
#define DEVICES_CONUNIT_H
#ifndef  EXEC_PORTS_H
#include "exec/ports.h"
#endif
#ifndef  DEVICES_CONSOLE_H
#include "devices/console.h"
#endif
#ifndef  DEVICES_KEYMAP_H
#include "devices/keymap.h"
#endif
#ifndef  DEVICES_INPUTEVENT_H
#include "devices/inputevent.h"
#endif
#define  PMB_ASM     (M_LNM+1)
#define  PMB_AWM     (PMB_ASM+1)
#define  MAXTABS     80
struct   ConUnit {
   struct   MsgPort cu_MP;
   struct   Window *cu_Window;
   WORD  cu_XCP;
   WORD  cu_YCP;
   WORD  cu_XMax;
   WORD  cu_YMax;
   WORD  cu_XRSize;
   WORD  cu_YRSize;
   WORD  cu_XROrigin;
   WORD  cu_YROrigin;
   WORD  cu_XRExtant;
   WORD  cu_YRExtant;
   WORD  cu_XMinShrink;
   WORD  cu_YMinShrink;
   WORD  cu_XCCP;
   WORD  cu_YCCP;
   struct   KeyMap cu_KeyMapStruct;
   UWORD cu_TabStops[MAXTABS];
   BYTE     cu_Mask;
   BYTE     cu_FgPen;
   BYTE     cu_BgPen;
   BYTE     cu_AOLPen;
   BYTE     cu_DrawMode;
   BYTE     cu_AreaPtSz;
   APTR     cu_AreaPtrn;
   UBYTE    cu_Minterms[8];
   struct   TextFont *cu_Font;
   UBYTE    cu_AlgoStyle;
   UBYTE    cu_TxFlags;
   UWORD    cu_TxHeight;
   UWORD    cu_TxWidth;
   UWORD    cu_TxBaseline;
   UWORD    cu_TxSpacing;
   UBYTE    cu_Modes[(PMB_AWM+7)/8];
   UBYTE    cu_RawEvents[(IECLASS_MAX+7)/8];
};
#endif
