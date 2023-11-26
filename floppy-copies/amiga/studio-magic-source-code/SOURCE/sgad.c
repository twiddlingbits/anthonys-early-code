
UBYTE SIBuffer1[80];
struct StringInfo GadgetSI1 = {
	SIBuffer1,	/* buffer where text will be edited */
	NULL,	/* optional undo buffer */
	0,	/* character position in buffer */
	80,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	NULL,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

struct Gadget strgad = {
	NULL,	/* next gadget */
	30,23,	/* origin XY of hit box relative to window TopLeft */
	101,14,	/* hit box width and height */
	GADGHCOMP,	/* gadget flags */
	NULL,	/* activation flags */
	STRGADGET,	/* gadget type */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	0,	/* gadget mutual-exclude long word */
	(APTR)&GadgetSI1,	/* SpecialInfo structure for string gadgets */
	1,	/* user-definable data (ordinal gadget number) */
	NULL	/* pointer to user-definable data */
};

/* Gadget list */

struct NewWindow NewWindowStructure = {
	275,85,	/* window XY origin relative to TopLeft of screen */
	150,50,	/* window width and height */
	0,1,	/* detail and block pens */
	NULL,	/* IDCMP flags */
	NULL,	/* other window flags */
	&strgad,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Your new window",	/* window title */
	NULL,	/* custom screen */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	640,200,	/* maximum width and height */
	WBENCHSCREEN	/* destination screen type */
};

/* end of PowerWindows source generation */
