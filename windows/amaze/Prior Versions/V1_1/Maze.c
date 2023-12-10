#include "windows.h" 
#include "maze.h"     
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
  
/********************************************************/

extern HINSTANCE hInst;               /* current instance */

/********************************************************/

int solve_speed=250;
static unsigned short seed=221;		/** Need a more random seed **/
short maze_width_in_cells;
short maze_height_in_cells;
unsigned short cell[MAX_MAZE_WIDTH_IN_CELLS][MAX_MAZE_HEIGHT_IN_CELLS];	/*x, y */
//unsigned short solve_cell[MAX_MAZE_WIDTH_IN_CELLS][MAX_MAZE_HEIGHT_IN_CELLS];	/*x, y */

/********************************************************/

void CalcMaze(HWND hWnd)
{
short x,y,i,k;
RECT rectClient;
HDC hDC;
HBRUSH hbr;

GetClientRect (hWnd, &rectClient);

/** Clear window & calc maze size **/

hDC = GetDC(hWnd);
hbr=GetStockObject(WHITE_BRUSH);
FillRect(hDC, &rectClient, hbr);
ReleaseDC(hWnd, hDC);

maze_height_in_cells=(RECT_HEIGHT(rectClient)-OFFSET_Y*2)/CELL_HEIGHT_IN_PIXALS;
maze_width_in_cells=(RECT_WIDTH(rectClient)-OFFSET_X*2)/CELL_WIDTH_IN_PIXALS;

for (x=0; x < maze_width_in_cells; x++) {
	for (y=0; y < maze_height_in_cells; y++) {
		cell[x][y]=0;
        }
	}
 
/** First, draw outline of maze **/
/** origin is upper left corner, like a pixmap **/

for (x=0; x < maze_width_in_cells; x++) {
	WallNorth(hWnd, x, 0);	 						/* top edge */
	WallSouth(hWnd, x, (short)(maze_height_in_cells-1));		/* bottom edge */
	}

for (y=0; y < maze_height_in_cells; y++) {
	WallWest (hWnd, 0, y);							/* Left edge */
	WallEast (hWnd, (short)(maze_width_in_cells-1), y);		/* right edge */
	}

/** Compute maze! **/

ComputeTrack(hWnd, 0, 0, LINE_SOUTH);

/*** MAKE THIS RANDODOM!!! **/

for (y=0; y < maze_height_in_cells; y++) {
	for (x=0; x < maze_width_in_cells; x++) {
		if (!(cell[x][y]&CELL_DONE)) {
			if ((cell[x][y]&0xF)==0xF) {
				cell[x][y] |= CELL_DONE;
				if (LegalClear(x,y,LINE_NORTH))
					ClearNorth(hWnd, x,y);		/** FIX! **/
				else
					ClearSouth(hWnd, x, y);		/** FIX! **/
				}
			else {
				for (i=0; i < 4; i++) { /** Find direction of next track **/
					if (!((1<<i)&cell[x][y])) {
						for (k=0; k < 4; k++) {  /** find who it connects too **/
							if ((1<<k)&cell[x][y]) {
								if (LegalClear(x,y,(short)(1<<k))) {
									break;
									}
								}
							}
//if (k==4) printf("NO PLACE TO CLEAR!\n");
							ComputeTrack(hWnd, x, y, (short)(1<<i));
							ClearDir(hWnd, x, y, (short)(1<<k));	/** FIX THIS RIGHT1!! **/
							break;
							}
						}
//if (i==4) printf("NO DIR TO GO IN!!\n");
					}
				}
			}
		}

/** Clear entrance and exit **/

ClearDir(hWnd, 0, 0, LINE_NORTH);
ClearDir(hWnd, (short)(maze_width_in_cells-1), (short)(maze_height_in_cells-1), LINE_SOUTH);
}

/********************************************************/

