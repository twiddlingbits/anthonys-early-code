#include <exec/types.h>
#include <exec/exec.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/clip.h>

char NameString[]="GadLibHandler";

#define GE_LBUTTONDOWN 1
#define GE_LBUTTONUP 2
#define GE_DELTAMOVE 3

struct GadEvent {
   struct Window *window;     /** active window **/
   short x;                   /** xpos or delta x */
   short y;
   short type;                /** BUTTONDOWN or BUTTONUP **/
   };

struct GadEvent event_buffer[512];
short read_ptr=0;    /** Next read by app prog comes from this index **/
short write_ptr=0;   /** Next event write by handler goes to this index **/
long gadlib_sigbit; /** signal this task by this bit when new events **/
struct Task *tc_gadlib;
short signal_ok = 1;

struct Library *LayersBase;

extern struct IntuitionBase *IntuitionBase;
extern void inhandler();

struct IOStdReq *inputReqBlk;
struct MsgPort *inputPort;
struct Interrupt *inputHandler;

void WaitForUser();
void rem_gadlib_handler();

VOID main()
{
int i,k;
short end_ptr;

#if 0
struct Layer *l;
struct Layer_Info *li;

IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",32L);

LayersBase = (struct Library *)OpenLibrary("layers.library",32L);
printf("lb=%x\n",LayersBase);

printf("s %x\n",IntuitionBase->ActiveWindow);
printf("li %x\n",(li=&(IntuitionBase->ActiveScreen->LayerInfo)));

/*LockLayerInfo(li);*/
printf("flags %x\n",li->Flags);
l = WhichLayer(li,100,100);
/*UnlockLayerInfo(li);*/
printf("layer %x win %x\n",l,l->Window);
CloseLibrary(LayersBase);
exit(0);
#endif

LayersBase = (struct Library *)OpenLibrary("layers.library",32L);

if (add_gadlib_handler()) {
/*  WaitForUser(); */
   for (k=0; k <30; k++) {
signal_ok = 1;
      Wait(1<<gadlib_sigbit);
      end_ptr = write_ptr;
      for (i=read_ptr; i !=end_ptr; i=(i+1)&511)
         printf("i: %d type %x x %d y %d win %x\n",i,
            event_buffer[i].type, event_buffer[i].x, event_buffer[i].y,
            event_buffer[i].window);
      read_ptr = end_ptr;
      }
   rem_gadlib_handler();
   }
}

/********************************************************************/

add_gadlib_handler()
{
IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0);

if ((gadlib_sigbit=AllocSignal(-1))!=-1) {
   if (inputPort=(struct MsgPort *)CreatePort(0,0)) {
      if (inputHandler=(struct Interrupt *)AllocMem(sizeof(struct Interrupt),MEMF_CLEAR|MEMF_PUBLIC)) {
         if (inputReqBlk=(struct IOStdReq *)CreateExtIO(inputPort,sizeof(struct IOStdReq))) {
            if (!OpenDevice("input.device",NULL,inputReqBlk,NULL)) {
               tc_gadlib=(struct Task *)FindTask(0);
               inputHandler->is_Code=inhandler;
               inputHandler->is_Data = NULL;
               inputHandler->is_Node.ln_Pri = 51;  /** IDCMP+1 **/
               inputHandler->is_Node.ln_Name=NameString;
               inputReqBlk->io_Data=(APTR)inputHandler;
               inputReqBlk->io_Command=IND_ADDHANDLER;
               DoIO(inputReqBlk);
               return(1);
               }
            DeleteExtIO(inputReqBlk);
            }
         FreeMem(inputHandler,sizeof(struct Interrupt));
         }
      DeletePort(inputPort);
      }
   FreeSignal(gadlib_sigbit);
   }
return(0);     /** an allocation error occured **/
}

/********************************************************************/

void rem_gadlib_handler()
{
inputReqBlk->io_Data=(APTR)inputHandler;
inputReqBlk->io_Command=IND_REMHANDLER;
DoIO(inputReqBlk);
CloseDevice(inputReqBlk);
DeleteExtIO(inputReqBlk);
FreeMem(inputHandler,sizeof(struct Interrupt));
DeletePort(inputPort);
FreeSignal(gadlib_sigbit);
if (IntuitionBase) 
   CloseLibrary(IntuitionBase);
}

/********************************************************************/

struct NewWindow mywin= {
   0,0,124,10,0,1,CLOSEWINDOW,
   WINDOWDRAG|WINDOWCLOSE|SIMPLE_REFRESH|NOCAREREFRESH,
   NULL,NULL,NameString,NULL,NULL,0,0,0,0,WBENCHSCREEN};

void WaitForUser()
{
struct Window *win;

if (IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0)) {
   if (win=(struct Window *)OpenWindow(&mywin)) {
      WaitPort(win->UserPort);
      ReplyMsg(GetMsg(win->UserPort));
      CloseWindow(win);
      }
   CloseLibrary(IntuitionBase);
   }
}
