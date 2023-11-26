#include "exec/types.h"
#include "intuition/intuition.h"
#include "devices/serial.h"
#include "stdio.h"
#include "desk.h"

struct IOExtSer *sw;
/* struct IOExtSer *sr; */
LONG dummy;       /** serial data dumped here */

/***************************************/
/** Open the PostScript serial device **/
/***************************************/

openpost()
{
struct IOExtSer *openser_r();
/* struct IOExtSer *openser_w(); */
void free_ser_req();
void setserparm();
sw=openser_r();
if (sw==0) {
   return(CANTOPENFILE);
   }

setserparm();

/*
sw=openser_w(sr);
if (sw==0) {
   CloseDevice(sr);
   free_ser_req(sr);
   return(CANTOPENFILE);
   }  */

return(AOK);
}

/****************************************/
/** Close the PostScript Serial device **/
/****************************************/

void closepost()

{
void free_ser_req();
/* printf("B4 final flush:\n"); */
/* my_flush(); */      /** flush anything in the read buffer **/
/* printf("After final flush:\n"); */
CloseDevice(sw);
/* free_ser_req(sr); */
free_ser_req(sw);
}


/*******************************/
/*** Open the serial device  ***/
/*******************************/

struct IOExtSer *openser_r()

{
LONG error;
struct MsgPort *rp,*CreatePort();
struct IOExtSer *serreq;

rp=CreatePort(NULL,0);

if (rp==NULL)
   return(NULL);

serreq=(struct IOExtSer *)CreateExtIO(rp,sizeof(struct IOExtSer));

if (serreq==NULL) {
   DeletePort(rp);
   return(NULL);
   }


serreq->io_SerFlags=0;

error=OpenDevice("serial.device",0,serreq,0);
if (error!=0) {
   DeleteExtIO(serreq);
   DeletePort(rp);
   return(NULL);
   }

return(serreq);
}

#if 0
/***************************************************/
/** Given serial read req, make a write serwrtreq **/
/***************************************************/


struct IOExtSer *openser_w(sr)

struct IOExtSer *sr;

{
struct MsgPort *wp,*CreatePort();
struct IOExtSer *serwrt;

wp=CreatePort("PostScript_W",0);

if (wp==0)
   return(0);

serwrt=(struct IOExtSer *)CreateExtIO(wp,sizeof(struct IOExtSer));
if (serwrt==0) {
   DeletePort(wp);
   return(0);
   }
*serwrt = *sr;
serwrt->IOSer.io_Message.mn_ReplyPort = wp;
return(serwrt);
}

#endif

/****************************/
/*** Free serial req data ***/
/****************************/

void free_ser_req(req)

struct IOExtSer *req;

{
struct MsgPort *p;

p = req->IOSer.io_Message.mn_ReplyPort;
DeleteExtIO(req);
DeletePort(p);
}

#if 0
/*******************************************/
/** Wait for a character from serial port **/
/*******************************************/

getcharser()

{
char c;

sr->IOSer.io_Data = (APTR)&c;
sr->IOSer.io_Length=1;
sr->IOSer.io_Command=CMD_READ;
DoIO(sr);
return(c);
}
#endif

/**************************************/
/*** send a string to serial device ***/
/**************************************/

void poststr(s)

char *s;

{
extern FILE *fp;

if (fp!=0) {
   fputs(s,fp);
   fputc('\n',fp);
   }

else {
   sw->IOSer.io_Data = (APTR)s;
   sw->IOSer.io_Length=-1;
   sw->IOSer.io_Command=CMD_WRITE;
   DoIO(sw);
   sw->IOSer.io_Data = (APTR)"\n";
   sw->IOSer.io_Length=1;
   sw->IOSer.io_Command=CMD_WRITE;
   DoIO(sw);
/*   my_flush(); */
   }
}

/***********************************************************/
/*** Attempt to empty everything from the recieve buffer ***/
/***********************************************************/
#if 0
my_flush()

{
/* short i=0; */
short done;
/* short x; */

struct MsgPort *rp;
rp = sr->IOSer.io_Message.mn_ReplyPort;

done = FALSE;
do {
   sr->IOSer.io_Data = (APTR)&dummy;
   sr->IOSer.io_Length= 1;
   sr->IOSer.io_Error= 0;
   sr->IOSer.io_Command=CMD_READ;
   sr->IOSer.io_Flags = IOF_QUICK;     /** bit stays set when i/o fast **/
   BeginIO(sr);
/*   i++; */
   if ( ((sr->IOSer.io_Flags) & IOF_QUICK)==0) {
      done = TRUE;

/*      for (x=0; x<250; x++) ;
      done=GetMsg(rp);
      if (done == 0) {
      for (x=0; x<600; x++) ;
         done=GetMsg(rp);
         }  */
      }
   pb();
   } while (!done);

   if (!CheckIO(sr)) {
      AbortIO(sr);
      WaitIO(sr);
      }
   pb();
/*   printf(":%d:",i); */
}

pb()
{
pcar( (dummy>>24) &0xFF);
pcar( (dummy>>16) &0xFF);
pcar( (dummy>>8)  &0xFF);
pcar(dummy&0xFF);
dummy = 0;
}

/****************************************************/
/** Given a character, check for xoff and print it **/
/****************************************************/

pcar(c)
UBYTE c;
{
if (c==0)
   return;

if (c==19) {      /** control S, xoff **/
   Delay (100);      /** Kludge --- wait 2 seconds (never happens)**/
   return;
   }

/* putchar(c&0x7F); */
}
#endif

void poststr2(s)

char *s;

{

extern FILE *fp;

if (fp!=0) {
   fputs(s,fp);
   }

else {
   sw->IOSer.io_Data = (APTR)s;
   sw->IOSer.io_Length=-1;
   sw->IOSer.io_Command=CMD_WRITE;
   DoIO(sw);
/*   my_flush(); */
   }

}

/*********************************************************/
/** set serial paramaters to laserprinter values (9600) **/
/*********************************************************/

void setserparm()

{

sw->io_Baud=9600;
sw->io_StopBits=1;
sw->io_ReadLen=8;
sw->io_WriteLen=8;
sw->io_RBufLen=512;
sw->io_CtlChar=0x11130000;
/* sr->io_SerFlags=SERB_RAD_BOOGIE; */
sw->io_SerFlags=0;

sw->IOSer.io_Command=SDCMD_SETPARAMS;
DoIO(sw);
}

