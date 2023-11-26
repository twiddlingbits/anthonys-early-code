

/* opena2024.c -- development test of opening a2024 screen   */

#include <exec/types.h>
#include <intuition/intuition.h>

#define NONEWINCLUDES      1   /* don't have 1.3 includes yet   */


#if NONEWINCLUDES      /* Some additional definitions   */

/********** from intuition/screens.h ************/

#define NS_EXTENDED     0x1000    /* NewScreen.Extension is valid      */

struct TagItem
{
    ULONG   ti_Tag;      /* identifies the type of this item   */
    ULONG   ti_Data;   /* type-specific data, can be a pointer   */
};

/* ----   system tag values -----------------------------   */
#define TAG_DONE   (0L)   /* terminates array of TagItems. ti_Data unused   */
#define   TAG_IGNORE (1L)   /* ignore this item, not end of array      */
#define   TAG_MORE   (2L)   /* ti_Data is pointer to another array of TagItems
          * note that this tag terminates the current array
          */

/* ----   user tag identification -----------------------   */
#define TAG_USER  (1L<<31)    /* differentiates user tags from system tags*/

/* until further notice, tag bits 16-30 are RESERVED and should be zero.
 * Also, the value (TAG_USER | 0) should never be used as a tag value.
 */


/* ----   tag values special to NewScreen.Extension -----   */
#define NSTAG_EXT_VPMODE (TAG_USER | 1)
/* ti_Data specifies data for ViewPort.ExtendedMode (ZZZ name?)   */

#define NewScreen tempNewScreen

/* this is the extended NewScreen from the new screens.h   */
struct tempNewScreen
{
    SHORT LeftEdge, TopEdge, Width, Height, Depth;  /* screen dimensions */

    UBYTE DetailPen, BlockPen;   /* for bar/border/gadget rendering   */

    USHORT ViewModes;      /* the Modes for the ViewPort (and View) */

    USHORT Type;      /* the Screen type (see defines above)   */
    
    struct TextAttr *Font;   /* this Screen's default text attributes */
    
    UBYTE *DefaultTitle;   /* the default title for this Screen   */
    
    struct Gadget *Gadgets;   /* your own Gadgets for this Screen   */
    
    /* if you are opening a CUSTOMSCREEN and already have a BitMap 
     * that you want used for your Screen, you set the flags CUSTOMBITMAP in
     * the Type field and you set this variable to point to your BitMap
     * structure.  The structure will be copied into your Screen structure,
     * after which you may discard your own BitMap if you want
     */
    struct BitMap *CustomBitMap;

    struct TagItem   *Extension;
            /* more specification data, scanned if
             * NS_EXTENDED is set in NewScreen.Type
             */
};


/********** from graphics/view.h ****************/
#define VPF_A2024      0x40
#define VPF_AGNUS      0x20
#define VPF_TENHZ      0x20


#endif   NONEWINCLUDES



struct TextAttr SafeFont =
    {
    "topaz.font",
    8,
    0,
    0,
    };

 
/* This is the smallest case of the proposed data extension 
 * For now, use exactly this format, and the final scheme will
 * be compatible with this.
 * These structures and definitions are in intuition/screens.h
 * for now, but may be moved.
 *
 * ViewPort extended modes flag VPF_A2024 defined in
 * graphics/viewport.h. 
 */
struct TagItem   nsextension[] = {
    { NSTAG_EXT_VPMODE, VPF_A2024},
    { TAG_DONE, 0}
};


struct Screen *
OpenA2024Screen()
{
    struct NewScreen   ns;

    ns.LeftEdge      =   0;
    ns.TopEdge      =   0;
    ns.Width      =   1008;
    ns.Height      =   800;
    ns.Depth      =   2;   /* can use 1 or 2   */
    ns.DetailPen    =   0;
    ns.BlockPen      =   1;
    ns.ViewModes    =   HIRES;   /* determined above   */

    /* Note that NS_EXTENDED is set to indicate data in ns.Extension   */
    ns.Type      =   CUSTOMSCREEN | NS_EXTENDED;

    ns.Font      =   &SafeFont;
    ns.DefaultTitle = "City Desk 2.0 © 1987 & 1988 MicroSearch.  Written by SunRize Industries";
    ns.Gadgets      =   NULL;
    ns.CustomBitMap   =   NULL;

    /* supply extended specification here   */
    ns.Extension    =   nsextension;
    return ((struct Screen *) OpenScreen(&ns));
}

