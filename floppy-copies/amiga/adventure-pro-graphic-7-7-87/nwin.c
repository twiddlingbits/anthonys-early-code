#include <exec/types.h>
#include <intuition/intuition.h>


nwwin(screen,x,y,w,h)
struct Screen *screen;
int x,y,w,h;
{

    struct NewWindow NewWindow;
    struct Window *Window;

    NewWindow.LeftEdge = x;
    NewWindow.TopEdge = y;
    NewWindow.Width = w;
    NewWindow.Height = h;
    NewWindow.DetailPen = 0;
    NewWindow.BlockPen = 1;
    NewWindow.Title = NULL;
    NewWindow.Flags = SMART_REFRESH |
                      ACTIVATE | BORDERLESS ;
    NewWindow.IDCMPFlags = NULL;
    NewWindow.Type = CUSTOMSCREEN;
    NewWindow.FirstGadget = NULL;
    NewWindow.CheckMark = NULL;
    NewWindow.Screen = screen;
    NewWindow.BitMap = NULL;
    NewWindow.MinWidth = 0;
    NewWindow.MinHeight = 0;
    NewWindow.MaxWidth = 640;
    NewWindow.MaxHeight = 400;

    if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
        exit(FALSE);

    return(Window);

}

