

tr = CreateTimer(UNIT_MICROHZ);
if (tr==0)
   exit(0);

tr->tr_node.io_Command = TR_ADDREQUEST;
tr->tr_time.tv_secs = 0;
tr->tr_time.tv_micro = 33333;




CreateTimer(unit)
ULONG unit;
{
struct MsgPort *timerport;
struct timerequest *timermsg;
int error;

timerport = CreatePort(0,0);
if (timerport==NULL) {
   printf("Can't open port\n");
   return(NULL);
   }

timermsg = (struct timerequest *)
    CreateExtIO(timerport, sizeof(struct timerequest));
if (timermsg==NULL) {
   DeletePort(timerport);
   return(0);
   }
error = OpenDevice(TIMERNAME, unit, timermsg, 0);
if (error!=0) {
   DeleteTimer(timermsg);
   return(0);
   }
return(timermsg);
}

DeleteTimer(tr)
struct timerequest *tr;
{
struct MsgPort *tp;

if (tr!=0) {
   tp = tr->tr_node.io_Message.mn_ReplyPort;
   if (tp!=0) {
      DeletePort(tp);
   }
   CloseDevice(tr);
   DeleteExtIO(tr,sizeof(struct timerequest));
   }
}

