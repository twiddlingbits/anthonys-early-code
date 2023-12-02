#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <fcntl.h>

static struct NewWindow NewWindow = {
   0,
   0,
   300,
   250,
   0,	/** color **/
   3,	/** color **/
   CLOSEWINDOW|NEWSIZE,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|WINDOWSIZING,
   0,0,
   "AW's MAZE", /** TITLE **/
   0,0,
   40,20,-1,-1,
   WBENCHSCREEN
   };

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Window *window;
static unsigned short seed;
short maze_width_in_cells;

void OpenLibraries();
void CloseLibraries();
void DrawCell();
void SetUp();
void CleanUp();
void WallEast();
void WallWest();
void WallNorth();
void WallSouth();
void CalcNewXY();
short PickNewDir();
void RenderMaze();
void ClearDir();
void ClearEast();
void ClearWest();
void ClearNorth();
void ClearSouth();
unsigned short myrand();
void WallDir();
void CalcMaze();


#define COLOR_BACKGROUND 0
#define COLOR_DRAW 1

#define MAZE_WIDTH_IN_CELLS 110	/** MAX **/
#define CELL_WIDTH_IN_PIXALS 8
#define OFFSET_Y 30

#define LINE_NORTH (1<<0)
#define LINE_SOUTH (1<<1)
#define LINE_EAST  (1<<2)
#define LINE_WEST  (1<<3)
#define CELL_DONE  (1<<4)

unsigned short cell[MAZE_WIDTH_IN_CELLS][MAZE_WIDTH_IN_CELLS];	/*x, y */

main()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
short calcmaze;

OpenLibraries();
SetUp();

CalcMaze();

while (TRUE) {
	calcmaze=FALSE;
	while ((message=(struct IntuiMessage *)GetMsg(window->UserPort))!=0) {
		class=message->Class;
		code=message->Code;
		ReplyMsg(message);

		if (class==CLOSEWINDOW) {
			CleanUp();
			exit(0);
			}

		else if (class==NEWSIZE) {
			calcmaze=TRUE;
			}
		}
	if (calcmaze)
		CalcMaze();
	}
}

/********************************************************/

void CalcMaze()
{
short xy,x,y,i,k;

/** Clear window & calc maze size **/

SetAPen(window->RPort, COLOR_BACKGROUND);
RectFill(window->RPort, 20, OFFSET_Y,window->Width-20,window->Height-3);

maze_width_in_cells = min (
	(window->Height-OFFSET_Y-8)/CELL_WIDTH_IN_PIXALS,
	(window->Width-42)/CELL_WIDTH_IN_PIXALS);

for (x=0; x < maze_width_in_cells; x++) {
	for (y=0; y < maze_width_in_cells; y++) {
		cell[x][y]=0;
		}
	}

/** First, draw outline of maze **/
/** origin is upper left corner, like a pixmap **/

for (xy=0; xy < maze_width_in_cells; xy++) {
	WallWest(0,xy);								/* Left edge */
	WallEast(maze_width_in_cells-1,xy);		/* right edge */
	WallNorth(xy,0);	 							/* top edge */
	WallSouth(xy,maze_width_in_cells-1);	/* bottom edge */
	}

/** Compute maze! **/

ComputeTrack(0,0, LINE_SOUTH);

/*** MAKE THIS RANDODOM!!! **/

for (y=0; y < maze_width_in_cells; y++) {
	for (x=0; x < maze_width_in_cells; x++) {
		if (!(cell[x][y]&CELL_DONE)) {
			if ((cell[x][y]&0xF)==0xF) {
				cell[x][y] |= CELL_DONE;
				if (LegalClear(x,y,LINE_NORTH))
					ClearNorth(x,y);		/** FIX! **/
				else
					ClearSouth(x,y);		/** FIX! **/
				}
			else {
				for (i=0; i < 4; i++) { /** Find direction of next track **/
					if (!((1<<i)&cell[x][y])) {
						for (k=0; k < 4; k++) {  /** find who it connects too **/
							if ((1<<k)&cell[x][y]) {
								if (LegalClear(x,y,1<<k)) {
									break;
									}
								}
							}
if (k==4) printf("NO PLACE TO CLEAR!\n");
							ComputeTrack(x,y, 1<<i);
							ClearDir(x,y,1<<k);	/** FIX THIS RIGHT1!! **/
							break;
							}
						}
if (i==4) printf("NO DIR TO GO IN!!\n");
					}
				}
			}
		}

/** Clear entrance and exit **/

ClearDir(0,0,LINE_NORTH);
ClearDir(maze_width_in_cells-1,maze_width_in_cells-1,LINE_SOUTH);
}

