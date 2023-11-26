/* mainsed40.c 1/22/87   Milburn   */
#include "sed.include.h"

char comp=0;
int sr_key=60;
extern int SaveIFF();
extern void CreateIFF();
extern void do_menu();
extern void clear_voices();

struct timerequest tr;
struct Remember *RememberKey;

#define DISKMAX 12
short j;
struct Menu DiskMenu =
{ NULL,0,0,115,11,MENUENABLED,"Project",NULL };
struct MenuItem DiskItem[DISKMAX];
struct IntuiText DiskText[DISKMAX];

#define EDITMAX 9
struct Menu EditMenu =
{ NULL,118,0,115,11,MENUENABLED,"Edit",NULL };
struct MenuItem EditItem[EDITMAX];
struct IntuiText EditText[EDITMAX];

#define UNDO_MENU_NUMBER (0+1) 

#define TOOLMAX 14 
struct Menu ToolMenu =
{ NULL,236,0,115,11,MENUENABLED,"Tool",NULL };
struct MenuItem ToolItem[TOOLMAX];
struct IntuiText ToolText[TOOLMAX];

#define MISCMAX 6
struct Menu MiscMenu =
{ NULL,354,0,115,11,MENUENABLED,"Keyboard",NULL };
struct MenuItem MiscItem[MISCMAX];
struct IntuiText MiscText[MISCMAX];

#define MIDMAX 6
struct Menu MidMenu =
{ NULL,472,0,115,11,MENUENABLED,"Sequencer",NULL };
struct MenuItem MidItem[MIDMAX];
struct IntuiText MidText[MIDMAX];

struct MenuItem MenuItem_Template =
   {
   NULL,
   0,0,
   135,11,
   ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,NULL,0,NULL,0,
   };

short show_sect = 1;
short bank_offset = 0;
short current_buffer = 0;
short copy_buffer = 0;
short lastlastmoved=0;
struct IntuiMessage *NewMessage;
#define MAXSER 5
#define MAXSECT 16
#define MAXSECTGAD 8
#define MAXNAME 10
#define MAXGRAVY 6
#define MIN_FRSIZE 3


struct IOExtSer IORser,TempIORser[MAXSER];
short nser = 0;
struct MsgPort *port;
short period[MAXSECT][100];
short used[4] = {0,0,0,0};
long loopcom[4] = {0,0,0,0};
short ncycles = 1;
short tempvoice;
unsigned char note_channel[MAXSECT][100];
short key_section[100];
short undo_flag=1,midi_format_flag=0;
short key_split_flag = 0;
char DiskName[25] = "                     ";
short tempo=60;
char midi_record_flag=0;
char ext_clock_flag=0;
char ext_wait_flag=0;
long ext_clock=0;
char midi_playback_flag=0;
char midi_repeat_flag=0;
char midi_overdub_flag=0;
char midi_echo_flag=0;
short n_midi_events=0;
short n_play_events=0;
short n_midi_echoes;
short echo_decay=8;
int midi_echo_delay;
long start_sec,start_mic,now;
short this_event=0;
#define MAXMIDIEVENTS 900
typedef struct event {
	unsigned char what1;
	unsigned char what2;
	unsigned char what3;
	unsigned char voice;
	long when;
	};
struct event event[MAXMIDIEVENTS+1];
/* end of file_choose stuff */
short bendmsb,bendlsb;

short stereo_flag = 0;
short shflag=0;
long stereo_offset;
short current_hist=0;
short velocity_flag= 0;
char vel_vol[128];
short kk;
short done;
short oldpitch[3];
#define MAXTABLESIZE 1000
int table[MAXTABLESIZE];

typedef struct {
	float re;
	float im;
	} complex;

#define MAXFFTPOINTS 512
complex ss[MAXFFTPOINTS];

float fac,fff;
struct section
   {
   int firstsamp,lastsamp;
   int loopstart,loopend;   
   int flags;
   int frsize;
   int currbuf;	
   struct IntuiText *text;
   char name[MAXNAME];
   };
short num,num2,div=100;
struct section section[MAXSECT];
struct section section_defaults;
unsigned char midi[256];
char RString[41];
char UrString[41];

long preserve_begsamp,preserve_endsamp,preserve_nsamps;
extern long zero_search();
extern void delays();
extern void midi_sort();
extern void New();
extern void ReallyNew();
extern void insert();
extern void bevel();
extern void preserve();
extern long playtime();
extern void reset_clock();
extern void save_midi();
extern void load_midi();
extern void midi_playback();
extern void fft();
extern struct AudChannel aud[];
extern struct MsgPort *CreatePort();
extern void comb();
extern void a_m();
extern void echo();
extern void hist();
extern void draw_sect();
extern void begplot();
extern void endplot();
extern void strfloat();
extern void display();
extern void readSer();
extern void querySer();
extern void load();
extern void load_sec();
extern int save();
extern void save_everything();
extern int file_choose();
extern void set_params();
extern int cue();
extern long interpolate();
extern int get_ivalue();
extern float get_fvalue();
extern int get_pitch_value();
extern void reverse();
extern void play();
extern void get_midi();
extern void post();
extern void unpost();
extern void left_slide();
extern void right_slide();
extern void paste();
extern void update_sliders();
extern void next_channel();
extern int record();
extern long compress();
extern long expand();
extern void do_compress();
extern void do_expand();
extern void do_gravy();
extern void draw_key();
extern int any_key();
extern void buffer_toggle();
extern void SPrint();
extern void open_midi();
extern void close_midi();
extern void sot_edit();
extern void overlay();

#define INTUITION_REV 0
#define GRAPHICS_REV 0
#define WHITE 1
#define BLUE 0
#define BLACK 2
#define YELLOW 3
#define NOTE_ON 144
#define VOICE_CHANGE 192
#define PLOT_WIDTH 68
#define LOOP_FLAG 1
#define SPLIT_FLAG 2
#define STEREO_ON_FLAG 4
#define AM_FLAG 8
#define SUSTAIN_FLAG 16

struct IntuiMessage *message,*oldmsg;
extern struct Library *OpenLibrary();
extern struct Task *FindTask();
/* channel allocation map */
UBYTE allocationMap[] = {15};

