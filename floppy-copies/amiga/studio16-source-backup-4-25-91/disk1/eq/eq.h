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

static struct NewWindow EQ_NewWindow = {
   (640-548)>>1,
   15,
   330,
   63,
   BLACK,
   WHITE,
   CLOSEWINDOW|NEWSIZE|ACTIVEWINDOW|INACTIVEWINDOW,
   REPORTMOUSE|ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|WINDOWSIZING,
   0,0,
   " Graphic Equlizer ", /** TITLE **/
   0,0,
   154,67,640,400,
   CUSTOMSCREEN
   };

/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/
static void EQ31func();
static struct NewSlider EQ31gad=
	{NULL,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ31func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,				/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ62func();
static struct NewSlider EQ62gad=
	{&EQ31gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ62func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ125func();
static struct NewSlider EQ125gad=
	{&EQ62gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ125func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ250func();
static struct NewSlider EQ250gad=
	{&EQ125gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ250func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ500func();
static struct NewSlider EQ500gad=
	{&EQ250gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ500func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ1000func();
static struct NewSlider EQ1000gad=
	{&EQ500gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ1000func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ2000func();
static struct NewSlider EQ2000gad=
	{&EQ1000gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ2000func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ4000func();
static struct NewSlider EQ4000gad=
	{&EQ2000gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ4000func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ8000func();
static struct NewSlider EQ8000gad=
	{&EQ4000gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ8000func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/
static void EQ16000func();
static struct NewSlider EQ16000gad=
	{&EQ8000gad,					/* *next (gadget) */
	NULL,NULL,NULL,NULL,
	SLIDER_COOL|FREE_VERT,			/* Flags */
	EQ16000func,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	0,					/* id */
	NULL,					/* *gad_ptr */
	33,1,16,					/* list_size | show_size | top */
	-1,3};				/* knob_width | knob_Height */
/*���������������������������������������������������������������������*/