short LegalClear(short x, short y, short dir)
{
if (y==maze_height_in_cells-1 && (dir&LINE_SOUTH))
	return(FALSE);
if (x==maze_width_in_cells-1 && (dir&LINE_EAST))
	return(FALSE);
if (y==0 && (dir&LINE_NORTH))
	return(FALSE);
if (x==0 && (dir&LINE_WEST))
	return(FALSE);
return(TRUE);
}

/********************************************************/

short ComputeTrack(HWND hWnd, short x,short y,short dir)
{
short newdir;
static short kount;

/*printf("x %d y %d dir %x\n",x,y,dir);*/
/*
kount++;
if (kount==2) {
	kount=0;
	Delay(1);
	}*/

/** Mark this cell as allocated **/

cell[x][y] |= CELL_DONE;

/** If current dir is blocked, then pick a new one or quit **/

if (cell[x][y]&dir || myrand() <= 0x6000) {	/* orig 6000 */
	newdir = PickNewDir(x, y, dir);
	WallDir(hWnd, x, y, dir);
	}
else {
	newdir = dir;	/** just make non zero **/
	}

/** Draw sides of cell based on current direction **/

if (dir&LINE_NORTH || dir&LINE_SOUTH) {
	WallEast(hWnd, x, y);
	WallWest(hWnd, x, y);
	}
else if (dir&LINE_EAST || dir&LINE_WEST) {
	WallNorth(hWnd, x, y);
	WallSouth(hWnd, x, y);
	}

if (newdir==0) {
	return(CELL_DONE);
	}

/** Get next x,y based on current direction **/

ClearDir(hWnd, x, y, newdir);
CalcNewXY(&x,&y,newdir);

return(ComputeTrack(hWnd, x, y, newdir));
}

/********************************************************/

void ClearDir(HWND hWnd, short x,short y,short dir)
{
if (dir&LINE_EAST)
	ClearEast(hWnd, x, y);
else if (dir&LINE_WEST)
	ClearWest(hWnd, x, y);
else if (dir&LINE_NORTH)
	ClearNorth(hWnd, x, y);
else if (dir&LINE_SOUTH)
	ClearSouth(hWnd, x, y);
}
/********************************************************/

void WallDir(HWND hWnd, short x,short y,short dir)
{
if (dir&LINE_EAST)
	WallEast(hWnd, x,y);
else if (dir&LINE_WEST)
	WallWest(hWnd, x,y);
else if (dir&LINE_NORTH)
	WallNorth(hWnd, x,y);
else if (dir&LINE_SOUTH)
	WallSouth(hWnd, x,y);
}

/********************************************************/

short PickNewDir(short x,short y,short currentdir)
{
short dir;
short revdir;
short xxcell;

revdir = ReverseDir(currentdir);
xxcell=cell[x][y];
cell[x][y] |= revdir;
cell[x][y] |= currentdir;	/** Fource change in dir **/

if (!(cell[x][y]&LINE_NORTH))
	dir = LINE_NORTH;
else if (!(cell[x][y]&LINE_SOUTH))
	dir = LINE_SOUTH;
else if (!(cell[x][y]&LINE_EAST))
	dir = LINE_EAST;
else if (!(cell[x][y]&LINE_WEST))
	dir = LINE_WEST;
else
	 dir = 0;
cell[x][y]=xxcell;
return(dir);
}

/********************************************************/

short ReverseDir(short dir)
{
if (dir&LINE_NORTH)
	return(LINE_SOUTH);
else if (dir&LINE_SOUTH)
	return(LINE_NORTH);
else if (dir&LINE_EAST)
	return(LINE_WEST);
else // always if (dir&LINE_WEST)
	return(LINE_EAST);
}

/********************************************************/

void CalcNewXY(short *x,short *y,short dir)
{
if (dir&LINE_NORTH)
	(*y)--;
else if (dir&LINE_SOUTH)
	(*y)++;
else if (dir&LINE_EAST)
	(*x)++;
else if (dir&LINE_WEST)
	(*x)--;
}

/********************************************************/

