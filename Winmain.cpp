//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <windows.h>    
#include <commdlg.h>
#include <stdio.h>
#include "resource.h"   
#include "basic.h"        

#define WIN_X_OFFSET 10
#define WIN_Y_OFFSET 10

/* MODULE GLOBALS */
static BasicCode* the_process;
static bool break_flag = FALSE;
static HWND ghWnd;               /* Main window handle. */
static HINSTANCE hInst;				
static MSG msg;
static HFONT my_font;
static bool lower_case_mod_installed = TRUE;
static long my_cell_w1;  
static long my_cell_w2;  
static long my_cell_h1;;
static long my_cell_h2;;
static long my_cell_h3;;
static HBRUSH my_color;
static long my_cx;
static long my_cy;


/* MODULE PROTOTYPES */
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance,int nCmdShow);
long CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DrawTrs80Graphic(HDC dc, unsigned short offset, unsigned char val);
void Trs80DrawChar(HDC dc, unsigned short offset, unsigned char value);
void DoOpenFile(HWND hWnd, char* filepath);

/***********************************************************************/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	struct IoConsole *debug_con, *primary_con;
	HANDLE hin;
	long k;
	FILE* fp;

    if (!hPrevInstance)					/* Other instances of app running? */
	  if (!InitApplication(hInstance))	/* Initialize shared things */
        return (FALSE);					/* Exits if unable to initialize     */

  	primary_con=IoWinWinOpen();
	debug_con=NULL;

	the_process = BasicCreateProcess(primary_con, debug_con);
	if (the_process==NULL)
		return FALSE;	

	if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

	if (fp=fopen("autorun.bas", "r"))
	{
		fclose(fp);
		DoOpenFile(ghWnd, "autorun.bas");
	}

	hin=GetStdHandle(STD_INPUT_HANDLE);
	if (INVALID_HANDLE_VALUE==hin || the_process->debug_con==NULL)
		k=0;
	else
		k=1;

	/*
	 * Main message loop.  Note the covoluted way this works:
	 *   - when a RUN is executed (e.g. File | Open, File | Run), this loop will block
	 *     until the executed code exits.
	 *   - However, when a BASIC program is running, it is either calling 
	 *     io.io_getc (WinGetC), or io.io_chk_brk (WinCheckForBreak).  Both of these
	 *     functions also run the message loop.   This means that MainWndProc can be called
	 *     re-entrently.  It handles this.
	 */

	while (TRUE)
	{
		MsgWaitForMultipleObjects(k,&hin,FALSE,INFINITE,QS_ALLEVENTS);

		DebugTimeSlice(the_process);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))							
		{
			if (msg.message==WM_QUIT)
				break;

			TranslateMessage(&msg);			/* Translates virtual key codes       */
			DispatchMessage(&msg);			/* Dispatches message to window       */

			if (msg.message==WM_CHAR)
				WinOnChar((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 

			else if (msg.message==WM_KEYDOWN)
				WinOnKeyDown((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 

			else if (msg.message==WM_KEYUP)
				WinOnKeyUp((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 			
		}

		hin=GetStdHandle(STD_INPUT_HANDLE);		/* debug window status could have changed */
		if (INVALID_HANDLE_VALUE==hin || the_process->debug_con==NULL)
			k=0;
		else
			k=1;
	}

	BasicDestroyProcess(the_process);	/* Free any process resources */
    return (msg.wParam);				/* Returns the value from PostQuitMessage */
}

/***********************************************************************/

int WinCheckForBreak(struct IoConsole* io)
{
	DebugTimeSlice(the_process);				/* If debug window is open, handle it's events */

	msg.message = 0;
    /* Acquire and dispatch messages until a WM_QUIT message is received. */

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))							
	{
		if (msg.message==WM_QUIT)
			break;

		TranslateMessage(&msg);			/* Translates virtual key codes       */
		DispatchMessage(&msg);			/* Dispatches message to window       */
		
		if (msg.message==WM_CHAR)
		{
			if ((TCHAR) msg.wParam==0x1b)		// ESC key
				break_flag = TRUE;
			WinOnChar((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 
		}

		else if (msg.message==WM_KEYDOWN)
			WinOnKeyDown((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 

		else if (msg.message==WM_KEYUP)
			WinOnKeyUp((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 

		if (break_flag)	// Handle the case where a message process set the break flag
			break;

	}

	if (msg.message==WM_QUIT)
		return ERR_WINDOWCLOSED;
	
	if (break_flag)
	{
		break_flag=FALSE;
		return ERR_BREAK;
	}

	else
		return ERR_OKAY;		
}

/***********************************************************************/

char WinGetC(struct IoConsole* io)
{
	HANDLE hin;
	long k;

	hin=GetStdHandle(STD_INPUT_HANDLE);

	if (INVALID_HANDLE_VALUE==hin)
		k=0;
	else
		k=1;


	while (TRUE)
	{
		MsgWaitForMultipleObjects(k,&hin,FALSE,INFINITE,QS_ALLEVENTS);

		DebugTimeSlice(the_process);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))							
		{
			if (msg.message==WM_QUIT)
				break;

			TranslateMessage(&msg);			/* Translates virtual key codes       */
			DispatchMessage(&msg);			/* Dispatches message to window       */
			
			if (msg.message==WM_CHAR)
			{
				WinOnChar((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 
				WinInkey(NULL);	// clear the inkey buffer which is also set by WinOnChar.
							// we want the keyboard emulator to know the key is down (for example, incase a peek
							// to the keyboard follows.  But GetC does not leave a char in the inkey buffer.
			
				return (TCHAR) msg.wParam;    // character code 
			}

			else if (msg.message==WM_KEYDOWN)
				WinOnKeyDown((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16); 

			else if (msg.message==WM_KEYUP)
				WinOnKeyUp((int) msg.wParam, msg.lParam&0xFF, msg.lParam>>16);

			if (break_flag)
			{
				break_flag=FALSE;
				return 0x1b;		// esc key
			}

		}
	}

	/** WM_QUIT must have been recieved **/
	 PostQuitMessage(0);	/* put it back*/
	 return 0x1b;	
}
/*************************************************************************/

BOOL InitApplication(HINSTANCE hInstance)
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
    wc.hIcon = LoadIcon(hInstance, "MainIcon");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); /*GetStockObject(WHITE_BRUSH);*/
    wc.lpszMenuName =  "BasicMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "TrsBasicWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/***********************************************************/


BOOL InitInstance(HINSTANCE hInstance,int nCmdShow)
{
	HFONT oldfont;

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    /* Create a main window for this application instance.  */

    ghWnd = CreateWindow(
        "TrsBasicWClass",               /* See RegisterClass() call.          */
        "awBASIC (TRS-80 Compatible)",  /* Text for window title bar.         */
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

    if (!ghWnd)
        return (FALSE);

	/* Accept drag & drop files in client window */
	DragAcceptFiles(ghWnd, TRUE);

	/* create and set the font we want */

	if (NULL==(my_font = CreateFont(21, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 
						FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
						CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH|TMPF_TRUETYPE, 
						"courier new")))
		return FALSE;


	HDC dc;
	
	dc=GetDC(ghWnd);
	if (dc==NULL)
		return FALSE;

	oldfont = (HFONT)SelectObject(dc, my_font);
	if (!oldfont)
		return FALSE;

	/* Use font size to determine char widht & heights, and window width & height */
	/* TRS-80 window is 64 chars wide by 16 chars high */

	TEXTMETRIC tm;
	if (!GetTextMetrics(dc, &tm))
		return FALSE;

	my_cx = tm.tmAveCharWidth;
	my_cy = tm.tmHeight;

	// Calc each cell seperately to avoid rounding errors
	my_cell_w1 = my_cx / 2;  
	my_cell_w2 = my_cx - my_cell_w1;  
	my_cell_h1 = my_cy / 3;
	my_cell_h2 = my_cy / 3;
	my_cell_h3 = my_cy - my_cell_h1 - my_cell_h2;
	my_color = CreateSolidBrush(GetTextColor(dc));


	RECT rect;

	GetWindowRect(ghWnd, &rect);
	SetRect(&rect, rect.left, rect.top, rect.left+my_cx*64+WIN_X_OFFSET*2, rect.top+my_cy*16+WIN_Y_OFFSET*2);
	AdjustWindowRectEx(&rect, GetWindowLong(ghWnd, GWL_STYLE), TRUE, GetWindowLong(ghWnd, GWL_EXSTYLE));
	MoveWindow(ghWnd, rect.left,  rect.top, rect.right-rect.left+1, rect.bottom-rect.top+1,  FALSE); 

	ReleaseDC(ghWnd, dc);


    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(ghWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(ghWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/***************************************************************/

void DoOpenFile(HWND hWnd, char* filepath)
{
	int err;

	if (!BasicLoadFile(the_process, filepath))
	{
		MessageBox(hWnd, "Unable to load file", 0, MB_OK);
	}
	else
	{
		EnableMenuItem(GetMenu(hWnd), IDM_RUN, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_SAVEAS, MF_ENABLED);
		
		err=BasicRun(the_process);

		if (err==ERR_WINDOWCLOSED)
			PostQuitMessage(0);
	}
}

bool WinGetFileName(char* fname, int size_fname, int mode)
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.lpstrFile=fname;
	fname[0]=0;
	ofn.nMaxFile=size_fname;
	ofn.hwndOwner = ghWnd;
	if (mode==0)
		return 0!=GetSaveFileName(&ofn);
	else
		return 0!=GetOpenFileName(&ofn);
}

bool WinLprintMessage()
{
	return IDYES==MessageBox(ghWnd, "This application is printing to a lineprinter (LPRINT).  \nDo you want the output sent to a file? (selecting NO will ingore LPRINTs)", "LPRINT", MB_YESNO);
}

/***************************************************************/


long CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDROP hDropInfo;
	char fname[400];

    switch (message)
    {
        case WM_COMMAND:       /* message: command from application menu */
        	switch(LOWORD(wParam))
        	{
        		case IDM_OPEN:
					if (the_process->run_mode==RM_IDLE)
					{
						if (WinGetFileName(fname, sizeof(fname), 1))
							DoOpenFile(hWnd, fname);
					}
					else
					{
						/* If currently running, break execution, and repost menue message so above code will execute */
						if (break_flag==TRUE)
						{ // detect case where task not broken
							MessageBox(NULL,"Internal Error",0,0);
							break_flag=FALSE;
						}
						else
						{
							break_flag = TRUE;
							PostMessage(hWnd, WM_COMMAND, MAKELONG(IDM_OPEN, 1), lParam);
						}
					}
        			break;

        		case IDM_RUN:
					if (the_process->run_mode==RM_IDLE)
					{
						BasicSetDefaultState(the_process);
						BasicRun(the_process);
					}
					else
					{
						/* If currently running, break execution, and repost menue message so above code will execute */
						if (break_flag==TRUE)
						{ // detect case where task not broken
							MessageBox(NULL,"Internal Error",0,0);
							break_flag=FALSE;
						}
						else
						{
							break_flag = TRUE;
							PostMessage(hWnd, WM_COMMAND, MAKELONG(IDM_RUN, 1), lParam);
						}
					}
        			break;
        		
        		case IDM_SAVEAS:
					if (WinGetFileName(fname, sizeof(fname), 0))
					{
						if (!BasicSaveFile(the_process, fname))
							MessageBox(NULL,"Unable to save to file.",0,0);
					}

        			break;

        		case IDM_FILE_DEBUGCONSOLE:
					if (the_process->debug_con)
					{
						io_close(the_process->debug_con);
						DebugAttachConsole(the_process, NULL);
						CheckMenuItem(GetMenu(hWnd), IDM_FILE_DEBUGCONSOLE, MF_UNCHECKED);
					}

					else
					{
						DebugAttachConsole(the_process, IoWinConOpen());
						CheckMenuItem(GetMenu(hWnd), IDM_FILE_DEBUGCONSOLE, MF_CHECKED);
					}
        			break;
        				
        		case IDM_EXIT:
        		   	DestroyWindow(hWnd);
		   			break;
        				
            	case IDM_ABOUT:
	                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);            
                    break;
                	
    		    default:             	    /* Passes it on if unproccessed    */
	                return (DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;
                

		case WM_PAINT:
			hdc=BeginPaint(hWnd, &ps);

			SelectObject(hdc, my_font);
		
			io_draw_range(the_process->std_con, 0, the_process->std_con->io_width*the_process->std_con->io_height-1);

	   		EndPaint(hWnd, &ps);
    		break;

        case WM_DESTROY:          /* message: window being destroyed */
            PostQuitMessage(0);
            break;

		case WM_DROPFILES:
			if (the_process->run_mode==RM_IDLE)
			{
				hDropInfo = (HDROP)wParam;
				// should detect and warn if multiple files dropped in future...
				DragQueryFile(hDropInfo, 0, fname, sizeof(fname));
				DragFinish(hDropInfo);
				DoOpenFile(hWnd, fname);
			}
			else
			{
				/* If currently running, break execution, and repost menue message so above code will execute */
				if (break_flag==TRUE)
				{ // detect case where task not broken
					MessageBox(NULL,"Internal Error",0,0);
					break_flag=FALSE;
				}
				else
				{
					break_flag = TRUE;
					PostMessage(hWnd, WM_DROPFILES, wParam, lParam);
				}
			}

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

BOOL CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

//*************************************************

void WinDrawRange(struct IoConsole* io, unsigned char* vm, int start, int end)
{
	
	HDC dc;

	dc=GetDC(ghWnd);

	if (dc)
	{
		SelectObject(dc, my_font);

		for (int i=start; i <= end; i++)
			Trs80DrawChar(dc, i, vm[i]);

		ReleaseDC(ghWnd, dc);
	}

	/*InvalidateRect(ghWnd, NULL, FALSE);*/
}

//**************************************************


void Trs80DrawChar(HDC dc, unsigned short offset, unsigned char value)
{
	if (value&128 || value==32)
	{
		DrawTrs80Graphic(dc, offset, value);
	}
	else
	{
		int x, y;

		x = WIN_X_OFFSET + (offset%64)*my_cx;
		y = WIN_X_OFFSET + (offset/64)*my_cy;

		if (!lower_case_mod_installed)
		{
			if ((value & 32) == 0)	// When No lowercase mod installed
				value |= 64;		// BIT6 = NOT (BIT5 OR BIT7)
			else // this part not needed -- why?
				value &= (~64);		// BIT6 = NOT (BIT5 OR BIT7)
		}
		TextOut(dc, x, y, (char *)&value, 1);
	}
}

//**************************************************

void FillSolidRect(HDC dc, int x, int y, int cx, int cy, HBRUSH clr)
{
	RECT r;

	r.left=x;
	r.top=y;
	r.right=x+cx;
	r.bottom=y+cy;

	FillRect(dc, &r, clr);
}

//**************************************************

void DrawTrs80Graphic(HDC dc, unsigned short offset, unsigned char val)
{
	int x, y;

	x = WIN_X_OFFSET + (offset%64)*my_cx;
	y = WIN_X_OFFSET + (offset/64)*my_cy;

	FillSolidRect(dc, x, y, my_cx, my_cy, (HBRUSH)(COLOR_WINDOW+1));

	if (val == 32)
		return;

	if (val&1)
		FillSolidRect(dc, x, y, my_cell_w1, my_cell_h1, my_color);

	y=y+my_cell_h1;

	if (val&4)
		FillSolidRect(dc, x, y, my_cell_w1, my_cell_h2, my_color);

	y=y+my_cell_h2;

	if (val&16)
		FillSolidRect(dc, x, y, my_cell_w1, my_cell_h3, my_color);

	x=x+my_cell_w1;

	if (val&32)
		FillSolidRect(dc, x, y, my_cell_w2, my_cell_h3, my_color);

	y=y-my_cell_h2;

	if (val&8)
		FillSolidRect(dc, x, y, my_cell_w2, my_cell_h2, my_color);

	y=y-my_cell_h1;

	if (val&2)
		FillSolidRect(dc, x, y, my_cell_w2, my_cell_h1, my_color);

}

//**************************************************
/* simple trs-80 speed simulation.  10ms delay for each instruction*/

void WinTrsDelay(void)
{
	static short k=0;

	if (k++>5)
	{
		Sleep(10);
		k=0;
	}
}

//*************************************************

struct IoConsole* IoWinWinOpen()
{
	static struct IoConsole io;
	static unsigned char video_mem[1024];

	for (short i=0; i < 1024; i++)
		video_mem[i]=' ';

	io.io_inkey		= WinInkey;
	io.io_putc		= NULL;			// Use default implementation
	io.io_getc		= WinGetC;
	io.io_chk_brk	= WinCheckForBreak;
	io.io_close		= NULL;			// don't call any close

	io.io_draw_range= WinDrawRange;
	io.io_peek_keyboard=WinTrsPeekKeyboard;

	io.io_width = 64;
	io.io_height = 16;
	
	io.cursor_visable = FALSE;
	io.cursor=0;
	io.video_mem = video_mem;

	return &io;
}


