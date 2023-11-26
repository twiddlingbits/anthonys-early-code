/** convert samples per second to amiga hardware period value **/

toperiod(ss)
int ss;
{
unsigned short j;
j=(int)(1.0/(float)ss*3579546.5);
return(j);
}
