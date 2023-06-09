; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTRS80EmuApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TRS80Emu.h"
LastPage=0

ClassCount=4
Class1=CTRS80EmuApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=2
Resource1=IDR_MAINFRAME
Class2=CChildView
Resource2=IDD_ABOUTBOX

[CLS:CTRS80EmuApp]
Type=0
HeaderFile=TRS80Emu.h
ImplementationFile=TRS80Emu.cpp
Filter=N
LastObject=ID_APP_RUN3
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=C
LastObject=CChildView
BaseClass=CWnd 
VirtualFilter=WC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=TRS80Emu.cpp
ImplementationFile=TRS80Emu.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308480

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_RUN1
Command2=ID_APP_RUN2
Command3=ID_APP_RUN3
Command4=ID_APP_RUN4
Command5=ID_APP_EXIT
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

