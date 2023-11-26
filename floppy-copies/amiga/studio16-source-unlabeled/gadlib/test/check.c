void OpenHelp();
struct NewButton HELPbutton = 
	{&ABORTbutton,				/* *next (gadget) */
	310,75,390,85,			/* Box x1,y1,x2,y2 */
	MANUAL_XOR|BUTN_COOL|BUTN_TOGGLE,				/* Flags */
	OpenHelp,				/* Call Function */
	NULL,					/* user_data */
	&colors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"HELP!"};				/* *text */


static struct NewWindow HelpWind = {
   130,
   124,
   510,
   76,
   0,
   1,
   NULL,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH,
   0,0,
   " Install Audition 4 Help ", /** TITLE **/
   0,0,
   104,32,640,400,
   WBENCHSCREEN
   };

static char *entries[] =
	{
	"HELP INSTALLING AUDITION 4",
	"--------------------------",
	"Follow these step by step directions to install",
	"Audition 4 to your hard disk."
	" ",
	" ",
	"1) Boot your computer as you normally would. (not from",
	"   the Audition 4 disk)",
	" ",
	"2) Select the device you want to install Audition on.",
	"   This is done by selecting one on the items in the",
	"   list in the upper right hand corner.  You should",
	"   select the item that corresponds to the name under",
	"    your hard disk icon.  Select it now.",
	" ",
	"3) Select what you want to install on your hard drive",
	"   This is done by selecting one or more of the four",
	"   buttons on the left hand side of the window.",
	"   The first button 'Install Audition 4' should be",
	"   highlighted if you want to install the Audition 4",
	"   main program on your hard drive.  The next button,",
	"   'Install Samples' should be selected if you want to",
	"   copy the contents of the Sample Disk on you Hard",
	"   Drive.  The next button 'Install Libraries' should",
	"   be selected if you are installing Audition 4 for",
	"   the first time,  it installs 'req.library.'",
	" ",
	"4) Now insert your Audition 4 disk into any disk drive",
	"   and select the 'BEGIN INSTALL' gadget.  If you are",
	"   installing samples,  the computer will prompt you to",
	"   insert the samples disk.  Do so when this occurs.",
	" ",
	"5) If you feel you have made a mistake, hit the ",
	"   'Abort!!' button and try again.",
	" ",
	"**        To exit help select the 'HELP!' button       **"
	""
	};

static struct NewList HelpList = {
   	NULL,10,15,500,70,LIST_COOL|HIDE_GADGET,NULL,
	0,&colors,0,0,0, entries,NULL,0};


void OpenHelp(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	{
	if ((Hwind=OpenWindow(&HelpWind))==NULL)  {telluser("Can not open window!","",NOTICE_RETURN_NOW);return;}

	SetAPen(Hwind->RPort, 0);
	RectFill(Hwind->RPort, 2,11, Hwind->Width-3, Hwind->Height-2);

	SetAPen(Hwind->RPort, 1);
	RectFill(Hwind->RPort, 0 ,0, 1, Hwind->Height);

	SetAPen(Hwind->RPort, 2);
	RectFill(Hwind->RPort, Hwind->Width-2 ,0, Hwind->Width, Hwind->Height);
	RectFill(Hwind->RPort, 1 , Hwind->Height-1, Hwind->Width, Hwind->Height);

	create_gadget_chain(Hwind,&HelpList);
	show_gadget(HelpList.gad_ptr);
	refresh_gadget(HelpList.gad_ptr);

	}
else
	{
	if (Hwind)
		{
		hide_gadget(HelpList.gad_ptr);
		delete_gadget_chain(Hwind);
		CloseWindow(Hwind);
		Hwind=NULL;
		}
	}
}
/*****************************************************************/

