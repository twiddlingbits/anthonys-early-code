#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "psound.h"

/***************************************************/
/*** add gadgets new to version 2.1 to the window **/
/***************************************************/

USHORT *newdata;



USHORT newgadsData[] =
{

0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x001F,0xFC00,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0002,0x0000,0x01E0,0x03C0,0x0000,
0x0000,0x0000,0x0000,0x0007,0x0204,0x0000,
0x0600,0x0030,0x0000,0x0000,0x0000,0x0000,
0x000F,0x0408,0x0000,0x0800,0x0008,0x0000,
0x0000,0x0000,0x0000,0x001F,0x0810,0x0000,
0x3000,0x0006,0x0000,0x2000,0x0000,0x0000,
0x07FF,0x1020,0x0000,0x279F,0x3CF2,0x0000,
0x5081,0x0180,0x0000,0x07FF,0x1020,0x0000,
0x4404,0x2491,0x0000,0x90C7,0xC140,0x0000,
0x07FF,0x1020,0x0000,0x4784,0x24F1,0x0000,
0x8944,0x4120,0x0000,0x07FF,0x1020,0x0000,
0x4084,0x2481,0x0000,0x8924,0x233F,0x8000,
0x001F,0x1020,0x0000,0x2784,0x3C82,0x0001,
0x051C,0x1200,0x0000,0x000F,0x0810,0x0000,
0x3000,0x0006,0x0002,0x0200,0x0A00,0x0000,
0x0007,0x0408,0x0000,0x0800,0x0008,0x0000,
0x0000,0x0400,0x0000,0x0000,0x0204,0x0000,
0x0600,0x0030,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0002,0x0000,0x01E0,0x03C0,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x001F,0xFC00,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
};


newgadgets(window)

struct Window *window;

{

static struct Image newgadsimage =
{
0,0,          /* LeftEdge, TopEdge */
134,17,1,      /* Width, Height, Depth */
&newgadsData[0],
1,1,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};


static struct Gadget graph = {
   NULL,   /* next gadget */
   585,165,   /* origin XY of hit box relative to window TopLeft */
   41,17,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   0,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   GRAPH,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};


static struct Gadget playrange = {
   &graph,   /* next gadget */
   492,165,   /* origin XY of hit box relative to window TopLeft */
   37,17,   /* hit box width and height */
   GADGHCOMP|GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   &newgadsimage,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   PLAYRANGE,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static struct Gadget stop = {
   &playrange,   /* next gadget */
   538,165,   /* origin XY of hit box relative to window TopLeft */
   37,17,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   NULL,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   STOP,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

newdata=AllocMem(sizeof(newgadsData),MEMF_CHIP);
if (newdata==0) return(0);
movmem(&newgadsData[0],newdata,sizeof(newgadsData));
newgadsimage.ImageData=newdata;


AddGadget(window,&stop,-1);    /** add our new gadgets to the list **/
AddGadget(window,&playrange,-1);
AddGadget(window,&graph,-1);

RefreshGadgets(&stop,window,NULL);
}


closenewgadgets()
{
FreeMem(newdata,sizeof(newgadsData));
}
