/** convert samples per second to amiga hardware period value **/

toperiod(ss)
long ss;
{
unsigned long j;
j=35795465L/ss;
return(j/10);
}