/********************************************************/

LegalClear(x,y,dir)
short x,y,dir;
{
if (y==maze_width_in_cells-1 && (dir&LINE_SOUTH))
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

void RenderMaze()
{
short x,y;

/** Render the Maze **/

for (x=0; x < maze_width_in_cells; x++) {
	for (y=0; y < maze_width_in_cells; y++) {
		DrawCell(x,y);
		}
	}
}
	

/********************************************************/

ComputeTrack(x,y,dir)
short x,y,dir;
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
	newdir = PickNewDir(x,y,dir);
	WallDir(x,y,dir);
	}
else {
	newdir = dir;	/** just make non zero **/
	}

/** Draw sides of cell based on current direction **/

if (dir&LINE_NORTH || dir&LINE_SOUTH) {
	WallEast(x,y);
	WallWest(x,y);
	}
else if (dir&LINE_EAST || dir&LINE_WEST) {
	WallNorth(x,y);
	WallSouth(x,y);
	}

if (newdir==0) {
	return(CELL_DONE);
	}

/** Get next x,y based on current direction **/

ClearDir(x,y,newdir);
CalcNewXY(&x,&y,newdir);

return(ComputeTrack(x,y,newdir));
}

/********************************************************/

void ClearDir(x,y,dir)
short x,y,dir;
{
if (dir&LINE_EAST)
	ClearEast(x,y);
else if (dir&LINE_WEST)
	ClearWest(x,y);
else if (dir&LINE_NORTH)
	ClearNorth(x,y);
else if (dir&LINE_SOUTH)
	ClearSouth(x,y);
}
/********************************************************/

void WallDir(x,y,dir)
short x,y,dir;
{
if (dir&LINE_EAST)
	WallEast(x,y);
else if (dir&LINE_WEST)
	WallWest(x,y);
else if (dir&LINE_NORTH)
	WallNorth(x,y);
else if (dir&LINE_SOUTH)
	WallSouth(x,y);
}

/********************************************************/

short PickNewDir(x,y,currentdir)
short x,y,currentdir;
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

ReverseDir(dir)
short dir;
{
if (dir&LINE_NORTH)
	return(LINE_SOUTH);
else if (dir&LINE_SOUTH)
	return(LINE_NORTH);
else if (dir&LINE_EAST)
	return(LINE_WEST);
else if (dir&LINE_WEST)
	return(LINE_EAST);
}

/********************************************************/

void CalcNewXY(x,y,dir)

short *x,*y,dir;
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

void WallEast(x,y)
short x,y;
{
cell[x][y] |= LINE_EAST;
DrawCell(x,y);
/** Also need to adjust neighbor **/

if (x+1 < maze_width_in_cells) {
	cell[x+1][y] |= LINE_WEST;
	DrawCell(x+1,y);
	}
}

/********************************************************/

void WallWest(x,y)
short x,y;
{
cell[x][y] |= LINE_WEST;
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (x-1 >= 0) {
	cell[x-1][y] |= LINE_EAST;
	DrawCell(x-1,y);
	}
}

/********************************************************/

void WallSouth(x,y)
short x,y;
{
cell[x][y] |= LINE_SOUTH;
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (y+1 < maze_width_in_cells) {
	cell[x][y+1] |= LINE_NORTH;
	DrawCell(x,y+1);
	}
}

/********************************************************/

void WallNorth(x,y)
short x,y;
{
cell[x][y] |= LINE_NORTH;
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (y-1 >= 0) {
	cell[x][y-1] |= LINE_SOUTH;
	DrawCell(x,y-1);
	}
}


/********************************************************/

