
#include <exec/types.h>
#include <intuition/intuition.h>

struct NewWindow MagWindow;
struct Window *MagWin;

magwin(screen)
struct Screen *screen;
{

    MagWindow.LeftEdge = 20;
    MagWindow.TopEdge = 20;
    MagWindow.Width = 250;
    MagWindow.Height = 126;
    MagWindow.DetailPen = 0;
    MagWindow.BlockPen = 15;
    MagWindow.Title = "     Magnifier  ";
    MagWindow.Flags = RMBTRAP | SMART_REFRESH | ACTIVATE | WINDOWCLOSE | WINDOWDRAG;
    MagWindow.IDCMPFlags = REPORTMOUSE;
    MagWindow.Type = CUSTOMSCREEN;
    MagWindow.FirstGadget = NULL;
    MagWindow.CheckMark = NULL;
    MagWindow.Screen = screen;
    MagWindow.BitMap = NULL;
    MagWindow.MinWidth = -1;
    MagWindow.MinHeight = -1;
    MagWindow.MaxWidth = -1;
    MagWindow.MaxHeight = -1;

    MagWin = (struct Window *)OpenWindow(&MagWindow);

    return(MagWin);

}


