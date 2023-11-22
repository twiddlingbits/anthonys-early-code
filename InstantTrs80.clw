; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CLibraryDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TRS80Emu.h"
LastPage=0

ClassCount=5
Class1=CMainApp
Class3=CMainFrame
Class4=CWinTrs80Thread

ResourceCount=3
Resource1=IDR_MAINFRAME
Class2=CChildView
Resource2=IDD_ABOUTBOX
Class5=CLibraryDlg
Resource3=IDD_LIBRARY

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
BaseClass=CFrameWnd
VirtualFilter=fWC




[DLG:IDD_ABOUTBOX]
Type=1
Class=?
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
Command5=ID_APP_VIEW_LIB
Command6=ID_APP_EXIT
Command7=ID_APP_ABOUT
CommandCount=7

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

[CLS:CWinTrs80Thread]
Type=0
HeaderFile=wintrs80thread.h
ImplementationFile=wintrs80thread.cpp
BaseClass=CWinThread
LastObject=CWinTrs80Thread

[CLS:CLibraryDlg]
Type=0
HeaderFile=LibraryDlg.h
ImplementationFile=LibraryDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_TREE1
VirtualFilter=dWC

[DLG:IDD_LIBRARY]
Type=1
Class=CLibraryDlg
ControlCount=1
Control1=IDC_TREE1,SysTreeView32,1350631431

[CLS:CMainApp]
Type=0
HeaderFile=mainapp.h
ImplementationFile=mainapp.cpp
BaseClass=CWinApp
LastObject=CMainApp
Filter=N
VirtualFilter=AC

