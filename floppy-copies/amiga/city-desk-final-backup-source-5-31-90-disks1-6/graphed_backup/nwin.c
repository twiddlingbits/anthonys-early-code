#include <exec/types.h>
#include <intuition/intuition.h>


nwwin(screen)
struct Screen *screen;
{
UWORD var1,var2;
struct NewWindow NewWindow;
struct Window *Window;

    var2 = TRUE;
    NewWindow.LeftEdge = 0;
    NewWindow.TopEdge = 0;
    var1 = screen->BitMap.BytesPerRow;
    NewWindow.Width = var1 * 8;
    NewWindow.Height = screen->BitMap.Rows;
    NewWindow.DetailPen = 0;
    NewWindow.BlockPen = 1<<screen->BitMap.Depth -1;
    NewWindow.Title = NULL;
    NewWindow.Flags = BORDERLESS | SMART_REFRESH | ACTIVATE;
    NewWindow.IDCMPFlags = NULL;
    NewWindow.Type = CUSTOMSCREEN;
    NewWindow.FirstGadget = NULL;
    NewWindow.CheckMark = NULL;
    NewWindow.Screen = screen;
    NewWindow.BitMap = NULL;
    NewWindow.MinWidth = -1;
    NewWindow.MinHeight = -1;
    NewWindow.MaxWidth = -1;
    NewWindow.MaxHeight = -1;

    if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
        var2 =FALSE;

    return(Window);

}


