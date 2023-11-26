/** convert samples per second to amiga hardware period value **/

extern int vid_clock;

toperiod(ss)
long ss;
{
unsigned long j;
j=vid_clock/ss;
return(j);
}
