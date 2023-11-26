#include "exec/types.h"
#include "intuition/intuition.h"
#include "fcntl.h"
#include "desk.h"
#include "text.h"

extern COORD x_gridsize;
extern COORD y_gridsize;
extern SHORT snaptogrid;
extern SHORT displaygrid;
extern SHORT displayguides;
extern SHORT screensize;         /* non interlaced */
extern SHORT scribble_style;
extern SHORT wp_style;
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT snaptotop;
extern SHORT autoflow;
extern SHORT flow_warnings;
extern SHORT usedefault;
extern SHORT default_mag;
extern SHORT autohyphenate;
extern SHORT irregular;
extern struct text_state defaultstate;
extern struct rule       defaultrule;
extern struct object     default_object;
extern struct printerprefs printerprefs;
extern char default_font_name[40];
extern char default_cmd_list[256];

extern struct Window *clipboard;

/* #define PREFSID 0xC4E356AE ver 1.0 */
/* #define PREFSID 0xC4E356AD ver 1.1 */
#define PREFSID 0xC4E356A6

static char pref_fname[34]={0};     /** preferences save and load file */
static char pref_drawer[35]={0};    /** name and subdirectroy          */

/*************************************/
/** get filename and call loadprefs **/
/*************************************/


loadpreffile()
{
char path[80];
int cancel;

#if GERMAN
cancel=get_fname(clipboard,clipboard->WScreen,"Preferences einlesen",
        pref_fname,pref_drawer);
#else
cancel=get_fname(clipboard,clipboard->WScreen,"Load Preferences",
        pref_fname,pref_drawer);
#endif

if (cancel==NULL)
   return(CANCEL);

path[0]=0;
add(path,pref_drawer);  /* connect drawer and filename into a path */
add(path,pref_fname);
return(loadprefs(path,FALSE));
}


/*************************************/
/** get filename and call aveprefs **/
/*************************************/


savepreffile()
{
char path[80];
int cancel;


#if GERMAN
cancel=get_fname(clipboard,clipboard->WScreen,"Preferences sicheren",
        pref_fname,pref_drawer);
#else
cancel=get_fname(clipboard,clipboard->WScreen,"Save Preferences",
        pref_fname,pref_drawer);
#endif

if (cancel==NULL)
   return(CANCEL);

path[0]=0;
add(path,pref_drawer);  /* connect drawer and filename into a path */
add(path,pref_fname);
return(saveprefs(path));
}

/**************************************/
/** given a filename, save the prefs **/
/**************************************/

saveprefs(fname)

char *fname;

{
int fh;
LONG id=PREFSID;

fh=open(fname,O_CREAT,0);

if (fh==-1)
   return(CANTOPENFILE);

write(fh,&id,4);        /** write file id so read can check **/

write(fh,&irregular,sizeof(irregular));
write(fh,&autohyphenate,sizeof(autohyphenate));
write(fh,&default_mag,sizeof(default_mag));
write(fh,&flow_warnings,sizeof(flow_warnings));
write(fh,&usedefault,sizeof(usedefault));
write(fh,&snaptotop,sizeof(snaptotop));
write(fh,&autoflow,sizeof(autoflow));
write(fh,&x_gridsize,sizeof(x_gridsize));
write(fh,&y_gridsize,sizeof(y_gridsize));
write(fh,&snaptogrid,sizeof(snaptogrid));
write(fh,&displaygrid,sizeof(displaygrid));
write(fh,&displayguides,sizeof(displayguides));
write(fh,&screensize,sizeof(screensize));
write(fh,&scribble_style,sizeof(scribble_style));
write(fh,&wp_style,sizeof(wp_style));
write(fh,&strip_spaces,sizeof(strip_spaces));
write(fh,&double_newlines,sizeof(double_newlines));
write(fh,&ansi_escapes,sizeof(ansi_escapes));
write(fh,&real_ascii,sizeof(real_ascii));
write(fh,&defaultstate,sizeof(defaultstate));
write(fh,default_font_name,40);
write(fh,default_cmd_list,256);
/* write(fh,&defaultpage,sizeof(defaultpage)); */
write(fh,&defaultrule,sizeof(defaultrule));
write(fh,&default_object,sizeof(default_object));
if (write(fh,&printerprefs,sizeof(printerprefs))!=sizeof(printerprefs)) {
   close(fh);
   return(DISKERROR);
   }

save_default(fh);    /** save default even & odd pages **/
close(fh);
return(AOK);
}

/**************************************/
/** given a filename, load the prefs **/
/**************************************/

loadprefs(fname,load_them_fonts)

char *fname;
int load_them_fonts;

{
int fh;
LONG id;
fh=open(fname,O_RDONLY,0);

if (fh==-1)
   return(FILENOTFOUND);

read(fh,&id,4);        /** read file id so read can check **/
if (id!=PREFSID) {
   close(fh);
   return(NOTPREFS);
   }

read(fh,&irregular,sizeof(irregular));
read(fh,&autohyphenate,sizeof(autohyphenate));
read(fh,&default_mag,sizeof(default_mag));
read(fh,&flow_warnings,sizeof(flow_warnings));
read(fh,&usedefault,sizeof(usedefault));
read(fh,&snaptotop,sizeof(snaptotop));
read(fh,&autoflow,sizeof(autoflow));
read(fh,&x_gridsize,sizeof(x_gridsize));
read(fh,&y_gridsize,sizeof(y_gridsize));
read(fh,&snaptogrid,sizeof(snaptogrid));
read(fh,&displaygrid,sizeof(displaygrid));
read(fh,&displayguides,sizeof(displayguides));
read(fh,&screensize,sizeof(screensize));
read(fh,&scribble_style,sizeof(scribble_style));
read(fh,&wp_style,sizeof(wp_style));
read(fh,&strip_spaces,sizeof(strip_spaces));
read(fh,&double_newlines,sizeof(double_newlines));
read(fh,&ansi_escapes,sizeof(ansi_escapes));
read(fh,&real_ascii,sizeof(real_ascii));

read(fh,&defaultstate,sizeof(defaultstate));
read(fh,default_font_name,40);
read(fh,default_cmd_list,256);

/* read(fh,&defaultpage,sizeof(defaultpage)); */
read(fh,&defaultrule,sizeof(defaultrule));
read(fh,&default_object,sizeof(default_object));
default_object.data = (APTR)&defaultrule;
if (read(fh,&printerprefs,sizeof(printerprefs))!=sizeof(printerprefs)) {
   close(fh);
   return(DISKERROR);
   }

/** since we now what kind of printer (its int printerprefs) load **/
/** laser fonts or set preferences printer dpi **/

if (load_them_fonts) {
   if (loadfonts(0)!=AOK) {
      close_fonts();
      closelibraries();
      exit(10);
      }
setdeffont(0);     /** alter defaultstate ptrs **/
   }

id=load_default(fh);
close(fh);
return(id);
}


