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
extern void FreeChannelMenu();
extern void S16Text();
extern void MakeMenuesFit();
extern int * ItemData();
extern int * CheckItem();
extern int * CheckNum();
extern int * CheckData();
extern int MenuCheckSum();
extern int ItemNum();
extern struct MenuItem * BuildChannelMenu();
extern struct MenuItem * FindCheckedItem();
extern struct MenuItem * BuildHandlerMenu();
extern void FreeHandlerMenu();
extern void show_error();
extern void newgetstr();
extern struct MenuItem * BuildProjectMenu();
extern struct MenuItem * BuildApplicationMenu();
extern void FreeProjectMenu();
extern void FreeApplicationMenu();
extern void ParseStringForIllegalChars();