struct RastPort *rastPort;
struct Library *MathBase = 0;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;

struct MsgPort *allocPort = 0;
struct IOAudio *allocIOB;
struct Device *device = 0;
struct Interrupt *interrupt = 0;
struct MsgPort *soundPort = 0;
BYTE *buffer,*buffer0,*buffer1,*orig,*dest,*undo_buffer;
#define MAXCOM 20
int curcom = 0;
struct IOAudio *audcom[MAXCOM];
unsigned char fn[40];
char SpaceStr[50] = "                                     ";
char TempStr[42];
ULONG bufsiz = 100000;

BYTE *SliderData;
#define SLIDERSIZE 98
USHORT OriginalSliderData[] =
{
0x1800,0x3C00,0x7E00,0xFF00,0x1800,0x2400,
0x4200,0x9900,0x522E,0xFFFB,0x776F,0x726B,
0x696E,0x672E,0x2E2E,0x6E6C,0x792E,0x0A0A,
0x2062,0x6520,0x6672,0x6565,0x6C79,0x2064,

0x6973,0x7472,0x6962,0x7574,0x6564,0x2066,
0x6F72,0x206E,0x6F6E,0x2D70,0x726F,0x6669,
0x740A,0x6400,0x0116,0x2006,0x2206,0xE581,
0x41EE,0xFF88,0xD1C1,0x2D48,0xFFCC,0x4A90,
};
struct Image Slider =
{
0,0,          /* LeftEdge, TopEdge */
8,4,2,      /* Width, Height, Depth */
&OriginalSliderData[0],
0x3,0x0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};

struct PropInfo PerInfo =
   {
   FREEHORIZ,
   0,0,
   1000,1000,
   0,0,0,0,0,0,
   };

struct Gadget PerGadget =
   {
   NULL,
   16,160,250,8,
   GADGHCOMP | GADGIMAGE,
   GADGIMMEDIATE | RELVERIFY,
   PROPGADGET,
   (APTR)&Slider,
   NULL,
   NULL,
   0,
   NULL,
   0,
   NULL
   };

struct IntuiText TText =
   {
   WHITE,BLUE,   /* foreground,backgroiund */
   JAM1,
   3,3, /* x,y */
   NULL,
   NULL, /* point to text */
   NULL,
   };

struct IntuiText MText =
   {
   BLACK,WHITE,   /* foreground,backgroiund */
   JAM2,
   0,0, /* x,y */
   NULL,
   NULL, /* point to text */
   NULL,
   };

USHORT LimitData[] =
   {
   0x0FF0,
   0xFFFF,
   0xFFFF,
   0x0FF0,
   0x0FF0,
   };
/*
struct Requester StringRequest =
   {
   NULL,
   2,2,
   200,40,
   NULL,NULL,
   NULL, 
   NULL,
   NULL, 
   NULL,
   WHITE,
   NULL,NULL,NULL,NULL,NULL
   };
*/
struct Requester SliderRequest =
   {
   NULL,
   320,88,
   300,50,
   NULL,NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   WHITE,
   NULL,NULL,NULL,NULL,NULL
   };

struct Requester PostIt =
   {
   NULL,
   200,30,
   200,15,
   NULL,NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   BLACK,
   NULL,NULL,NULL,NULL,NULL
   };

struct Gadget SectNameGadget[MAXSECTGAD];

struct StringInfo SectNameInfo[MAXSECTGAD];

UBYTE SIBuffer1[MAXNAME];

/* */

struct StringInfo ReqStringInfo = {
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

struct Gadget goodstrgad = {
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
	(APTR)&SIBuffer1,	/* SpecialInfo structure for string gadgets */
	1,	/* user-definable data (ordinal gadget number) */
	NULL	/* pointer to user-definable data */
};


/* */

struct IntuiText ReqText;
struct IntuiText PostItText;
struct Image LimitImage[6];
struct PropInfo LimitInfo[6];
struct Image DoneImage;
struct Gadget DoneGadget;
struct IntuiText DoneText;
struct Image QuitImage;
struct Gadget QuitGadget;
struct IntuiText QuitText;

struct Gadget LimitGadget[6];
struct Image PlayImage;
struct Gadget PlayGadget;
struct IntuiText PlayText;
struct Image SetImage;
struct Gadget SetGadget;
struct IntuiText SetText;
struct Image LoopImage;
struct Gadget LoopGadget;
struct IntuiText LoopText;
struct Image StopImage;
struct Gadget StopGadget;
struct IntuiText StopText;
struct Image BufImage;
struct Gadget BufGadget;
struct IntuiText BufText;
struct Image BankImage;
struct Gadget BankGadget;
struct IntuiText BankText;
struct Image TuneImage;
struct Gadget TuneGadget;
struct IntuiText TuneText;
struct Image SectionImage[8];
struct Gadget SectionGadget[8];
struct IntuiText SectionText[MAXSECTGAD];
char SectionStr[MAXSECTGAD][3];
char BankStr[5];
struct IntuiText TempMeterText;
struct IntuiText FrsizeText;
struct IntuiText DurationText;
struct IntuiText PerText;
struct IntuiText ValueText;
struct IntuiText BodyText;
struct IntuiText PosText;
struct IntuiText NegText;

#define BUTTONSIZE 316
BYTE *ButtonData;
USHORT OriginalButtonData[] =
{

0xFFFF,0xFFFF,0xFE00,0xFFFF,0xFFFF,0xFC00,
0xFFFF,0xFFFF,0xFA00,0xE000,0x0000,0x0C00,
0xE000,0x0000,0x0A00,0xE000,0x0000,0x0C00,
0xE000,0x0000,0x0A00,0xE000,0x0000,0x0C00,
0xE000,0x0000,0x0A00,0xE000,0x0000,0x0C00,
0xEAAA,0xAAAA,0xAA00,0xD555,0x5555,0x5400,
0xAAAA,0xAAAA,0xAA00,0xAAAA,0xAAAA,0xAA00,
0x5555,0x5555,0x5600,0x8000,0x0000,0x0600,
0x5FFF,0xFFFF,0xF600,0x9FFF,0xFFFF,0xF600,
0x5FFF,0xFFFF,0xF600,0x9FFF,0xFFFF,0xF600,
0x5FFF,0xFFFF,0xF600,0x9FFF,0xFFFF,0xF600,
0x5FFF,0xFFFF,0xF600,0xB555,0x5555,0x5600,
0x7FFF,0xFFFF,0xFE00,0xFFFF,0xFFFF,0xFE00,

0x776F,0x726B,0x696E,0x672E,0x2E2E,0x6E6C,
0x792E,0x0A0A,0x2062,0x6520,0x6672,0x6565,
0x6C79,0x2064,0x6973,0x7472,0x6962,0x7574,
0x6564,0x2066,0x6F72,0x206E,0x6F6E,0x2D70,
0x726F,0x6669,0x740A,0x0001,0x0001,0xA010,
0x00FC,0x158E,0x00FE,0x1454,0x0000,0x3DA4,
0x00FE,0x06B8,0x0001,0xA010,0x0000,0x0080,
0x0000,0x8000,0x0000,0x0001,0x0001,0xA010,
0x0001,0xFCC8,0x0001,0xFCC8,0x00FD,0xF91A,
0x0001,0xA010,0x0001,0xA088,0x0000,0x0001,
0x0001,0xA340,0x0001,0xA010,0x0001,0xA010,
0x00FD,0xF9C6,0x0001,0xA010,0x0000,0x0140,
0x0000,0x0048,0x0000,0x0049,0x0001,0xEF2C,
};


struct Image Button =
{
0,0,          /* LeftEdge, TopEdge */
39,13,2,      /* Width, Height, Depth */
&OriginalButtonData[0],
31,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};



/* Boolean Template */

struct Gadget BTempGadget =
{
NULL,
340,89,39,13,
GADGIMAGE | GADGHCOMP,
GADGIMMEDIATE,
BOOLGADGET,
NULL, /* (APTR)&Button, */
NULL,NULL,0,NULL,0,NULL
};

