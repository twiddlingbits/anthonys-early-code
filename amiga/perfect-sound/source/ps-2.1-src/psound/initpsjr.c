
/******************************/
/*** get one byte from psjr ***/
/******************************/
/* NOW IN ASSEMBLER 
getpsjrbyte()
{
unsigned char *pb;
unsigned char bit,val;
short clk;

pb= 0xBFE101;

*pb=128+64+32;
val=0;

for (clk=0;clk<8;clk++) {
   *pb=*pb|2;
   *pb=(*pb)&(~2);
   bit=(*pb)&1;
   val=val|(bit<<(7-clk));
   }

*pb=*pb|4;
return(val);
}
*/

/**************************/
/*** set par: for psjr  ***/
/**************************/

initpsjr()
{

unsigned char *pb,*dir;

dir=0xBFE301;
pb= 0xBFE101;

*dir=0x0C;     /* bits 2 & 3 output */
*pb=8;        /* cs not active */
}