void ClearEast(x,y)
short x,y;
{
cell[x][y] &= (~LINE_EAST);
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (x+1 < maze_width_in_cells) {
	cell[x+1][y] &= (~LINE_WEST);
	DrawCell(x+1,y);
	}
}

/********************************************************/

void ClearWest(x,y)
short x,y;
{
cell[x][y] &= (~LINE_WEST);
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (x-1 >= 0) {
	cell[x-1][y] &= (~LINE_EAST);
	DrawCell(x-1,y);
	}
}

/********************************************************/

void ClearSouth(x,y)
short x,y;
{
cell[x][y] &= (~LINE_SOUTH);
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (y+1 < maze_width_in_cells) {
	cell[x][y+1] &= (~LINE_NORTH);
	DrawCell(x,y+1);
	}
}

/********************************************************/

void ClearNorth(x,y)
short x,y;
{
cell[x][y] &= (~LINE_NORTH);
DrawCell(x,y);

/** Also need to adjust neighbor **/

if (y-1 >= 0) {
	cell[x][y-1] &= (~LINE_SOUTH);
	DrawCell(x,y-1);
	}
}


/********************************************************/

void DrawCell(x,y)
short x,y;
{

SetAPen(window->RPort, COLOR_BACKGROUND);
RectFill(window->RPort, 20+x*CELL_WIDTH_IN_PIXALS, OFFSET_Y+y*CELL_WIDTH_IN_PIXALS,
	20+(x+1)*CELL_WIDTH_IN_PIXALS-1,OFFSET_Y+(y+1)*CELL_WIDTH_IN_PIXALS-1);

SetAPen(window->RPort, COLOR_DRAW);

if (cell[x][y]&LINE_NORTH){
	Move(window->RPort, 20+x*CELL_WIDTH_IN_PIXALS, OFFSET_Y+y*CELL_WIDTH_IN_PIXALS);
	Draw(window->RPort, 20+(x+1)*(CELL_WIDTH_IN_PIXALS)-1, OFFSET_Y+y*CELL_WIDTH_IN_PIXALS);
	}

if (cell[x][y]&LINE_SOUTH) {
	Move(window->RPort, 20+x*CELL_WIDTH_IN_PIXALS, OFFSET_Y+(y+1)*(CELL_WIDTH_IN_PIXALS)-1);
	Draw(window->RPort, 20+(x+1)*(CELL_WIDTH_IN_PIXALS)-1, OFFSET_Y+(y+1)*(CELL_WIDTH_IN_PIXALS)-1);
	}

if (cell[x][y]&LINE_WEST) {
	Move(window->RPort, 20+x*CELL_WIDTH_IN_PIXALS, OFFSET_Y+y*CELL_WIDTH_IN_PIXALS);
	Draw(window->RPort, 20+x*CELL_WIDTH_IN_PIXALS, OFFSET_Y+(y+1)*(CELL_WIDTH_IN_PIXALS)-1);
	}

if (cell[x][y]&LINE_EAST) {
	Move(window->RPort, 20+(x+1)*(CELL_WIDTH_IN_PIXALS)-1, OFFSET_Y+y*CELL_WIDTH_IN_PIXALS);
	Draw(window->RPort, 20+(x+1)*(CELL_WIDTH_IN_PIXALS)-1, OFFSET_Y+(y+1)*(CELL_WIDTH_IN_PIXALS)-1);
	}
}

/********************************************************/

void SetUp()
{
OpenLibraries();

window = (struct Window *)OpenWindow(&NewWindow);
if (window==0) {
   printf("can't open back window\n");
   CloseLibraries();
	exit(10);
   }

/** Set my random number generator seed to Horiz. Beam Pos **/

seed = *((unsigned short *)0xDFF006);
}

/********************************************************/

void CleanUp()
{
CloseWindow(window);
CloseLibraries();
}

/********************************************************/

void OpenLibraries()
{
IntuitionBase = 0;
GfxBase = 0;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   printf("Use Workbench 1.2 or greater\n");
   CloseLibraries();
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!\n");
   CloseLibraries();
   exit(10);
   }
}

/********************************************************/

void CloseLibraries()
{
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);
}

/********************************************************/

unsigned short myrand()
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
