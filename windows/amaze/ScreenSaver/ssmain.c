#include <windows.h>
#include <scrnsave.h>
#include <assert.h>
#include "ssmain.h"
#include "../maze.h"
#include "../stack.h"

#define MINVEL  1               /* minimum h scroll bar value    */ 
#define MAXVEL  10              /* maximum h scroll bar value    */ 
 
LONG    lSpeed = DEF_SPEED;			/* redraw speed variable         */ 
LONG    lCellSize = DEF_CELLSIZE;	/* redraw speed variable         */  
LONG	lBlackBG = DEF_BLACK_BK;	/* draw black or white bg?		 */
LONG	lSlowDraw = DEF_SLOW_DRAW;	/* Low Maze Draw			 */
extern HINSTANCE hMainInstance;		/* screen saver instance handle  */ 
 
CHAR   szTemp[20];              /* temporary array of characters */ 
CHAR   szRedrawSpeed[] = "Redraw Speed";   /* .ini speed entry   */ 
CHAR   szCellSize[] = "Cell Size";
CHAR   szBlackBG[] = "Black Background";
CHAR   szSlowDraw[] = "Slow Draw";

/********************************************************/
 
BOOL WINAPI ScreenSaverConfigureDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hSpeed;		/* handle to speed scroll bar */ 
    static HWND hCellSize;	/* handle to cell size scroll bar */
    static HWND hOK;		/* handle to OK push button */ 
 
    switch(message) 
    { 
        case WM_INITDIALOG: 
 
            /* Retrieve the application name from the .rc file. */ 
 
            LoadString(hMainInstance, idsAppName, szAppName, sizeof(szAppName)); 
 
            /* Retrieve the .ini (or registry) file name. */ 
 
            LoadString(hMainInstance, idsIniFile, szIniFile, sizeof(szIniFile)); 
 
            /* Retrieve user setting data from the registry. */ 
 
            lSpeed = GetPrivateProfileInt(szAppName, szRedrawSpeed, DEF_SPEED, szIniFile); 
			lCellSize = GetPrivateProfileInt(szAppName, szCellSize, DEF_CELLSIZE, szIniFile); 
 			lBlackBG=GetPrivateProfileInt(szAppName, szBlackBG, DEF_BLACK_BK, szIniFile);
			lSlowDraw=GetPrivateProfileInt(szAppName, szSlowDraw, DEF_SLOW_DRAW, szIniFile);
            /* 
             * If the initialization file does not contain an entry 
             * for this screen saver, use the default value. 
             */ 
 
            if(lSpeed > MAXVEL || lSpeed < MINVEL) 
                lSpeed = DEF_SPEED; 

            if(lCellSize > MAXVEL || lCellSize < MINVEL) 
                lCellSize = DEF_CELLSIZE; 
 
            /* Initialize controls */
            hSpeed = GetDlgItem(hDlg, ID_SPEED); 
            SetScrollRange(hSpeed, SB_CTL, MINVEL, MAXVEL, FALSE); 
            SetScrollPos(hSpeed, SB_CTL, lSpeed, TRUE); 

            hCellSize = GetDlgItem(hDlg, ID_CELLSIZE); 
            SetScrollRange(hCellSize, SB_CTL, MINVEL, MAXVEL, FALSE); 
            SetScrollPos(hCellSize, SB_CTL, lCellSize, TRUE); 

			if (lBlackBG==0)
				CheckDlgButton(hDlg,  IDC_BLACKBG,  BST_UNCHECKED);
			else
 				CheckDlgButton(hDlg,  IDC_BLACKBG,  BST_CHECKED);

			if (lSlowDraw==0)
				CheckDlgButton(hDlg,  IDC_SLOWDRAW,  BST_UNCHECKED);
			else
 				CheckDlgButton(hDlg,  IDC_SLOWDRAW,  BST_CHECKED);

            /* Retrieve a handle to the OK push button control. */ 
 
            hOK = GetDlgItem(hDlg, ID_OK); 
 
            return TRUE; 
 
        case WM_HSCROLL: 
 
            /* 
             * Process scroll bar input, adjusting the lSpeed 
             * value as appropriate. 
             */ 
			if ((HWND) lParam==hSpeed)       // handle to scroll bar 
			{

				switch (LOWORD(wParam)) 
					{ 
						case SB_PAGEUP: 
							--lSpeed; 
						break; 
 
						case SB_LINEUP: 
							--lSpeed; 
						break; 
 
						case SB_PAGEDOWN: 
							++lSpeed; 
						break; 
 
						case SB_LINEDOWN: 
							++lSpeed; 
						break; 
 
						case SB_THUMBPOSITION: 
							lSpeed = HIWORD(wParam); 
						break; 
 
						case SB_BOTTOM: 
							lSpeed = MINVEL; 
						break; 
 
						case SB_TOP: 
							lSpeed = MAXVEL; 
						break; 
 
						case SB_THUMBTRACK: 
						case SB_ENDSCROLL: 
							return TRUE; 
						break; 
					} 
					if ((int) lSpeed <= MINVEL) 
						lSpeed = MINVEL; 
					if ((int) lSpeed >= MAXVEL) 
						lSpeed = MAXVEL; 
 
					SetScrollPos((HWND) lParam, SB_CTL, lSpeed, TRUE); 
				break; 
			}
			else	/* must be cell size */
			{
				switch (LOWORD(wParam)) 
					{ 
						case SB_PAGEUP: 
							--lCellSize; 
						break; 
 
						case SB_LINEUP: 
							--lCellSize; 
						break; 
 
						case SB_PAGEDOWN: 
							++lCellSize; 
						break; 
 
						case SB_LINEDOWN: 
							++lCellSize; 
						break; 
 
						case SB_THUMBPOSITION: 
							lCellSize = HIWORD(wParam); 
						break; 
 
						case SB_BOTTOM: 
							lCellSize = MINVEL; 
						break; 
 
						case SB_TOP: 
							lCellSize = MAXVEL; 
						break; 
 
						case SB_THUMBTRACK: 
						case SB_ENDSCROLL: 
							return TRUE; 
						break; 
					} 
					if ((int) lCellSize <= MINVEL) 
						lCellSize = MINVEL; 
					if ((int) lCellSize >= MAXVEL) 
						lCellSize = MAXVEL; 
 
					SetScrollPos((HWND) lParam, SB_CTL, lCellSize, TRUE); 
				break; 
			}

 
        case WM_COMMAND: 
            switch(LOWORD(wParam)) 
            { 
                case ID_OK: 
 
                   /* 
                    * Write the current redraw speed variable to 
                    * the .ini file. 
                    */ 
 
                    wsprintf(szTemp, "%ld", lSpeed); 
                    WritePrivateProfileString(szAppName, szRedrawSpeed, szTemp, szIniFile); 

                    wsprintf(szTemp, "%ld", lCellSize); 
                    WritePrivateProfileString(szAppName, szCellSize, szTemp, szIniFile); 

					if (IsDlgButtonChecked(hDlg, IDC_BLACKBG) == BST_CHECKED)
					{
						WritePrivateProfileString(szAppName, szBlackBG, "1", szIniFile); 
						lBlackBG = 1;
					}
					else 
					{
						WritePrivateProfileString(szAppName, szBlackBG, "0", szIniFile); 
						lBlackBG = 0;
					}

					if (IsDlgButtonChecked(hDlg, IDC_SLOWDRAW) == BST_CHECKED)
					{
						WritePrivateProfileString(szAppName, szSlowDraw, "1", szIniFile); 
						lSlowDraw = 1;
					}
					else 
					{
						WritePrivateProfileString(szAppName, szSlowDraw, "0", szIniFile); 
						lSlowDraw = 0;
					}

                case ID_CANCEL: 
                    EndDialog(hDlg, LOWORD(wParam) == ID_OK); 
					return TRUE; 

            } 
			break;

		case WM_CLOSE:
			EndDialog(hDlg, LOWORD(wParam) == ID_OK); 
			return TRUE; 
    } 
    return FALSE; 
} 

