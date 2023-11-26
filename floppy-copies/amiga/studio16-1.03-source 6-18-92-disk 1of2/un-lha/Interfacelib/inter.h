struct	InterfaceBase	*InterfaceBase = 0L;

BOOL OpenInterfaceBase()
{
	InterfaceBase=(struct InterfaceBase *)OpenLibrary("Interface.library",1L);
	if(InterfaceBase) {
	 return(TRUE);
	 }
	else {
	 return(FALSE);
	 }
}

void CloseInterfaceBase()
{
	if(InterfaceBase) CloseLibrary(InterfaceBase);
	InterfaceBase=0L;
}

extern struct window *  OpenNewWind();
extern struct window *  CloseWind();
extern void ColorGadList();
extern void HandleIntuitMess();
extern void RefreshWind();
extern void RefreshBar();
extern void WindTitleChange();
extern char * FindFileName();
extern char * FileRequest();
