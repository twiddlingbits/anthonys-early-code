/** maze.h **/

/** protos **/

void  CalcMaze(HWND hWnd);
short LegalClear(short x, short y, short dir);
short ComputeTrack(HWND hWnd, short x,short y,short dir);
void  ClearDir(HWND hWnd, short x,short y,short dir);
void  WallDir(HWND hWnd, short x,short y,short dir);
short PickNewDir(short x,short y,short currentdir);
short ReverseDir(short dir);
void  CalcNewXY(short *x,short *y,short dir);
void  WallEast(HWND hWnd, short x,short y);
void  WallWest(HWND hWnd, short x, short y);
void  WallNorth(HWND hWnd, short x, short y);
void  WallSouth(HWND hWnd, short x, short y);
void  ClearEast(HWND hWnd, short x, short y);
void  ClearWest(HWND hWnd, short x, short y);
void  ClearSouth(HWND hWnd, short x, short y);
void  ClearNorth(HWND hWnd, short x,short y);
void  DrawCell(HWND hWnd, short x, short y);
unsigned short myrand(void);
void PaintMaze(HWND hWnd, PAINTSTRUCT *ps);
short TryCell(HWND hWnd, short x, short y);
void SolveMaze(HWND hWnd);
short IsMazeSolved(short x, short y);
void DrawTryCell(HWND hWnd, short x, short y, short op);
void PrintMaze(HWND hWnd);
void DrawCellWithHDC(HDC hDC, short x, short y, short line_width, short , short);
HDC GetPrinterDC (void);
BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT message, UINT wParam,LONG lParam);
BOOL CALLBACK AbortProc (HDC hdcPrn, short nCode);
void PrintCells(HWND hwnd, HDC hDC);

/** defines **/

//#define COLOR_BACKGROUND 0
//#define COLOR_DRAW 1

#define RECT_HEIGHT(r) (r.bottom - r.top)
#define RECT_WIDTH(r) (r.right - r.left)

#define CELL_WIDTH_IN_PIXALS 12
#define CELL_HEIGHT_IN_PIXALS 10
#define MAX_MAZE_WIDTH_IN_CELLS (1024/CELL_WIDTH_IN_PIXALS+1)	/** MAX **/
#define MAX_MAZE_HEIGHT_IN_CELLS (768/CELL_HEIGHT_IN_PIXALS+1)	/** MAX **/
#define OFFSET_Y 30
#define OFFSET_X 20

#define LINE_NORTH ((short)(1<<0))
#define LINE_SOUTH ((short)(1<<1))
#define LINE_EAST  ((short)(1<<2))
#define LINE_WEST  ((short)(1<<3))
#define CELL_DONE  ((short)(1<<4))
#define CELL_TRIED ((short)(1<<5))

