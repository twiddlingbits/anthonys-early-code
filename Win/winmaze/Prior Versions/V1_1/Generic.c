#include "windows.h"            /* required for all Windows applications */
#include "generic.h"            /* specific to this program          */
#include "maze.h"
#include <stdio.h>
#include <string.h>

HINSTANCE hInst;               /* current instance              */

#define SS_INIT 0
#define SS_SIZING 1
#define SS_IDLE 2


/***********************************************************************/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;                     /* message              */

    if (!hPrevInstance)          /* Other instances of app running? */
    if (!InitApplication(hInstance)) /* Initialize shared things */
        return (FALSE);      /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg,    /* message structure              */
        NULL,          /* handle of window receiving the message */
        0,          /* lowest message to examine          */
        0))         /* highest message to examine         */
    {
    TranslateMessage(&msg);    /* Translates virtual key codes       */
    DispatchMessage(&msg);     /* Dispatches message to window       */
    }
    return (msg.wParam);       /* Returns the value from PostQuitMessage */
}


/*************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;                  /* current instance       */
{
    WNDCLASS  wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = 0;                    /* Class style(s).                    */
    wc.lpfnWndProc = MainWndProc;       /* Function to retrieve messages for  */
                                        /* windows of this class.             */
    wc.cbClsExtra = 0;                  /* No per-class extra data.           */
    wc.cbWndExtra = 0;                  /* No per-window extra data.          */
    wc.hInstance = hInstance;           /* Application that owns the class.   */
    wc.hIcon = LoadIcon(hInstance, "MazeIcon");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =  "MazeMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "MazeWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/***********************************************************/


BOOL InitInstance(hInstance, nCmdShow)
    HINSTANCE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
        "MazeWClass",                /* See RegisterClass() call.          */
        "Maze",						   /* Text for window title bar.         */
        WS_OVERLAPPEDWINDOW,            /* Window style.                      */
        CW_USEDEFAULT,                  /* Default horizontal position.       */
        CW_USEDEFAULT,                  /* Default vertical position.         */
        200,             			    /* Default width.                     */
        250,              			    /* Default height.                    */
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        hInstance,                      /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/***************************************************************/


long CALLBACK MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                      /* window handle                 */
UINT message;                   /* type of message               */
WPARAM wParam;                  /* additional information        */
LPARAM lParam;                  /* additional information        */
{
PAINTSTRUCT ps;
static BOOL size_state = SS_INIT;

    switch (message)
    {
        case WM_COMMAND:       /* message: command from application menu */
        	switch(wParam)
        	{
        		case IDM_NEW:
	        		EnableWindow (hWnd, FALSE);
    				CalcMaze(hWnd);
           			SolveMaze(hWnd);
	        		EnableWindow (hWnd, TRUE);
        			break;
        		
        		case IDM_PRINT:
        			PrintMaze(hWnd);
        			break;
        				
        		case IDM_EXIT:
        		   	DestroyWindow(hWnd);
		   			break;
        				
            	case IDM_ABOUT:
	                DialogBox(hInst,        /* current instance          */
    	                "AboutBox",         /* resource to use           */
        	            hWnd,               /* parent handle             */
            	        About);             /* About() instance address  */
                    break;
                    
				case IDM_OPTIONS:
		            DialogBox(hInst,        /* current instance          */
        	            (LPCTSTR )"MazeOptions",      /* resource to use           */
            	        hWnd,               /* parent handle             */
                	    Options);           /* About() instance address  */
                	break;
                	
    		    default:             	    /* Passes it on if unproccessed    */
	                return (DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;
                
		case WM_SIZE:  
   			if (size_state==SS_INIT)
			{
				EnableWindow (hWnd, FALSE);
				CalcMaze(hWnd);
				SolveMaze(hWnd);
   				EnableWindow (hWnd, TRUE);
			}
			size_state = SS_SIZING;
			break;

		case WM_EXITSIZEMOVE:
   			if (size_state==SS_SIZING)
			{
				EnableWindow (hWnd, FALSE);
				CalcMaze(hWnd);
				SolveMaze(hWnd);
   				EnableWindow (hWnd, TRUE);
				size_state = SS_IDLE;
			}
            break;

		case WM_PAINT:
    		BeginPaint(hWnd, &ps);
    		PaintMaze(hWnd, &ps);
    		EndPaint(hWnd, &ps);
    		break;

        case WM_DESTROY:          /* message: window being destroyed */
            PostQuitMessage(0);
            break;

        default:                  /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

    WM_INITDIALOG - initialize dialog box
    WM_COMMAND    - Input received

    COMMENTS:

    No initialization is needed for this particular dialog box, but TRUE
    must be returned to Windows.

    Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL CALLBACK About(hDlg, message, wParam, lParam)
HWND hDlg;               /* window handle of the dialog box */
UINT message;        /* type of message                 */
WPARAM wParam;             /* message-specific information    */
LPARAM lParam;
{
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:               /* message: received a command */
            if (wParam == IDOK         /* "OK" box selected?          */
                || wParam == IDCANCEL) /* System menu close command?  */
            {
                EndDialog(hDlg, TRUE); /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);               /* Didn't process a message    */
}

BOOL CALLBACK Options(hDlg, message, wParam, lParam)
HWND hDlg;               /* window handle of the dialog box */
UINT message;        /* type of message                 */
WPARAM wParam;             /* message-specific information    */
LPARAM lParam;
{
extern int solve_speed;
HWND hWnd;
HWND hSpeed, hX, hY;
WORD nPos,id;
HDC hdc;
RECT rectClient;
char string[80];

    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */ 
        	hSpeed=GetDlgItem(hDlg, IDC_SPEED);
        	SetScrollRange(hSpeed, SB_CTL, 1, 500, FALSE);
           	SetScrollPos(hSpeed, SB_CTL, solve_speed, FALSE);

        	hX=GetDlgItem(hDlg, IDC_HCELLSIZE);
        	SetScrollRange(hX, SB_CTL, 3, 20, FALSE);
           	SetScrollPos(hX, SB_CTL, 5, FALSE);

        	hY=GetDlgItem(hDlg, IDC_VCELLSIZE);
        	SetScrollRange(hY, SB_CTL, 3, 20, FALSE);
           	SetScrollPos(hY, SB_CTL, 5, FALSE);

            return(TRUE);
            
        case WM_COMMAND:               /* message: received a command */
            if (wParam == IDOK         /* "OK" box selected?          */
                || wParam == IDCANCEL) /* System menu close command?  */
            {
                EndDialog(hDlg, TRUE); /* Exits the dialog box        */
                return (TRUE);
            }
            break;
            
        case WM_HSCROLL:
        	if (wParam==SB_THUMBPOSITION || wParam==SB_THUMBTRACK)
        	{
				nPos = LOWORD(lParam);     		/* current position of scroll box */
				hWnd = (HWND) HIWORD(lParam); 	/* handle of the control    */
            	SetScrollPos(hWnd, SB_CTL, nPos, TRUE);
            	
           		hdc = GetDC(hWnd);
                sprintf(string,"%03d ",nPos);
           		GetClientRect (hWnd, &rectClient);
           		TextOut(hdc, rectClient.right+10, rectClient.bottom-23,
           				string, strlen(string));
				ReleaseDC(hWnd, hdc);
	           	
	           	id=GetDlgCtrlID(hWnd);
            	if (id==IDC_SPEED)
            		solve_speed=nPos;
 
	           	return(TRUE);
            }

    }
    return (FALSE);               /* Didn't process a message    */
}