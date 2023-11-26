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
int open_edit(), do_edit(), close_edit();
int open_open_list(),do_open_list(),close_open_list();
int open_smpte(),do_smpte();
int open_cue_list(),do_cue_list();
struct Module *add_module();

static struct NewModule open_list_data =
   {0,open_open_list,do_open_list,close_open_list,0,"OpenList",MOD_OPEN_AT_STARTUP};

static struct NewModule edit_data =
   {0,open_edit,do_edit,close_edit,0,"SoundEdit",0};

/************
static struct NewModule smpte_data =
   {0,open_smpte,do_smpte,0,0,"SMPTE",0};

static struct NewModule cue_list_data =
   {0,open_cue_list,do_cue_list,0,0,"CueList",0};
***************/

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


