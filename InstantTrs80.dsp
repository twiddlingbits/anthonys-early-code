# Microsoft Developer Studio Project File - Name="TRS80Emu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TRS80Emu - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "InstantTrs80.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "InstantTrs80.mak" CFG="TRS80Emu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TRS80Emu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TRS80Emu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TRS80Emu - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/AWSoftware.exe"

!ELSEIF  "$(CFG)" == "TRS80Emu - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/AWSoftware.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TRS80Emu - Win32 Release"
# Name "TRS80Emu - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\DataAsteroid.cpp
# End Source File
# Begin Source File

SOURCE=.\DataGermany.cpp
# End Source File
# Begin Source File

SOURCE=.\DataLdosDsk.cpp
# End Source File
# Begin Source File

SOURCE=.\DataLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\DataModIRom.cpp
# End Source File
# Begin Source File

SOURCE=.\DataNewDos80Dsk.cpp
# End Source File
# Begin Source File

SOURCE=.\DataQuest.cpp
# End Source File
# Begin Source File

SOURCE=.\DataTime.cpp
# End Source File
# Begin Source File

SOURCE=.\LibraryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainApp.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Trs80Configurator.cpp
# End Source File
# Begin Source File

SOURCE=.\Trs80Emulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Trs80Fdd.cpp
# End Source File
# Begin Source File

SOURCE=.\WinTrs80.cpp
# End Source File
# Begin Source File

SOURCE=.\WinTrs80Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\Z80C.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\LibraryDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainApp.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Trs80Configurator.h
# End Source File
# Begin Source File

SOURCE=.\Trs80Emulator.h
# End Source File
# Begin Source File

SOURCE=.\Trs80Fdd.h
# End Source File
# Begin Source File

SOURCE=.\WinTrs80.h
# End Source File
# Begin Source File

SOURCE=.\WinTrs80Thread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\InstantTrs80.rc
# End Source File
# Begin Source File

SOURCE=.\res\TRS80Emu.ico
# End Source File
# Begin Source File

SOURCE=.\res\TRS80Emu.rc2
# End Source File
# End Group
# Begin Group "Z80"

# PROP Default_Filter ".c, .h"
# Begin Source File

SOURCE=.\Z80\Codes.h
# End Source File
# Begin Source File

SOURCE=.\Z80\CodesCB.h
# End Source File
# Begin Source File

SOURCE=.\Z80\CodesED.h
# End Source File
# Begin Source File

SOURCE=.\Z80\CodesXCB.h
# End Source File
# Begin Source File

SOURCE=.\Z80\CodesXX.h
# End Source File
# Begin Source File

SOURCE=.\Z80\Debug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Z80\Tables.h
# End Source File
# Begin Source File

SOURCE=.\Z80\Z80.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Z80\Z80.h
# End Source File
# End Group
# End Target
# End Project
