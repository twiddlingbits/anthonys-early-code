/** lc -v main.c **/

#include "exec/types.h"
#include "exec/exec.h"
#include <intuition/intuition.h>
#include "exec/interrupts.h"
#include <hardware/intbits.h>

#define BLACK 0
#define WHITE 1

#include "setup.pwc"

#define BIT (1<<15)

struct Interrupt *myint;
unsigned long vcount;
unsigned long bufmem[256];
unsigned long *cpubuf=bufmem;
unsigned long dataptr=0;
unsigned long readptr=0;
unsigned long latch_size = 10;
unsigned long cpu;
struct Window *window;
struct Task *tc_cpucount;
struct Task *tc_dograph;
long graph_sigbit;

void setup();
void setdown();
void openlibraries();
void closelibraries();
void FreeInt();
struct Interupt *MakeInt();
void RemInt();
void AddInt();
__saveds void cpucount();

main()
{
unsigned long mask;

tc_dograph=(struct Task *)FindTask(0);

setup();
vcount=0;
cpu=0;

SetBPen(window->RPort,0);		/** used by scrollrast **/
SetAPen(window->RPort,1);		/** used by writepixel **/

while (TRUE) {
	mask=Wait( (1<<graph_sigbit) | (1<<window->UserPort->mp_SigBit));
	if (mask & (1<<window->UserPort->mp_SigBit)) {
	   setdown();
		}

	if (mask & (1<<graph_sigbit)) {
		while (readptr!=dataptr) {
   		WritePixel(window->RPort,4,15+cpubuf[readptr]/3000);
			ScrollRaster(window->RPort, -1, 0, 4, 11,window->Width-4,window->Height-11);
/*				printf("%d\n",cpubuf[readptr]/3000); */
			readptr=(readptr+1)&0xFF;
			}
		}
	}
}

/**************************************************************/

__saveds void cpucount()
{
while (TRUE)
	cpu++;
}

/**************************************************************/

void AddInt()
{
extern void VertCount();
extern struct Interrupt *MakeInt();

myint = MakeInt("pm.vertb",127,VertCount,&vcount);
if (myint==0) {
	setdown();
	}
AddIntServer(INTB_VERTB, myint); 
}
/*****************************************************************/

void RemInt()
{
void FreeInt();

RemIntServer(INTB_VERTB,myint);
FreeInt(myint);
}
/*****************************************************************/

struct Interupt *MakeInt(name,pri,code,data)
char *name;
int pri;
void (*code)();
APTR data;
{
struct Interrupt *intr;

intr=(struct Interrupt *)AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
if (intr==0) return(0);

intr->is_Node.ln_Pri=pri;
intr->is_Node.ln_Type=NT_INTERRUPT;
intr->is_Node.ln_Name=name;
intr->is_Data = data;
intr->is_Code = code;

return(intr);
}
/*****************************************************************/

void FreeInt(intr)
struct Interrupt *intr;
{
if (intr==0) return;
if (intr->is_Node.ln_Type != NT_INTERRUPT) return;
intr->is_Node.ln_Type=0;
FreeMem(intr,sizeof(*intr));
}

/**************************************************************/

void setup()
{
void setup_defaults();
void setdown();
struct Screen *OpenScreen();
struct Window *OpenWindow();

openlibraries();

window = OpenWindow(&NewWindow);
if (window==0) {
   printf("can't open back window\n");
   setdown();
   }

if (graph_sigbit=AllocSignal(-1)==-1)  {
	setdown();
	}

tc_cpucount=(struct Task *)CreateTask("ticktock",-128,cpucount,512);

if (tc_cpucount==0)
	setdown();
AddInt();
}

void setdown()
{
if (myint)
   RemInt();

if (tc_cpucount)
	DeleteTask(tc_cpucount);

if (window)
   CloseWindow(window);

if (graph_sigbit)
   FreeSignal(graph_sigbit);

closelibraries();
exit(0);
}

/**************************************************************/

/** do not use -b0 when compiling this module **/

struct GfxBase near *GfxBase;
struct ArpBase near *ArpBase;
struct IntuitionBase near *IntuitionBase;

void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   printf("Use Workbench 1.2 or greater.");
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!");
   closelibraries();
   exit(10);
   }
}

/**************************************************************/

void closelibraries()
{
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);
}


	