void WallEast(HWND hWnd, short x, short y)
{
cell[x][y] |= LINE_EAST;
DrawCell(hWnd, x, y);
/** Also need to adjust neighbor **/

if (x+1 < maze_width_in_cells) {
	cell[x+1][y] |= LINE_WEST;
	DrawCell(hWnd, (short)(x+1), y);
	}
}

/********************************************************/

void WallWest(HWND hWnd, short x, short y)
{
cell[x][y] |= LINE_WEST;
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (x-1 >= 0) {
	cell[x-1][y] |= LINE_EAST;
	DrawCell(hWnd, (short)(x-1),y);
	}
}

/********************************************************/

void WallSouth(HWND hWnd, short x, short y)
{
cell[x][y] |= LINE_SOUTH;
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (y+1 < maze_height_in_cells) {
	cell[x][y+1] |= LINE_NORTH;
	DrawCell(hWnd, x, (short)(y+1));
	}
}

/********************************************************/

void WallNorth(HWND hWnd, short x, short y)
{
cell[x][y] |= LINE_NORTH;
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (y-1 >= 0) {
	cell[x][y-1] |= LINE_SOUTH;
	DrawCell(hWnd, x, (short)(y-1));
	}
}


/********************************************************/

void ClearEast(HWND hWnd, short x, short y)
{
cell[x][y] &= (~LINE_EAST);
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (x+1 < maze_width_in_cells) {
	cell[x+1][y] &= (~LINE_WEST);
	DrawCell(hWnd, (short)(x+1), y);
	}
}

/********************************************************/

void ClearWest(HWND hWnd, short x, short y)
{
cell[x][y] &= (~LINE_WEST);
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (x-1 >= 0) {
	cell[x-1][y] &= (~LINE_EAST);
	DrawCell(hWnd, (short)(x-1),y);
	}
}

/********************************************************/

void ClearSouth(HWND hWnd, short x, short y)
{
cell[x][y] &= (~LINE_SOUTH);
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (y+1 < maze_height_in_cells) {
	cell[x][y+1] &= (~LINE_NORTH);
	DrawCell(hWnd, x, (short)(y+1));
	}
}

/********************************************************/

void ClearNorth(HWND hWnd, short x,short y)
{
cell[x][y] &= (~LINE_NORTH);
DrawCell(hWnd, x, y);

/** Also need to adjust neighbor **/

if (y-1 >= 0) {
	cell[x][y-1] &= (~LINE_SOUTH);
	DrawCell(hWnd, x, (short)(y-1));
	}
}


/********************************************************/

void DrawCell(HWND hWnd, short x, short y)
{
HDC hDC;

hDC=GetDC(hWnd);
DrawCellWithHDC(hDC, x, y, 1, CELL_WIDTH_IN_PIXALS, CELL_HEIGHT_IN_PIXALS);
ReleaseDC(hWnd, hDC);
}

/********************************************************/

void DrawCellWithHDC(
	HDC hDC
	,short x
	,short y
	,short line_width
	,short cell_width
	,short cell_height)
{
HBRUSH hbr;
HPEN hpen,hpenold;
RECT rcell;

rcell.left=OFFSET_X+x*cell_width;
rcell.top=OFFSET_Y+y*cell_height;
rcell.right=OFFSET_X+(x+1)*cell_width;
rcell.bottom=OFFSET_Y+(y+1)*cell_height;

hbr=GetStockObject(WHITE_BRUSH);
FillRect(hDC, &rcell, hbr);

hpen=CreatePen(PS_SOLID, line_width, RGB(0, 0, 0));
hpenold=SelectObject(hDC, hpen);

if (cell[x][y]&LINE_NORTH){
	MoveToEx(hDC, OFFSET_X+x*cell_width, OFFSET_Y+y*cell_height, NULL);
	LineTo(hDC, OFFSET_X+(x+1)*(cell_width), OFFSET_Y+y*cell_height);
	}

if (cell[x][y]&LINE_SOUTH) {
	MoveToEx(hDC, OFFSET_X+x*cell_width, OFFSET_Y+(y+1)*(cell_height)-1, NULL);
	LineTo(hDC, OFFSET_X+(x+1)*(cell_width), OFFSET_Y+(y+1)*(cell_height)-1);
	}

if (cell[x][y]&LINE_WEST) {
	MoveToEx(hDC, OFFSET_X+x*cell_width, OFFSET_Y+y*cell_height, NULL);
	LineTo(hDC, OFFSET_X+x*cell_width, OFFSET_Y+(y+1)*(cell_height));
	}

if (cell[x][y]&LINE_EAST) {
	MoveToEx(hDC, OFFSET_X+(x+1)*(cell_width)-1, OFFSET_Y+y*cell_height, NULL);
	LineTo(hDC, OFFSET_X+(x+1)*(cell_width)-1, OFFSET_Y+(y+1)*(cell_height));
	}

SelectObject(hDC, hpenold);
DeleteObject(hpen);
}
          
