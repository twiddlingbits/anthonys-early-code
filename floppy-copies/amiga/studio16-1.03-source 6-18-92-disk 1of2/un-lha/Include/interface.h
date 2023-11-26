#ifndef INTERFACE_H
#define INTERFACE_H TRUE

/** COPYRIGHT 1992 SunRize Industries **/

/* Macros */
#define LockMod(a)    Forbid();((struct StandardModState *)(a))->lock++;Permit()
#define UnlockMod(a)    Forbid();((struct StandardModState *)(a))->lock--;Permit()

#define PREFERENCES	(1<<0)	/* Flags2 Flags */
#define AUTOCLOSE	(1<<1)
#define DONTMOVEWINDOW	(1<<2)
#define HIDEMESSAGE	(1<<3)
#define NOMESSAGE	(1<<4)
#define TE_NEGONE	(1<<5)
#define LE_NEGONE	(1<<6)
#define IL_NOSIMPLE	(1<<7)

/*#define WINDOWACTIVE  0x2000*/
#define LAST_WINDOWACTIVE 0x2000

struct WindowParms
	{
	LONG	Flags;
	LONG	Flags2;
	UBYTE *Title;
	struct NewWindow *NWind;
	struct Gadget SizeGad;
	struct NewButton DSizeButton;
	struct Gadget DragGad;
	struct NewButton CloseButton;
	struct NewButton DepthButton;
	struct NewButton PrefsButton;
	struct MenuHead *Menu;
	USHORT LE,TE,H,W;
	struct StandardModState *state;
	struct HitAreaColor *ActiveClrs;
	struct HitAreaColor *InactiveClrs;
	struct NewButton *menugads;
	struct MenuHead *curhead;
	struct Window *mwind;
	};

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
#define MI_SELECTED (1<<0)
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct MenuItm {
	char *text;
	struct MenuItm *next;
	struct MenuItm *sub;
	int flags;
	void (*func)();
	};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct MenuHead {
	char *text;
	struct MenuHead *next;
	struct MenuItm *list;
	int flags;
	struct Window *wind;
	struct NewButton *butt;
	};

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
#define FR_SINGLE_SELECT (1<<0)
#define FR_MULTI_SELECT (1<<1)
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
#endif