    int comvolume = 64;	
    short fake_rate = 5;
    long comdata,comlength;
    int channels = LEFT;
    int command;      
    int bendrange = 8;   
    long m;
    long nsamps0,nsamps1;
    long bs0,es0,bs1,es1;	
    struct Message *latest_msg;
    int maxamp,sr;
    int setflag = 0;   

    float rescale,xmult,feedback=.8;
#define MAXFILTER 1000
    char filter[MAXFILTER];
    float floatmax;
    int section_x = 2;
    int section_y = 176;
    int begsamp_x = 20;
    int begsamp_y = 92;
    int endsamp_x = 90;
    int endsamp_y = 92;
    int frsize_x = 452;
    int frsize_y = 163-15;
    int gduration_x = 452;
    int gduration_y = 181-15;
    int duration_x = 528;
    int duration_y = 141-15;
    int per_x = 374;
    int per_y = 141-15;
    int IntJunk;
    int midi_data=0;
    int midi_read,midi_status,midi_volume,midi_voice;
    int midi_channel,in_channel=0;
    int midi_flag = 0;   
    int midi_pitch = 0;
    char midi_testify=0;
    long int nsamps,begsamp,endsamp,tempsamp;
    long int frsize;
    int frmax,durmax;
    double gduration,duration,tic;
    double rad,del;		
    int n,i;
    int reqflag;
    short nvoice=0;
    int pper = 300;
    int midpper = 300;   
    int filenum;
    FILE *fp;
    char c,tempc;
    ULONG class;
    USHORT code;
    APTR address;
    SHORT moux,mouy;
    long LongJunk;
    float FloatJunk,durmult,tune_constant,FloatShit;
    int nsections=0;
    int current_section=0;
    int nbytes;
    int error;
    int actual;
    short lastmoved=0;
    char lastbeg=0;
    short lastbuffer=0;
    long l,paste_start,paste_end,dif;
    long copy_start=-2;
    long copy_end=-1;
    int MenuNumber;
    int ItemNumber;
    unsigned long rbl;
    unsigned long brk;
    unsigned long baud;
    unsigned char rwl;
    unsigned char wwl;
    unsigned char sf;
    unsigned long t0;
    unsigned long t1;
    int tlength=1;
    int k;
    int nsects=MAXSECT;


#define FGAD_X 8
#define FGAD_Y 12
#define FGAD_H 8

#define NFILES 14
struct FileLock *mylock,*oldlock,*thelock;
int well,nfiles;
char FileNameStr[62];
char OutStr[81];
char mycurdir[67] = "DF0:";
char thedir[67] = "DF0:";
short curdev = 0;
char default_unit[12] = "DF0:";
struct NewWindow NewWindow =
{ 0,0,640,200,0,1,GADGETDOWN | GADGETUP | REQCLEAR | MENUPICK,
SMART_REFRESH | NOCAREREFRESH | ACTIVATE | BORDERLESS | WINDOWDEPTH,
(struct Gadget *)&LimitGadget[0],
NULL,"Studio Magic v.1.4 © 1987 Sunrize Industries",NULL,NULL,NULL,NULL,NULL,NULL,WBENCHSCREEN};

char SPrintStr[62];
struct IntuiText FNText =
   {
   WHITE,BLUE,   /* foreground,backgroiund */
   JAM2,
   0,0, /* x,y */
   NULL,
   &SPrintStr[0], /* point to text, after copying, will double as SPrint's */
   NULL,
   };

struct Gadget AGadget =
{
NULL,
FGAD_X,FGAD_Y,309-FGAD_X,FGAD_H,
GADGIMAGE | GADGHCOMP,
GADGIMMEDIATE | TOGGLESELECT,
BOOLGADGET,
NULL,
NULL,NULL,0,NULL,0,NULL
};
int f = 0;

main(argc,argv)
char *argv[];
{
/* set up menus */
if(argc>1) bufsiz=atol(argv[1]);
	else bufsiz=100000;

   DiskMenu.NextMenu = &EditMenu;
   EditMenu.NextMenu = &ToolMenu;
   ToolMenu.NextMenu = &MiscMenu;
   MiscMenu.NextMenu = &MidMenu;

   DiskMenu.FirstItem = &DiskItem[0];
   EditMenu.FirstItem = &EditItem[0];
   ToolMenu.FirstItem = &ToolItem[0];
   MiscMenu.FirstItem = &MiscItem[0];   
   MidMenu.FirstItem = &MidItem[0];   
   for(k=0;k<DISKMAX;k++)
      {
      DiskItem[k] = MenuItem_Template;
      DiskItem[k].TopEdge = 11*k;
      DiskItem[k].ItemFill = (APTR)&DiskText[k];
      DiskText[k] = MText;
      DiskItem[k].NextItem = &DiskItem[k+1];   
      }
   DiskItem[DISKMAX-1].NextItem = NULL;
   DiskText[0].IText = "New";	
   DiskText[1].IText = "Open";
   DiskText[2].IText = "Append";
   DiskText[3].IText = "Record";
   DiskText[4].IText = "Load Midi";
   DiskText[5].IText = "Save Midi";  
   DiskText[6].IText = "Save Samples";
   DiskText[7].IText = "Save Everything";
   DiskText[8].IText = "Stereo On";   
   DiskText[11].IText = "Quit";
   DiskText[9].IText = "Save IFF Instr";
   DiskText[10].IText = "Create IFF Instr";

   for(k=0;k<EDITMAX;k++)
      {
      EditItem[k] = MenuItem_Template;
      EditItem[k].TopEdge = 11*k;
      EditItem[k].ItemFill = (APTR)&EditText[k];
      EditText[k] = MText;
      EditItem[k].NextItem = &EditItem[k+1];
      }
   EditItem[EDITMAX-1].NextItem = NULL;
   EditText[0].IText = "UNDO";
   EditText[1].IText = "Mark";
   EditText[2].IText = "Paste";
   EditText[3].IText = "Overlay";
   EditText[4].IText = "Remove";
   EditText[5].IText = "Clear Buffer";
   EditText[6].IText = "Insert";
   EditText[7].IText = "Bevel";
   EditText[8].IText = "Zero Region";

   for(k=0;k<TOOLMAX;k++)
      {
      ToolItem[k] = MenuItem_Template;
      ToolItem[k].TopEdge = 11*k;
      ToolItem[k].ItemFill = (APTR)&ToolText[k];
      ToolText[k] = MText;
      ToolItem[k].NextItem = &ToolItem[k+1];   
      }
   ToolItem[TOOLMAX-1].NextItem = NULL;
   ToolText[0].IText = "Reverse";
   ToolText[1].IText = "Interpolate";
   ToolText[2].IText = "Compress";
   ToolText[3].IText = "Expand";
   ToolText[4].IText = "Gravy";	
   ToolText[5].IText = "Rescale";		
   ToolText[6].IText = "Delays";
   ToolText[7].IText = "Flange";
   ToolText[8].IText = "Comb";
   ToolText[9].IText = "AM";
   ToolText[10].IText = "Echo";
   ToolText[11].IText = "DC Bias"; 
   ToolText[12].IText = "FFT";
   ToolText[13].IText = "Invert";

   for(k=0;k<MISCMAX;k++)
      {
      MiscItem[k] = MenuItem_Template;
      MiscItem[k].TopEdge = 11*k;
      MiscItem[k].ItemFill = (APTR)&MiscText[k];
      MiscText[k] = MText;
      MiscItem[k].NextItem = &MiscItem[k+1];   
      }
   MiscItem[MISCMAX-1].NextItem = NULL;
   MiscText[0].IText = "Midi Enable";
   MiscText[1].IText = "Set Split Keyboard";
   MiscText[2].IText = "Enable Split";
   MiscText[3].IText = "Set Sustain";
   MiscText[4].IText = "Velocity On";
   MiscText[5].IText = "Set Midi Channel";

   for(k=0;k<MIDMAX;k++)
      {
      MidItem[k] = MenuItem_Template;
      MidItem[k].TopEdge = 11*k;
      MidItem[k].ItemFill = (APTR)&MidText[k];
      MidText[k] = MText;
      MidItem[k].NextItem = &MidItem[k+1];   
      }
   MidItem[MIDMAX-1].NextItem = NULL;
   MidText[0].IText = "Midi Record";
   MidText[1].IText = "Midi Playback";
   MidText[2].IText = "Midi Repeat On";
   MidText[3].IText = "Midi Overdub";
   MidText[4].IText = "Tempo Adjust";
   MidText[5].IText = "Ext Sync On";

    midi_read=0;
    tic=279365./1000000.;
    frmax=4000;
    durmax=120;
    section_defaults.firstsamp = 0;
    section_defaults.lastsamp = 2;
    section_defaults.loopstart = 0;
    section_defaults.loopend = 0;      
    section_defaults.flags = 0;
    section_defaults.currbuf = 0;
    section_defaults.text = NULL;
    section_defaults.name[0]=0;
/*    strncpy(section_defaults.name,SpaceStr,MAXNAME); */
    strcpy(BankStr,"9-16");   
    for(k=0;k<101;k++)
   key_section[k]=0;


   soundPort = CreatePort("audad",0);         


 /* SET UP the message port in the I/O request */
    port = CreatePort (SERIALNAME,0);
    IORser.IOSer.io_Message.mn_ReplyPort = port;

/*    SET PARAMS for the serial.device */

    rbl = 512;
    rwl = 0x08;
    wwl = 0x08;
    brk = 750000;
    baud= 31250;
    sf  = 0x00;
    t0  = 0x51040303;
    t1  = 0x03030303;
  IntuitionBase = (struct IntuitionBase *)
      OpenLibrary("intuition.library",INTUITION_REV);
   if(IntuitionBase == NULL) exit(FALSE);

   GfxBase = (struct GfxBase *)
      OpenLibrary("graphics.library",INTUITION_REV);
   if(GfxBase == NULL) exit(FALSE);


    tune_constant = pper*pow(2.0,5.);
    for(n=0;n<100;n++)
       {
       FloatJunk = n/12.;
       i = tune_constant/pow(2.0,FloatJunk);
       for(k=0;k<nsects;k++)
       period[k][n] = i;
       }

   StopText = TText;
   LoopText = TText;
   PlayText = TText;
   SetText = TText;
   BankText = TText;
   BufText = TText;
   TuneText = TText;
   DoneText = TText;
   QuitText = TText;
  
   

   PosText.TopEdge=0;
   NegText.TopEdge=0;
   for(k=0;k<MAXSECT;k++)
   {
   section[k] = section_defaults;
   }
   for(k=0;k<MAXSECTGAD;k++)
      {
      SectionText[k] = TText;
      n = stcu_d(SectionStr[k],k+1,3);
      SectionText[k].IText = &SectionStr[k][0];
      SectionText[k].TopEdge = 3;
      SectionText[k].LeftEdge = 8;
      }
   FrsizeText = TText;
   DurationText = TText;
   PerText = TText;
   ValueText = TText;
   ReqText = TText;
   PostItText = TText;

   PostItText.TopEdge = 4;
   
   LoopText.IText = "Loop";
   PlayText.IText = "Play";
   DoneText.IText = "Done";
   QuitText.IText = "Quit";
   SetText.IText = "Set";
   StopText.IText = "Stop";
   BankText.IText = &BankStr;
   BufText.IText = "Buff";
   TuneText.IText = "Tune";

   FrsizeText.IText = "Frame Size";
   DurationText.IText = "Framed Duration";
   PerText.IText = "Rate";
   ValueText.FrontPen=BLACK;
   ValueText.TopEdge=1;
   ValueText.LeftEdge=40;

   ReqText.FrontPen=BLACK;
   ReqText.TopEdge=6;
   ReqText.LeftEdge=4;

   BodyText = ReqText;
	BodyText.TopEdge=20;

   PosText = ReqText;
   NegText = ReqText;
   PosText.TopEdge = 4;
   NegText.TopEdge = 4;
   RString[0] = 0;


   FrsizeText.TopEdge = 9;
   DurationText.TopEdge = 9;
   PerText.TopEdge = 10;

   StopGadget = BTempGadget;
   StopGadget.LeftEdge = 367;
   StopImage = Button;
   StopGadget.GadgetRender = (APTR)&StopImage;
   StopGadget.GadgetText = (struct IntuiText *)&StopText;

   BufGadget = BTempGadget;
   BufGadget.LeftEdge = 549;
   BufImage = Button;
   BufGadget.GadgetRender = (APTR)&BufImage;
   BufGadget.GadgetText = (struct IntuiText *)&BufText;

   BankGadget = BTempGadget;
   BankGadget.LeftEdge = 124;
   BankGadget.TopEdge = 186;	
   BankImage = Button;
   BankGadget.GadgetRender = (APTR)&BankImage;
   BankGadget.GadgetText = (struct IntuiText *)&BankText;

   TuneGadget = BTempGadget;
   TuneGadget.LeftEdge = 492;
   TuneImage = Button;
   TuneGadget.GadgetRender = (APTR)&TuneImage;
   TuneGadget.GadgetText = (struct IntuiText *)&TuneText;

   PlayGadget = BTempGadget;
   PlayGadget.LeftEdge = 307;
   PlayImage = Button;
   PlayGadget.GadgetRender = (APTR)&PlayImage;
   PlayGadget.GadgetText = (struct IntuiText *)&PlayText;


   DoneGadget = BTempGadget;
   DoneGadget.LeftEdge = 40;
   DoneGadget.TopEdge = 30;
   DoneImage = Button;
   DoneGadget.GadgetRender = (APTR)&DoneImage;
   DoneGadget.GadgetText = (struct IntuiText *)&DoneText;
   DoneGadget.Activation = ENDGADGET | GADGIMMEDIATE;
   DoneGadget.GadgetType = BOOLGADGET | REQGADGET;
   DoneGadget.NextGadget = (APTR)&QuitGadget; 


   QuitGadget = BTempGadget;
   QuitGadget.LeftEdge = 180;
   QuitGadget.TopEdge = 30;
   QuitImage = Button;
   QuitGadget.GadgetRender = (APTR)&QuitImage;
   QuitGadget.GadgetText = (struct IntuiText *)&QuitText;
   QuitGadget.Activation = ENDGADGET | GADGIMMEDIATE;
   QuitGadget.GadgetType = BOOLGADGET | REQGADGET;

   SetGadget = BTempGadget;
   SetGadget.LeftEdge = 124;
   SetGadget.TopEdge = 172;
   SetImage = Button;
   SetGadget.GadgetRender = (APTR)&SetImage;
   SetGadget.GadgetText = (struct IntuiText *)&SetText;

   LoopImage = Button;
   LoopGadget = BTempGadget;
   LoopGadget.LeftEdge = 427;
   LoopGadget.GadgetRender = (APTR)&LoopImage;
   LoopGadget.GadgetText = (struct IntuiText *)&LoopText;

   for(k=0;k<MAXSECTGAD;k++)
      {
      SectionImage[k] = Button;
      SectionGadget[k] = BTempGadget;
      SectionGadget[k].LeftEdge = 168;
      SectionGadget[k].TopEdge = 88+14*k;
      SectionGadget[k].GadgetRender = (APTR)&SectionImage[k];
      SectionGadget[k].GadgetText = (struct IntuiText *)&SectionText[k];
      SectionGadget[k].NextGadget = (struct Gadget *)&SectionGadget[k+1];
      }
   SectionGadget[MAXSECTGAD-1].NextGadget = NULL;
   for(n=0;n<6;n++)
      {
      LimitGadget[n] = PerGadget;
      LimitImage[n] = Slider;
      LimitInfo[n] = PerInfo;
      LimitGadget[n].Width = 608;
      LimitGadget[n].GadgetRender = (struct Image *)&LimitImage[n];
      LimitGadget[n].SpecialInfo = (APTR)&LimitInfo[n];
      LimitGadget[n].NextGadget = (struct Gadget *)&LimitGadget[n+1];	
      }
      LimitGadget[4].NextGadget = (struct Gadget *)&PlayGadget;

      	
      LimitGadget[0].TopEdge = 70;
      LimitGadget[1].TopEdge = 80;
      LimitGadget[2].TopEdge = 162;
      LimitGadget[3].TopEdge = 162;
      LimitGadget[4].TopEdge = 120-15; /*right above pergadget ?*/	
      LimitInfo[2].HorizBody = 65535/2;
      LimitInfo[3].HorizBody = 65535/2;
	
      LimitGadget[5].TopEdge = 10; /* this is for the requester */
      LimitGadget[5].LeftEdge = 40; 	
      LimitGadget[5].Width = 240;
      LimitGadget[5].GadgetType = PROPGADGET | REQGADGET;
      LimitGadget[5].NextGadget = &DoneGadget;
      LimitInfo[5].HorizBody = 65535/64;

	
      LimitGadget[2].Width = PLOT_WIDTH;
      LimitGadget[3].Width = PLOT_WIDTH;
      LimitGadget[4].Width = 310;

      LimitGadget[2].LeftEdge = 16;
      LimitGadget[3].LeftEdge = 86;
      LimitGadget[4].LeftEdge = 310;

      LimitInfo[0].HorizPot = 65535;
      LimitInfo[1].HorizPot = 1;
      LimitInfo[2].HorizPot = 65535/2;
      LimitInfo[3].HorizPot = 65535/2;
      PerInfo.HorizPot = 65535/6; /* kludge */   

      LimitGadget[4].GadgetText = NULL;
      	
	
      PerGadget.GadgetRender = (APTR)&Slider;
      PerGadget.SpecialInfo = (APTR)&PerInfo;
      PerGadget.TopEdge = 132-15;
      PerGadget.Width = 310;
      PerGadget.LeftEdge = 310;
      PerGadget.GadgetText = (struct IntuiText *)&PerText;
      LimitGadget[4].GadgetText = NULL;
      LimitGadget[4].Width = 310;
      LimitGadget[4].LeftEdge = 310;		

      SliderRequest.ReqGadget = (struct Gadget *)&LimitGadget[5];
      SliderRequest.ReqText = (struct IntuiText *)&ValueText;

      PostIt.ReqGadget = NULL;
      PostIt.ReqText = (struct IntuiText *)&PostItText;      

      for(k=0;k<MAXSECTGAD;k++)
   {   
   SectNameGadget[k]=goodstrgad;

   SectNameGadget[k].TopEdge = 90+14*k;
   SectNameGadget[k].LeftEdge = 212;
   SectNameGadget[k].Width = 90;
   SectNameGadget[k].Height = 14;
   SectNameGadget[k].NextGadget = (struct Gadget *)&SectNameGadget[k+1];
   SectNameGadget[k].SpecialInfo = (APTR)&SectNameInfo[k];
   SectNameInfo[k] = ReqStringInfo;
   SectNameInfo[k].Buffer = (APTR)&section[k].name[0];
   SectNameInfo[k].MaxChars = MAXNAME;      
   }

   SectNameGadget[MAXSECTGAD-1].NextGadget = NULL;
   PlayGadget.NextGadget = (struct Gadget *)&SetGadget;
   SetGadget.NextGadget = (struct Gadget *)&PerGadget;
   PerGadget.NextGadget = (struct Gadget *)&LoopGadget;
   LoopGadget.NextGadget = (struct Gadget *)&StopGadget;
   StopGadget.NextGadget = (struct Gadget *)&BufGadget;
   BufGadget.NextGadget = (struct Gadget *)&BankGadget;
   BankGadget.NextGadget = (struct Gadget *)&TuneGadget;
   TuneGadget.NextGadget = (struct Gadget *)&SectionGadget[0];
   SectionGadget[MAXSECTGAD-1].NextGadget = (struct Gadget *)&SectNameGadget[0];   
   /* the meters */
   TempMeterText = TText;
   TempMeterText.TopEdge=0;
   TempMeterText.LeftEdge=0;
   TempMeterText.FrontPen = WHITE;
   TempMeterText.BackPen = BLACK;
   TempMeterText.DrawMode = JAM1;
   TempMeterText.IText = (APTR)&TempStr[0];

   if((Window = (struct Window *)OpenWindow(&NewWindow)) == NULL)
                     exit(FALSE);

   rastPort = Window->RPort;

   /* add menus */
   SetMenuStrip(Window,&DiskMenu);
   OffMenu(Window,UNDO_MENU_NUMBER);
   /* broaden str gadgets */
   for(k=0;k<MAXSECTGAD;k++)
   SectNameInfo[k].NumChars = MAXNAME-1;
	k=OpenDevice(TIMERNAME,UNIT_MICROHZ,&tr,0);
	if(k!=0) cleanUp("Timer device error");
	tmsg.mn_Node.ln_Type = NT_MESSAGE;
	tmsg.mn_Node.ln_Pri = 0;
	tmsg.mn_Node.ln_Name = NULL;
	tmsg.mn_ReplyPort = NULL;
/* timer device now open and tr struct initialzed */

for(k=36;k<=100;k++)
	draw_key(k,FALSE);	

    durmult=4.;	
	SPrint(rastPort,"Duration",duration_x-80,duration_y,10);
	RememberKey=NULL;

    if ((allocPort = CreatePort("sound example", 0)) == 0)
     cleanUp("Cannot create reply port");
    if ((allocIOB = (struct IOAudio *)AllocRemember(&RememberKey,sizeof(struct IOAudio),
         MEMF_PUBLIC | MEMF_CLEAR)) == 0)
     cleanUp("Out of memory");
    allocIOB->ioa_Request.io_Message.mn_Node.ln_Pri = -40;
    allocIOB->ioa_Request.io_Message.mn_ReplyPort = allocPort;
    allocIOB->ioa_Data = allocationMap;
    allocIOB->ioa_Length = sizeof(allocationMap);

    /* open the audio device with channel allocation and check for errors */

    switch (OpenDevice(AUDIONAME, 0, allocIOB, 0)) {
    case IOERR_OPENFAIL:
     cleanUp("Cannot open audio device");
    case ADIOERR_ALLOCFAILED:
     cleanUp("Cannot allocate audio channel");
    }
    device = allocIOB->ioa_Request.io_Device;
     
      for(m=0;m<MAXCOM;m++)

      {
     if ((audcom[m] = (struct IOAudio *)AllocRemember(&RememberKey,sizeof(struct IOAudio),
          MEMF_PUBLIC | MEMF_CLEAR)) == 0)
         cleanUp("Out of memory");
     audcom[m]->ioa_Request.io_Message.mn_ReplyPort = soundPort;
     audcom[m]->ioa_Request.io_Device = allocIOB->ioa_Request.io_Device;
     audcom[m]->ioa_Request.io_Unit = 15;
     audcom[m]->ioa_Request.io_Flags = ADIOF_PERVOL;
     audcom[m]->ioa_AllocKey = allocIOB->ioa_AllocKey;
     audcom[m]->ioa_Volume = 64;
             }

     command = ADCMD_FINISH;
     comdata = buffer;
     comlength = nsamps;   

   	l=AvailMem(MEMF_CHIP);
printf("Availmem says %ld\n",l);
if(bufsiz==100000) bufsiz=(l-59000)/2;
printf("Allocating buffers of %ld\n",bufsiz);

if ((buffer0 = (BYTE *)AllocRemember(&RememberKey,bufsiz,
	 MEMF_CHIP | MEMF_CLEAR)) == 0)
	         cleanUp("Out of memory");
if ((buffer1 = (BYTE *)AllocRemember(&RememberKey,bufsiz,
	 MEMF_CHIP | MEMF_CLEAR)) == 0)
		 cleanUp("Out of memory");
   buffer = buffer0;
   stereo_offset=buffer1-buffer0;

if ((ButtonData = (BYTE *)AllocRemember(&RememberKey,BUTTONSIZE,
	 MEMF_CHIP | MEMF_CLEAR)) == 0)
		 cleanUp("Out of memory on button allocation.");


movmem(OriginalButtonData,ButtonData,BUTTONSIZE);

Button.ImageData = ButtonData;

if ((SliderData = (BYTE *)AllocRemember(&RememberKey,SLIDERSIZE,
	 MEMF_CHIP | MEMF_CLEAR)) == 0)
		 cleanUp("Out of memory on slider allocation.");


movmem(OriginalSliderData,SliderData,SLIDERSIZE);

Slider.ImageData = SliderData;

   nsamps0 = 2;
   nsamps1 = 2;
     sr=(1000000/(midpper*.279365))+.5;
     n = stcu_d(TempStr,sr,10);
     display(per_x,per_y,7);
     nsamps=2;
     endsamp=2;
    begsamp=0;
    nsamps=endsamp;
    bs0 = 0;
    bs1 = 0;
    es0 = 2;
    es1 = 2;
    duration=pper*tic*(endsamp-begsamp)/1000000.;
    hist(begsamp,nsamps);
    update_sliders();	
    nbytes=0;

   for(k=0;k<128;k++) vel_vol[k]=pow(2.0,k*6./127.);
   
   for(k=0;k<MAXCOM;k++)
      {
      audcom[k]->ioa_Request.io_Command = ADCMD_FINISH;
      audcom[k]->ioa_Request.io_Unit = 1;
      BeginIO(audcom[k]);		
      }

    open_midi();
	l=AvailMem(MEMF_CHIP);
if(l<50000) printf("\nWARNING: Only %ld bytes of CHIP memory left.\n",l);
	
    while (1)
      {
  Delay(1);        

   get_midi();
   if(midi_playback_flag) midi_playback();
   
/*     while(oldmsg != (message = (struct IntuiMessage *)GetMsg(port)))
	{
	oldmsg=message;
         ReplyMsg(message);
	} */
   
     while(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
         {
         /* there is some info from IDCMP */
         class = message->Class;
         code = message->Code;
         address = message->IAddress;
         moux = message->MouseX;
         mouy = message->MouseY;
         ReplyMsg(message);
         switch(class)
            {
            case MENUPICK:
	    do_menu();
            break; /* end of menupick case */
            case GADGETDOWN:
               if (address == (APTR)&BankGadget)
                  {
         if(bank_offset == 0)
            {
            bank_offset = 8;
            strcpy(BankStr,"0-8",5);
            }
         else
            {
            bank_offset = 0;
            strcpy(BankStr,"9-16",6);
            }
         for(k=0;k<MAXSECTGAD;k++)
         {
         n = stcu_d(&SectionStr[k][0],k+bank_offset+1,3);
         SectNameInfo[k].Buffer = &section[k+bank_offset].name;
	if(current_section == k+bank_offset) SectionText[k].FrontPen=YELLOW;
	else SectionText[k].FrontPen=WHITE;
	SPrint(rastPort,SpaceStr,212,90+14*k,10);
         }
        RefreshGadgets(&LimitGadget[0],Window,NULL);
                  }
               if (address == (APTR)&TuneGadget)
                  {
		  if(midi_flag == 0)
			open_midi();
		if((midi_testify == 0) && (midi_data==0)) 
			if(cue("     Do you have a midi keyboard?") == 0) break;
		  midi_testify=1;
		  n=midi_pitch;
		  post("Play note");
		  while(n == midi_pitch)
			{
			get_midi();
			}
		  unpost();
		  flush_channel();
		command=ADCMD_FINISH;
		play();
		  n = midi_pitch;
		  sr_key = midi_pitch;
		  close_midi();
		  post("Tuning");
		  FloatJunk = n/12.;	
                  tune_constant = midpper*pow(2.0,FloatJunk);
                  for(n=0;n<100;n++) {
                     FloatJunk = n/12.;
                     period[current_section][n] = tune_constant/pow(2.0,FloatJunk);
                     }
	open_midi();
	flush_channel();
        unpost();
	for(k=36;k<=100;k++) draw_key(k,FALSE);
		} /* end of if TUNE */
      if (address == (APTR)&BufGadget)
         {
	 buffer_toggle();
	 hist(0,nsamps);
	 update_sliders();
         }      
           if (address == (APTR)&PlayGadget)
                  {
   next_channel();
        pper = midpper;
        command = CMD_WRITE;
        comdata = &buffer[begsamp];
        comlength = endsamp-begsamp;  
	comvolume = 64;    
                  play();      
                  }
               if (address == (APTR)&LoopGadget)
           {
        if (ncycles == 1)
      {
           ncycles = 0;
         next_channel();
           pper = midpper;
           command = CMD_WRITE;
           comdata = &buffer[begsamp];
           comlength = endsamp-begsamp;      
	   LoopText.FrontPen=YELLOW;
	   RefreshGadgets(&LimitGadget[0],Window,NULL);
           play();      
      }
        else
      {
	l=begsamp;
	m=endsamp;
/*	begsamp=zero_search(begsamp);
	endsamp=zero_search(endsamp); */
	zero_search();
	if((endsamp-begsamp)<3) {
		begsamp=l;
		endsamp=m;
		}
	update_sliders();
        	 if (command == CMD_WRITE)
            		{
	                 command = ADCMD_FINISH;
		            play();
        		    command = CMD_WRITE;
		            play();
        		    }
      		}
            }
               if (address == (APTR)&StopGadget)
                  {
	if(ncycles==0) {
		ncycles=1;
		LoopText.FrontPen=WHITE;
	        RefreshGadgets(&LimitGadget[0],Window,NULL);
		} 
	clear_voices();
	}
	/* now check to see if a Section Gadget went down */
	shflag=-1;
        for(k=0;k<MAXSECTGAD;k++) 
         if (address == (APTR)&SectionGadget[k]) shflag=k;
		if(shflag != -1) {
			ncycles=1;
			LoopText.FrontPen=WHITE;
		       audcom[curcom]->ioa_Request.io_Command = CMD_FLUSH;
		       audcom[curcom]->ioa_Request.io_Unit = 15;
		        BeginIO(audcom[curcom]);   
			for(k=0;k<4;k++)
				used[k]=0;
		for(k=0;k<MAXSECTGAD;k++) 
			if(shflag != k) SectionText[k].FrontPen=WHITE;
			/* now reset */
			k=shflag;
			SectionText[k].FrontPen=YELLOW;
                        current_section = k+bank_offset;
                        if ((section[current_section].lastsamp == 2) || setflag)
                     		{                  
	                        section[current_section].firstsamp = begsamp;
           	                section[current_section].lastsamp = endsamp;
		     		section[current_section].currbuf = current_buffer;	
		                section[current_section].flags = 0;
				if(stereo_flag) section[current_section].flags = 
					(section[current_section].flags | STEREO_ON_FLAG);

                     		setflag = 0;  
			        draw_sect();	 
			        }
		     l=current_buffer;	
		     change_section();
     			if(current_buffer != l)
				{
				hist(0,nsamps);
				}
			update_sliders();
                        n=stcu_d(TempStr,current_section+1,2);
                        display(section_x,section_y,2);
                        strcpy(TempStr,section[current_section].name);
                        display(section_x+24,section_y,10);
			RefreshGadgets(&LoopGadget,Window,NULL);
			} /* end of if shflag != -1 */
                if (address == (APTR)&SetGadget)
                    {
              setflag=1;    
                     }
                  break;
           case GADGETUP:
            if (address == (APTR)&PerGadget || address == (APTR)&LimitGadget[4])
                  {
                  if ((PerInfo.Flags && KNOBHIT) || (LimitInfo[4].Flags && KNOBHIT))
                     {
                     FloatJunk=(PerInfo.HorizPot/65535.)*8.4;
                     midpper=127*pow(2.0,FloatJunk);
			if(address == (APTR)&PerGadget) {
				LimitInfo[4].HorizPot = 65532./2.;/*fine tune */
				RefreshGadgets(&LimitGadget[4],Window,NULL);
				}
		     FloatJunk=((LimitInfo[4].HorizPot*midpper)/65535.)/5.;	
		     midpper=midpper-(midpper/100.)+FloatJunk;	
           pper=midpper;
           sr=(1000000/(midpper*.279365))+.5;
                     n = stcu_d(TempStr,sr,10);
                     display(per_x,per_y,7);
                     duration=pper*tic*(endsamp-begsamp)/1000000.;
                     strfloat(TempStr,duration);
                     display(duration_x,duration_y,8);
                     }
                  }
    if ((address == (APTR)&LimitGadget[0]) || (address == (APTR)&LimitGadget[1]))
                  {
                  if ((LimitInfo[0].Flags && KNOBHIT) || (LimitInfo[1].Flags && KNOBHIT))
                     {
			lastbuffer=current_buffer;
                     if(address == (APTR)&LimitGadget[0]) 
                        lastmoved=0;
                     else
                        lastmoved=1;
                     begsamp=(LimitInfo[0].HorizPot/65535.)*nsamps/2.;
                     endsamp=(LimitInfo[1].HorizPot/65535.)*nsamps/2.;
                     if (begsamp<endsamp)
                        {
                        begsamp=begsamp*2;
                        endsamp=endsamp*2;
			lastbeg=0;
                        }
                     else
                        {
                        tempsamp=begsamp;
                        begsamp=endsamp*2;
                        endsamp=tempsamp*2;
			lastbeg=1;
                        }
		update_sliders();
              if (ncycles == 0)
         if (command == CMD_WRITE)
            {
                 command = ADCMD_FINISH;
            play();
            command = CMD_WRITE;
            play();
            }
                     }
                  }/* end of if address */
               if (address == (APTR)&LimitGadget[2])
                  {
                  if (LimitInfo[2].Flags && KNOBHIT)
                     {
n=((LimitInfo[2].HorizPot)*PLOT_WIDTH/131070) - (PLOT_WIDTH/4);
                     if((begsamp+2*n)>0) begsamp=begsamp+2*n;
                     LimitInfo[2].HorizPot = 0x7FFF;
                 update_sliders();   
                 if (ncycles == 0)
            if (command == CMD_WRITE)
               {
                 command = ADCMD_FINISH;
               play();
               command = CMD_WRITE;
               play();
               }
                    }
                  }
               if (address == (APTR)&LimitGadget[3])
                  {
                  if (LimitInfo[3].Flags && KNOBHIT)
                     {
		n=((LimitInfo[3].HorizPot)*PLOT_WIDTH/131070) - (PLOT_WIDTH/4);
                    if((endsamp+2*n)>0)endsamp=endsamp+2*n;
                     LimitInfo[3].HorizPot = 0x7FFF;
              update_sliders();
                 if (ncycles == 0)
            if (command == CMD_WRITE)
               {
                    command = ADCMD_FINISH;
               play();
                  command = CMD_WRITE;
               play();
               }
                    }
                  }
               break;
            } /* end of switch */
         } /* end of while msg */
       } /* end of while 1 */
} /* end of main */