/********************************************************/

BOOL WINAPI RegisterDialogClasses(hInst) 
HANDLE  hInst; 
{ 
    return TRUE; 
} 

/********************************************************/

LRESULT WINAPI ScreenSaverProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
static HDC          hdc;    /* device-context handle */ 
static RECT         rc;     /* RECT structure */ 
static UINT         uTimer; /* timer identifier */ 
static int firstcall=TRUE;
static unsigned long next_cell=0;
static struct Stack stack;


    switch(message) 
    { 
        case WM_CREATE: 
 
            /* Retrieve the application name from the .rc file. */ 
 
            LoadString(hMainInstance, idsAppName, szAppName, sizeof(szAppName)); 
 
            /* Retrieve the .ini (or registry) file name. */ 
 
            LoadString(hMainInstance, idsIniFile, szIniFile, 
                sizeof(szIniFile)); 
 
            /* Retrieve user settings from the registry. */ 
 
            lSpeed    = GetPrivateProfileInt(szAppName, szRedrawSpeed, DEF_SPEED, szIniFile); 
			lCellSize = GetPrivateProfileInt(szAppName, szCellSize, DEF_CELLSIZE, szIniFile); 
 			lBlackBG  = GetPrivateProfileInt(szAppName, szBlackBG, DEF_BLACK_BK, szIniFile);
 			lSlowDraw = GetPrivateProfileInt(szAppName, szSlowDraw, DEF_SLOW_DRAW, szIniFile);
           /* 
            * Set a timer for the screen saver window using the 
            * redraw rate stored in Regedit.ini. 
            */ 
 
            uTimer = SetTimer(hwnd, 1, lSpeed * 8, NULL); 
            break; 
 
        case WM_ERASEBKGND: 
 
           /* 
            * The WM_ERASEBKGND message is issued before the 
            * WM_TIMER message, allowing the screen saver to 
            * paint the background as appropriate. 
            */ 
 
            hdc = GetDC(hwnd); 
            GetClientRect (hwnd, &rc); 
            FillRect (hdc, &rc, GetStockObject(BLACK_BRUSH)); 
            ReleaseDC(hwnd,hdc); 

            break; 
 
        case WM_TIMER: 
 
           /* 
            * Draw another Solve try each timer tick
            */ 

			if (firstcall)
			{
				firstcall=FALSE;

				CalcMaze(hwnd, lCellSize, lBlackBG, lSlowDraw);
				SolveBegin();
			}

			if (SolveStep(hwnd))
			{
				int i;
				for (i=0; i < 40; i++)		// pause a few seconds after solving maze
				{
					Sleep(100);
					if (PeekEndScreenSaver())
						break;
				}
				firstcall=TRUE;
			}
			else if (lSpeed < 3) // double speed at faster solve rates
			{
					if (SolveStep(hwnd))
				{
					int i;
					for (i=0; i < 40; i++)		// pause a few seconds after solving maze
					{
						Sleep(100);
						if (PeekEndScreenSaver())
							break;
					}
					firstcall=TRUE;
				}
			}

            break; 
 
        case WM_DESTROY: 
 
           /* 
            * When the WM_DESTROY message is issued, the screen saver 
            * must destroy any of the timers that were set at WM_CREATE 
            * time. 
            */ 
 
            if (uTimer) 
                KillTimer(hwnd, uTimer); 
            break; 
    } 
 
   /* 
    * DefScreenSaverProc processes any messages 
    * ignored by ScreenSaverProc. 
    */ 
 
    return DefScreenSaverProc(hwnd, message, wParam, lParam); 
} 

