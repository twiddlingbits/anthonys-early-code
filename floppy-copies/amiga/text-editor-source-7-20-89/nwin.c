#include <exec/types.h>
#include <intuition/intuition.h>


nwin(screen,x,y,w,h)
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
    NewWindow.Title = "Text Editor";
    NewWindow.Flags = ACTIVATE | WINDOWSIZING | WINDOWDRAG;
    NewWindow.IDCMPFlags = NULL;
    NewWindow.Type = CUSTOMSCREEN;
    NewWindow.FirstGadget = NULL;
    NewWindow.CheckMark = NULL;
    NewWindow.Screen = screen;
    NewWindow.BitMap = NULL;
    NewWindow.MinWidth = 85;
    NewWindow.MinHeight = 95;
    NewWindow.MaxWidth = 640;
    NewWindow.MaxHeight = 400;


    if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
        exit(FALSE);

    return(Window);

}