/********************************************************/

unsigned short myrand(void)
{
unsigned short x,y;

x = seed&1;
y = seed&8;
y = y >>3;
x = (x^y)&1;

seed = seed>>1;
seed |= x<<15;

return(seed);
}

/********************************************************/

void PaintMaze(HWND hWnd, PAINTSTRUCT *ps)
{
RECT dummy;
RECT rcell;
short x,y;
     
for (x=0; x < maze_width_in_cells; x++) {
	for (y=0; y < maze_height_in_cells; y++) {
		rcell.left=OFFSET_X+x*CELL_WIDTH_IN_PIXALS;
		rcell.top=OFFSET_Y+y*CELL_HEIGHT_IN_PIXALS;
		rcell.right=OFFSET_X+(x+1)*(CELL_WIDTH_IN_PIXALS);
		rcell.bottom=OFFSET_Y+(y+1)*(CELL_HEIGHT_IN_PIXALS);
		if (IntersectRect(&dummy, &rcell, &ps->rcPaint))
			DrawCell(hWnd, x, y);
        }
	}
}
  
/********************************************************/

void SolveMaze(HWND hWnd)
{
TryCell(hWnd, 0, 0);
}

/********************************************************/

short TryCell(HWND hWnd, short x, short y)
{
short found_end;

if (cell[x][y]&CELL_TRIED)
	return(FALSE);

cell[x][y]|=CELL_TRIED;
DrawTryCell(hWnd, x, y, R2_BLACK);

if (IsMazeSolved(x,y))
	return(TRUE);

found_end=FALSE;

if (!(cell[x][y]&LINE_EAST)) {
	found_end=TryCell(hWnd, (short)(x+1), y);
	}

if (!found_end && !(cell[x][y]&LINE_NORTH) && !(x==0 && y==0)) {  /** don't go out entrance **/
	found_end=TryCell(hWnd, x, (short)(y-1));
	}
	
if (!found_end && !(cell[x][y]&LINE_SOUTH)) {
	found_end=TryCell(hWnd, x, (short)(y+1));
	}

if (!found_end && !(cell[x][y]&LINE_WEST)) {
	found_end=TryCell(hWnd, (short)(x-1), y);
	}

if (!found_end)
	DrawTryCell(hWnd, x, y, R2_WHITE);    /* erase uncessfull try */

return(found_end);
}
	
	
/********************************************************/

short IsMazeSolved(short x, short y)
{
return((maze_height_in_cells-1)==y && (maze_width_in_cells-1)==x);
}

/********************************************************/

