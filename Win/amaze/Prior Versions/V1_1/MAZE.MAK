# Microsoft Visual C++ generated build script - Do not modify

PROJ = MAZE
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = 
R_RCDEFINES = 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\SOURCE\WINMAZE\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = GENERIC.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /Zp1 /W3 /Zi /AL /YX /Od /D "_DEBUG" /FR /GA /Fd"MAZE.PDB" /Fp"MAZE.PCH"
CFLAGS_R_WEXE = /nologo /Gs /G2 /Zp1 /W3 /AS /YX /Ox /D "NDEBUG" /FR /GA /Fp"MAZE.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw llibcew LIBW commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw slibcew LIBW commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /NOLOGO  
RESFLAGS = /NOLOGO  -t
RUNFLAGS = 
DEFFILE = GENERIC.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = GENERIC.SBR \
		MAZE.SBR


GENERIC_DEP = d:\source\winmaze\generic.h \
	d:\source\winmaze\maze.h


MAZE_DEP = d:\source\winmaze\maze.h


all:	$(PROJ).EXE $(PROJ).BSC

GENERIC.OBJ:	GENERIC.C $(GENERIC_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c GENERIC.C

GENERIC.RES:	GENERIC.RC $(GENERIC_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r GENERIC.RC

MAZE.OBJ:	MAZE.C $(MAZE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MAZE.C


$(PROJ).EXE::	GENERIC.RES

$(PROJ).EXE::	GENERIC.OBJ MAZE.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
GENERIC.OBJ +
MAZE.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) GENERIC.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	GENERIC.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) GENERIC.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
