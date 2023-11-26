#include "/include/psound.h"

/**@@ link_standard_modules
   ARGS:
      none
   RETURNS:
      none
   FUNCTION:
      Link's all the standard modules, like edit and open_list, to
      studioBase. Will call setdown() and exit(10) if there is an
      error.
@@**/

void link_standard_modules()
{
int Delay_open_window(), Delay_do_event(),Delay_close_window();
int Record_open_window(),Record_do_event(),Record_close_window();
int open_eq_window(),do_eq_event(),close_eq_window();
int open_MIX_window(),do_MIX_event(),close_MIX_window();
int open_VU_window(),do_VU_event(),close_VU_window();
int open_SMON_window(),do_SMON_event(),close_SMON_window();

int open_edit(), do_edit(), close_edit();
int open_open_list(),do_open_list(),close_open_list();
int open_smpte(),do_smpte();
int open_cue_list(),do_cue_list();
struct Module *add_module();

static struct NewModule open_list_data =
   {0,open_open_list,do_open_list,close_open_list,0,"OpenList",MOD_OPEN_AT_STARTUP};

static struct NewModule edit_data =
   {0,open_edit,do_edit,close_edit,0,"SoundEdit",MOD_CALL_ALL_WINDOWS};

static struct NewModule Delay_data =
   {0,Delay_open_window,Delay_do_event,Delay_close_window,0,"Delay",0};

static struct NewModule Record_data =
   {0,Record_open_window,Record_do_event,Record_close_window,0,"Record",0};

static struct NewModule eq_data =
   {0,open_eq_window,do_eq_event,close_eq_window,0,"Equalizer",0};

static struct NewModule MIX_data =
   {0,open_MIX_window,do_MIX_event,close_MIX_window,0,"Mixer",0};

static struct NewModule VU_data =
   {0,open_VU_window,do_VU_event,close_VU_window,0,"VU",0};

static struct NewModule SMON_data =
   {0,open_SMON_window,do_SMON_event,close_SMON_window,0,"SmpteMonitor",0};


if (add_module(&open_list_data)==0) {
   telluser("Can't add open_list","");
   setdown();
   exit(10);
   }

if (add_module(&edit_data)==0) {
   telluser("Can't add edit","");
   setdown();
   exit(10);
   }

if (add_module(&Delay_data)==0) {
   telluser("Can't add delay","");
   setdown();
   exit(10);
   }

if (add_module(&Record_data)==0) {
   telluser("Can't add record","");
   setdown();
   exit(10);
   }

if (add_module(&eq_data)==0) {
   telluser("Can't add eq","");
   setdown();
   exit(10);
   }

if (add_module(&SMON_data)==0) {
   telluser("Can't add SmpteMonitor","");
   setdown();
   exit(10);
   }

if (add_module(&MIX_data)==0) {
   telluser("Can't add Mixer","");
   setdown();
   exit(10);
   }

if (add_module(&VU_data)==0) {
   telluser("Can't add VU","");
   setdown();
   exit(10);
   }







/************
if (add_module(&smpte_data)==0) {
   telluser("Can't add smpte","");
   setdown();
   exit(10);
   }

if (add_module(&cue_list_data)==0) {
   telluser("Can't add open_list","");
   setdown();
   exit(10);
   }
****************/
}


