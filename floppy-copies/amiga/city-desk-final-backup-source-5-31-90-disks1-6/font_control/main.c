#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "desk.h"
#include "laser.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct ArpBase *ArpBase;

static struct Window *window;

main()

{
static struct NewWindow NewWindowStructure1 = {
   0,0,
   640,70,
   0,1,
   MENUPICK,
   WINDOWDRAG|ACTIVATE|WINDOWDEPTH|NOCAREREFRESH|WINDOWSIZING,
   0,    /** first gadget **/
   NULL,
   "City Desk 2.0 Laser Utility  © 1989 MicroSearch, Inc.",
   NULL,
   NULL,
   5,5,
   640,200,
   WBENCHSCREEN
};

extern struct Menu Titles[];    /* menus done with MenuEd, Shareware */
struct Gadget *g;
struct IntuiMessage *message,*GetMsg();
SHORT newsize;
ULONG class;
USHORT code,menunum,itemnum,subnum;
char fname[70],dir[160],path[200];
int can,id=1;
short ch,i,j;
FILE *fp;
char msg[60];

fname[0]=0;
dir[0]=0;
path[0]=0;

IntuitionBase = OpenLibrary("intuition.library",0);
if (IntuitionBase==0) {
   exit(0);
   }

GfxBase = OpenLibrary("graphics.library",0);
if (GfxBase==0) {
   CloseLibrary(IntuitionBase);
   exit(0);
   }

ArpBase = OpenLibrary("arp.library",0);
if (ArpBase==0) {
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   exit(0);
   }

fp = fopen("par:","w");
if (fp==0) {
   CloseLibrary(IntuitionBase);
   CloseLibrary(ArpBase);
   CloseLibrary(GfxBase);
   exit(10);
   }

window = OpenWindow(&NewWindowStructure1);
if (window==0) {
   fclose(fp);
   CloseLibrary(IntuitionBase);
   CloseLibrary(ArpBase);
   CloseLibrary(GfxBase);
   exit(10);
   }

SetMenuStrip(window,Titles);   /* turn on menus */

stat("City Desk 2.0 Laser Utility Module");
stat("Written by SunRize Industries.");
stat("");
stat("");

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g = message->IAddress;
      ReplyMsg(message);

      menunum=MENUNUM(code);
      itemnum=ITEMNUM(code);
      subnum=SUBNUM(code);

 
      if (class== MENUPICK && menunum==0 && itemnum==0) {
         can = get_fname(window, 0, "Font List To Download", fname, dir);
         if (can) {
            busy_ptr(window);
            strmfn(path,0,dir,fname,0);
            download_list(fp, path);
            stat("End of list.");
            normal_ptr(window);
            }
         }

      if (class== MENUPICK && menunum==0 && itemnum==1) {
         can = get_fname(window, 0, "Pick Font To Download", fname, dir);
         if (can) {
            id = getint(window->WScreen,id,0,200,"Select Font ID",150);
            strmfn(path,0,dir,fname,0);
            /* printf("path:%s id %d\n",path,id); */
            busy_ptr(window);
            sprintf(msg, "Downloading font '%s' with id %d.",path,id);
            stat(msg);
            download_font(fp,path,id++);
            normal_ptr(window);
            }
         }

      if (class== MENUPICK && menunum==0 && itemnum==2) {
         delete_all_fonts(fp);
         stat("Cleared all soft fonts from printer memory.");
         }

      if (class== MENUPICK && menunum==0 && itemnum==3) {
         reset_printer(fp);
         stat("Printer Reset.");
         }

      if (class== MENUPICK && menunum==0 && itemnum==4) {
         stat("Printing test page.");

         for (j=3; j < 26; j++) {
            ch = j*10;
            fprintf(fp,"\033*p%dX\033*p%dY%d",300,100*(j-1),ch);
            for (i=0; i < 10; i++) {
               ch = j*10+i;
               if (ch < 256)
                  fprintf(fp,"\033*p%dX%c",500+150*i,ch);
               }
            }
         fprintf(fp,"\015\012\012123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\015\012");
         fprintf(fp,"!@#$%^&*(abcdefghijklmnopqrstuvwxyz\015\012");
         fprintf(fp,"\014");
         }

      if (class== MENUPICK && menunum==0 && itemnum==5) {
         can = getint(window->WScreen,id,0,200,"Select Font ID",150);
         fprintf(fp,"\033(%dX",can);
         sprintf(msg,"Printer now using the font with id %d.",can);
         stat(msg);
         }

      if (class== MENUPICK && menunum==0 && itemnum==6) {
         stat("");
         stat("Laser Printer Utility Module Ver 1.01 for City Desk 2.0.");
         stat("Written by Anthony J. Wood of SunRize Industries.");
         stat("");
         }

      if (class== MENUPICK && menunum==0 && itemnum==7) {   /** QUIT **/
         fclose(fp);
         CloseWindow(window);
         CloseLibrary(IntuitionBase);
         CloseLibrary(ArpBase);
         CloseLibrary(GfxBase);
         exit(0);
         }

      if (class== MENUPICK && menunum==1 && itemnum==0) {
         create_hp();
         }

      if (class== MENUPICK && menunum==1 && itemnum==1) {
         create_ps();
         }
      }
   }
}


download_list(fp, path)

FILE *fp;
char *path;

{
int id;
FILE *list_fp;
char line[160];
char file[160];
char dummy[20];
char msg[50];

list_fp = fopen(path, "r");
if (list_fp==0) {
   sprintf(msg,"Can't open file %s.\n",path);
   stat(msg);
   return(FALSE);
   }

while (fgets(line, 160, list_fp)) {

switch (line[0]) {

   case 'R':
   case 'r':
      reset_printer(fp);
      delete_all_fonts(fp);
      stat("Cleared all printer fonts.");
      break;

   case 'D':
   case 'd':
   case 'X':      /** these extetended D cmds have extra info when used **/
   case 'x':      /** by the city desk main program **/
      sscanf(line, "%s %s %d",dummy, file, &id);
      sprintf(msg,"Downloading font '%s' with id %d.",file,id);
      stat(msg);
      download_font(fp, file, id);
      break;
      }
   }
fclose(list_fp);
}


stat(s)

char *s;

{
ScrollRaster(window->RPort, 0, 9, 5,10, window->Width-17, window->Height-2);
SetAPen(window->RPort, 1);
Move(window->RPort,5,window->Height-4);
Text(window->RPort,s,strlen(s));
}
