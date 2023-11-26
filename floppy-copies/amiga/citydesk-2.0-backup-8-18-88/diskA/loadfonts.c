#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"

/***********************************************************/
/*** Looks at printer type and then either loads fonts or **/
/*** opens preferences printer and sets xinch, yinch      **/
/***********************************************************/

#define DIRBUFSIZE 2048
#define NUMFILES 60

loadfonts(screen)
struct Screen *screen;
{
extern struct printerprefs printerprefs;
extern struct Window *clipboard;
extern ULONG ypix;
extern UWORD yinch,xinch;
extern char *pname;
extern char printername[60];
extern SHORT do_trans;     /** If true, this means turn on translation */
extern UBYTE *translate; /** tables. False means don't translate     */
extern UBYTE hp_tab[],no_tab[],ps_tab[];


char buffer[65];
int i,numfiles;
char *file_list;
char *pointers[NUMFILES];

closelaserfonts();

if (printerprefs.printertype==PREFERENCES) {
   i=OpenPrinter();
   if (i==0) {
#if GERMAN
      telluser2(screen,"Ich kann nicht 'Printer","Device' Öffnen!",FALSE);
#else
      telluser2(screen,"Can't Open Printer","Device!",FALSE);
#endif
      return(CANCEL);
      }

   sprintf(printername,"%s",pname);

   ClosePrinter(i);
   if (ypix==0) ypix=yinch*11;   /* if room for full page, use it */
   if (xinch==0) {
#if GERMAN
      sprintf(buffer,"'%s' ist ein falsche Drucker.",pname);
      telluser2(screen,buffer,"Preferences wechseln.",FALSE);
#else
      sprintf(buffer,"'%s' is an invalid printer.",pname);
      telluser2(screen,buffer,"Change with Preferences.",FALSE);
#endif
      printerprefs.printertype=HPLASER;
      setdeffont(screen);
      return(CANCEL);
      }
   setdeffont(screen);
   return(AOK);
   }

newpointer(screen,BUSY);

file_list = (char *)AllocMem(DIRBUFSIZE,0);
if (file_list == 0) {
   showstat(screen,OUTOFMEM);
   numfiles=0;
   }


if (printerprefs.printertype==HPLASER) {
   if (file_list)
      numfiles = getfnl("laserfonts/#?.hp",file_list,DIRBUFSIZE,0);
   strcpy(printername,"HP LaserJet");
   if (do_trans) {
      translate = hp_tab;   /** see translation.c **/
      }
   else  {
      translate = no_tab;
      }
   }

if (printerprefs.printertype==POSTSCRIPT) {
   if (file_list)
      numfiles = getfnl("laserfonts/#?.ps",file_list,DIRBUFSIZE,0);
   strcpy(printername,"PostScript");
   if (do_trans) {
      translate = ps_tab;   /** see translation.c **/
      }
   else  {
      translate = no_tab;
      }
   }

if (numfiles > NUMFILES) {
#if GERMAN
   telluser2(screen,"Warnung:","zu viel Schriftartdateien.");
#else
   telluser2(screen,"Warning:","Too many font files.");
#endif
   }

if (numfiles == 0) {
   i=CANCEL;
   }
else {
   numfiles=strbpl(pointers,NUMFILES,file_list); /** build pointer list **/
   i=AOK;
   while (i==AOK && numfiles-- > 0) 
      i=openlaserfonts(pointers[numfiles]);
   }

if (file_list)
   FreeMem(file_list,DIRBUFSIZE);

if (i!=AOK) {

#if GERMAN
   telluser2(screen,"Warnung:","Ich kann Laserfonts nicht Öffnen!",FALSE);
#else
   telluser2(screen,"Warning:","Can't Open laserfont tables!",FALSE);
#endif
   }

setdeffont(screen);
newpointer(screen,NORMAL);
return(AOK);
}

