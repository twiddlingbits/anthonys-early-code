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

static struct NewWindow MIX_NewWindow = {
   (640-548)>>1,
   15,
   250,
   100,
   BLACK,
   WHITE,
   CLOSEWINDOW|NEWSIZE,
   REPORTMOUSE|ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|WINDOWSIZING,
   0,0,
   " Mixer ", /** TITLE **/
   0,0,
   100,50,640,400,
   CUSTOMSCREEN
   };

/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
static void MIX0func();
static struct NewSlider MIX0gad=
	{NULL,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	MIX0func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	151,1,50,				/* list_size | show_size | top */
	-1,11};				/* knob_width | knob_Height */
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
static void MIX1func();
static struct NewSlider MIX1gad=
	{&MIX0gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	MIX1func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	151,1,50,					/* list_size | show_size | top */
	-1,11};				/* knob_width | knob_Height */
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
static void MIX2func();
static struct NewSlider MIX2gad=
	{&MIX1gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	MIX2func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	151,1,50,					/* list_size | show_size | top */
	-1,11};				/* knob_width | knob_Height */
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
static void MIX3func();
static struct NewSlider MIX3gad=
	{&MIX2gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	MIX3func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	151,1,50,					/* list_size | show_size | top */
	-1,11};				/* knob_width | knob_Height */
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
static void MIXmonfunc();
static struct NewSlider MIXmongad=
	{&MIX3gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	MIXmonfunc,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* *gad_ptr */
	151,1,50,					/* list_size | show_size | top */
	-1,11};				/* knob_width | knob_Height */
/*風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風風�*/
