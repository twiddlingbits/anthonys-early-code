
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>

ntextwin(screen,x,y,w,h)
struct Screen *screen;
int x,y,w,h;
{

extern USHORT *goobleimage1;
extern USHORT *goobleimage2;
extern struct Gadget *gooblegadget[3];
extern struct PropInfo *goobleprop;

static struct PropInfo Gadget3SInfo = {
   AUTOKNOB+FREEVERT,
   -1,-1,
   -1,-1,
};

static struct Image Image1 = {
   0,0,
   12,62,
   0,
   NULL,
   0x0000,0x0000,
   NULL
};

static struct Gadget Gadget3 = {
   NULL,
   -22,22,
   20,-43,
   GRELRIGHT+GRELHEIGHT,
   RELVERIFY,
   PROPGADGET,
   (APTR)&Image1,
   NULL,
   NULL,
   NULL,
   (APTR)&Gadget3SInfo,
   NULL,
   NULL
};

static USHORT ImageData2[] = {
   0xFFFF,0xF000,0x8000,0x1000,0x8000,0x1000,0xB000,0xD000,
   0xBC03,0xD000,0xBF0F,0xD000,0xBFFF,0xD000,0x8FFF,0x1000,
   0x83FC,0x1000,0x80F0,0x1000,0x8000,0x1000,0xFFFF,0xF000
};

static struct Image Image2 = {
   0,0,
   20,12,
   2,
   ImageData2,
   0x0001,0x0000,
   NULL
};

static struct Gadget Gadget2 = {
   &Gadget3,
   -22,-20,
   20,12,
   GADGIMAGE+GRELRIGHT+GRELBOTTOM,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Image2,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL
};

static USHORT ImageData3[] = {
   0xFFFF,0xF000,0x8000,0x1000,0x80F0,0x1000,0x83FC,0x1000,
   0x8FFF,0x1000,0xBFFF,0xD000,0xBF0F,0xD000,0xBC03,0xD000,
   0xB000,0xD000,0x8000,0x1000,0x8000,0x1000,0xFFFF,0xF000
};

static struct Image Image3 = {
   0,0,
   20,12,
   2,
   ImageData3,
   0x0001,0x0000,
   NULL
};

static struct Gadget Gadget1 = {
   &Gadget2,
   -22,10,
   20,12,
   GADGIMAGE+GRELRIGHT,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Image3,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL
};


    struct NewWindow NewWindow;
    struct Window *Window;

    NewWindow.LeftEdge = x;
    NewWindow.TopEdge = y;
    NewWindow.Width = w;
    NewWindow.Height = h;
    NewWindow.DetailPen = 0;
    NewWindow.BlockPen = 1;
    NewWindow.Title = "Text Editor";
    NewWindow.Flags = WINDOWCLOSE | ACTIVATE | WINDOWSIZING | WINDOWDRAG;
    NewWindow.IDCMPFlags = NULL;
    NewWindow.Type = CUSTOMSCREEN;
    NewWindow.FirstGadget = &Gadget1;
    NewWindow.CheckMark = NULL;
    NewWindow.Screen = screen;
    NewWindow.BitMap = NULL;
    NewWindow.MinWidth = 150;
    NewWindow.MinHeight = 37;
    NewWindow.MaxWidth = -1;
    NewWindow.MaxHeight = 400;

    if ((goobleimage1= (USHORT *) AllocMem(48,MEMF_CHIP))== NULL)
        return(-1);
    if ((goobleimage2= (USHORT *) AllocMem(48,MEMF_CHIP))== NULL)
        return(-1);

    movmem(ImageData2,goobleimage1,48);
    movmem(ImageData3,goobleimage2,48);

    Image3.ImageData = goobleimage2;
    Image2.ImageData = goobleimage1;

    gooblegadget[0] = &Gadget1;
    gooblegadget[1] = &Gadget2;
    gooblegadget[2] = &Gadget3;
    goobleprop = &Gadget3SInfo;

    if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
        return(-2);

    return(Window);

}

