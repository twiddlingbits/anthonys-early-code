#include <exec/types.h>
#include <intuition/intuition.h>


nwwin()
{

    struct NewWindow NewWindow;
    struct Window *Window;

    NewWindow.LeftEdge = 0;
    NewWindow.TopEdge = 0;
    NewWindow.Width = 300;
    NewWindow.Height = 100;
    NewWindow.DetailPen = 0;
    NewWindow.BlockPen = 1;
    NewWindow.Title = "soundtest win ";
    NewWindow.Flags = SMART_REFRESH | WINDOWDEPTH |
                      ACTIVATE ;
    NewWindow.IDCMPFlags = NULL;
    NewWindow.Type = WBENCHSCREEN;
    NewWindow.FirstGadget = NULL;
    NewWindow.CheckMark = NULL;
    NewWindow.Screen = NULL;
    NewWindow.BitMap = NULL;
    NewWindow.MinWidth = 0;
    NewWindow.MinHeight = 0;
    NewWindow.MaxWidth = 640;
    NewWindow.MaxHeight = 400;

    if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
        exit(FALSE);

    return(Window);

}