void DrawTryCell(HWND hWnd, short x, short y, short op)
{
HDC hDC;
short px, py;
long i;
MSG msg;
	
hDC = GetDC(hWnd);
SetROP2(hDC, op);

px=OFFSET_X+x*CELL_WIDTH_IN_PIXALS+CELL_WIDTH_IN_PIXALS/2;
py=OFFSET_Y+y*CELL_HEIGHT_IN_PIXALS+CELL_HEIGHT_IN_PIXALS/2;

MoveToEx(hDC, px, py, 0);
LineTo(hDC, px+1, py);
LineTo(hDC, px+1, py+2);
LineTo(hDC, px, py+2);
LineTo(hDC, px, py);
ReleaseDC(hWnd, hDC);

/** pause **/

for (i=0; i < solve_speed; i++) {
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    	TranslateMessage(&msg);    /* Translates virtual key codes       */
    	DispatchMessage(&msg);     /* Dispatches message to window       */
    	}
    }
}

/********************************************************/

BOOL   bUserAbort ;
HWND   hDlgPrint ;

/********************************************************/

void PrintMaze (HWND hwnd)
{
extern HINSTANCE hInst;
static char szMessage [] = "Maze: Printing" ; 
FARPROC     lpfnAbortProc;
DLGPROC		lpfnPrintDlgProc;
HDC         hdcPrn ;

if (NULL == (hdcPrn = GetPrinterDC ()))
	return;

EnableWindow (hwnd, FALSE) ;
bUserAbort = FALSE ;
lpfnPrintDlgProc = PrintDlgProc;
hDlgPrint = CreateDialog(hInst, (LPCTSTR)"PrintDlgBox", hwnd, lpfnPrintDlgProc) ;
lpfnAbortProc = MakeProcInstance ((FARPROC) AbortProc, hInst) ;
Escape (hdcPrn, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;

if (Escape (hdcPrn, STARTDOC, sizeof(szMessage) - 1, szMessage, NULL) > 0)
     {
     PrintCells(hwnd, hdcPrn);
     if (Escape (hdcPrn, NEWFRAME, 0, NULL, NULL) > 0)
          Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;
     }

EnableWindow (hwnd, TRUE) ;     
DestroyWindow (hDlgPrint) ;
hDlgPrint = 0 ;
FreeProcInstance (lpfnPrintDlgProc) ;
FreeProcInstance (lpfnAbortProc) ;
DeleteDC (hdcPrn) ;
return;
}

/********************************************************/

void PrintCells(HWND hwnd, HDC hDC)
{
RECT rc;
short x,y;
short line_width;
short cell_width;
short cell_height;
short xpage, ypage;

xpage = GetDeviceCaps (hDC, HORZRES);
ypage = GetDeviceCaps (hDC, VERTRES);

/* calc printer mapping **/

GetWindowRect(GetDesktopWindow(), &rc);
line_width=(short)min(xpage/rc.right, ypage/rc.bottom);
if (line_width < 1)
	line_width=1;
cell_width=CELL_WIDTH_IN_PIXALS*line_width;
cell_height=CELL_HEIGHT_IN_PIXALS*line_width;

/** now print each cell **/

for (x=0; x < maze_width_in_cells; x++) {
	for (y=0; y < maze_height_in_cells; y++) {
		DrawCellWithHDC(hDC, x, y, line_width, cell_width, cell_height);
        }
	}
}

/********************************************************/

BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
{
switch (message)
     {
     case WM_INITDIALOG:
          EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE,
                                                       MF_GRAYED) ;
          return(TRUE);

     case WM_COMMAND:
          bUserAbort = TRUE ;
          return(TRUE);
     }
     
return(FALSE);
}          

/********************************************************/

BOOL CALLBACK AbortProc (HDC hdcPrn, short nCode)
{
MSG   msg ;

while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
     {
     if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     }
return (!bUserAbort);
}

/********************************************************/

HDC GetPrinterDC (void)
{
static char szPrinter [80] ;
char        *szDevice, *szDriver, *szOutput ;

GetProfileString ("windows", "device", ",,,", szPrinter, 80) ;

if (NULL != (szDevice = strtok (szPrinter, "," )) &&
    NULL != (szDriver = strtok (NULL,      ", ")) &&
    NULL != (szOutput = strtok (NULL,      ", ")))
          
	return(CreateDC (szDriver, szDevice, szOutput, NULL));

return(0);
}

/********************************************************/
