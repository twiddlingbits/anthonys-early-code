
/************************************************************/
/*** Take an error code and display the correct error msg ***/
/************************************************************/


#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

#if GERMAN

showstat(screen,val)

struct Screen *screen;
int val;

{
switch (val) {

case CANCEL:
case AOK:
      return(0);

case OUTOFMEM:
      telluser2(screen,"Kein Speicherplatz mehr.","",FALSE);
      break;
case FILENOTFOUND:
      telluser2(screen,"Datei nicht gefunden.",
          "Prüfen Sie den Dateinamen.",FALSE);
      break;
case CANTOPENFILE:
      telluser2(screen,"Ich kann die Datei nicht öffnen.",
        "Falscher Dateiname?",FALSE);
      break;
case DISKERROR:
      telluser2(screen,"Disk Lese/Schreib-fehler.","",FALSE);
      break;
case NOTIFF:
      telluser2(screen,"Die Datei ist nicht in IFF-Format","",FALSE);
      break;
case BADIFF:
      telluser2(screen,"Die Datei hat schlechtes IFF Format.","",FALSE);
      break;
case CANTOPENPRINTER:
      telluser2(screen,"Ich kann den Drucker nicht öffnen.","",FALSE);
      break;
case NOTCDPROJECT:
      telluser2(screen,"Die Datei ist nicht eine",
         "City Desk Projekt-Datei.",FALSE);
      break;
case BADFILEFORMAT:
      telluser2(screen,"Datei ist nicht im richtigen Format.","",FALSE);
      break;
case BADCD:
      telluser2(screen,"Schlechte City Desk Datei.","",FALSE);
      break;
case NOTPREFS:
      telluser2(screen,"Die Datei ist nicht eine City Desk",
       "Preferences Datei.",FALSE);
      break;
case TOOSMALL:
      telluser2(screen,"Notiz: Object ist zu",
         "klein, um dargestellt zu werden.",FALSE);
      break;
case NOTENUF:
      telluser2(screen,"Nicht genug Speicherplatz,",
         "um das Objekt zu drucken.",FALSE);
      break;
   }

}

#else

showstat(screen,val)

struct Screen *screen;
int val;

{
switch (val) {

case CANCEL:
case AOK:
      return(0);

case OUTOFMEM:
      telluser2(screen,"OUT OF MEMORY!","",FALSE);
      break;
case FILENOTFOUND:
      telluser2(screen,"File Not Found","Check the file name...",FALSE);
      break;
case CANTOPENFILE:
      telluser2(screen,"Can't Open File","Bad file name?",FALSE);
      break;
case DISKERROR:
      telluser2(screen,"Disk Read/Write ERROR!","",FALSE);
      break;
case NOTIFF:
      telluser2(screen,"File is not IFF format.","",FALSE);
      break;
case BADIFF:
      telluser2(screen,"File has bad IFF format.","",FALSE);
      break;
case CANTOPENPRINTER:
      telluser2(screen,"Can't Open Printer Device.","",FALSE);
      break;
case NOTCDPROJECT:
      telluser2(screen,"File is not a City Desk","Project File!",FALSE);
      break;
case BADFILEFORMAT:
      telluser2(screen,"File is not correct format.","",FALSE);
      break;
case BADCD:
      telluser2(screen,"Bad City Desk file.","",FALSE);
      break;
case NOTPREFS:
      telluser2(screen,"File is not a City Desk","Preferences file.",FALSE);
      break;
case TOOSMALL:
      telluser2(screen,"Note:","Object too small to display.",FALSE);
      break;
case NOTENUF:
      telluser2(screen,"Not Enough Memory to","Print Object.",FALSE);
      break;
case CANTPRINTTOFILE:
      telluser2(screen,"Can't print to a file when",
        "printer is set to preferences.",FALSE);
      break;
case CANTLINK:
      telluser2(screen,"This object is already linked.","",FALSE);
      break;
   }
}
#endif


