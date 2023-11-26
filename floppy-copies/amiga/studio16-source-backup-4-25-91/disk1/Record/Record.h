#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,LIGHT_BROWN,WHITE,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BLUE,BLACK,BLACK}
#define DROPCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,WHITE,BLACK}
static struct GadgetColors colors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	DROPCOLORS};


#define Wth 390
static struct NewWindow Record_NewWindow = {
   (640-548)>>1,
   15,
   Wth,
   65,
   BLACK,
   WHITE,
   CLOSEWINDOW,
   REPORTMOUSE|ACTIVATE|SMART_REFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   " Record Window ", /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static void GAINfunc();
static struct NewSlider GAINgad=
	{NULL,					/* *next (gadget) */
	10,15,(Wth>>1)-4,27,				/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ,			/* Flags */
	GAINfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	16,1,14,				/* list_size | show_size | top */
	8*7+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct slider *FREQknob;
static void FREQfunc();
static struct NewSlider FREQgad=
	{&GAINgad,					/* *next (gadget) */
	(Wth>>1)+4,15,379,27,				/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ,			/* Flags */
	FREQfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	28,1,26,				/* list_size | show_size | top */
	8*12+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct slider *RATEknob;
static void RATEfunc();
static struct NewSlider RATEgad=
	{&FREQgad,					/* *next (gadget) */
	10,31,379,43,				/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ,			/* Flags */
	RATEfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	600,1,550,					/* list_size | show_size | top */
	8*10+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct Button *FILEbut ;  
static void FILEfunc();
static struct NewButton FILEbutton = 
	{&RATEgad,				/* *next (gadget) */
	299,47,383,47+12,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE,				/* Flags */
	FILEfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	"Name"};					/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static void RECORDfunc();
static struct NewButton RECORDbutton = 
	{&FILEbutton,				/* *next (gadget) */
	208,47,292,47+12,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE,				/* Flags */
	RECORDfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	"Record"};					/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static void AUTOFILTfunc();
static struct NewButton AUTOFILTbutton = 
	{&RECORDbutton,				/* *next (gadget) */
	97,47,201,47+12,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE,				/* Flags */
	AUTOFILTfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	"Auto Filter"};					/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static void MONITORfunc();
static struct NewButton MONITORbutton = 
	{&AUTOFILTbutton,				/* *next (gadget) */
	10,47,90,47+12,					/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE,				/* Flags */
	MONITORfunc,					/* Call Function */
	NULL,						/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	"MONITOR"};					/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
