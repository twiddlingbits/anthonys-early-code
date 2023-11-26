#include <exec/types.h>
#include <intuition/intuition.h>

struct NewWindow MagWindow;
struct Window *MagWin;

magwin(screen)
struct Screen *screen;
{

    MagWindow.LeftEdge = 20;
    MagWindow.TopEdge = 20;
    MagWindow.Width = 244;
    MagWindow.Height = 116;
    MagWindow.DetailPen = 0;
    MagWindow.BlockPen = 1;
    MagWindow.Title = "     Magnifier  ";
    MagWindow.Flags = SMART_REFRESH | ACTIVATE | WINDOWCLOSE | WINDOWDRAG;
    MagWindow.IDCMPFlags = REPORTMOUSE;
    MagWindow.Type = CUSTOMSCREEN;
    MagWindow.FirstGadget = NULL;
    MagWindow.CheckMark = NULL;
    MagWindow.Screen = screen;
    MagWindow.BitMap = NULL;
    MagWindow.MinWidth = 0;
    MagWindow.MinHeight = 0;
    MagWindow.MaxWidth = 300;
    MagWindow.MaxHeight = 200;

    MagWin = (struct Window *)OpenWindow(&MagWindow);

    return(MagWin);

}


