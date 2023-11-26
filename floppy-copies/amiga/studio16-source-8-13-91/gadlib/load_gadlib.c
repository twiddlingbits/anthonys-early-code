/** lc -v gadlib.c **/

#include <exec/types.h>
#include <exec/exec.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"

static int gadlib_seg;
static struct MsgPort *gaddfltport;
static unsigned long my_seg[4];

/**************************************************************/

proc_gadlib(name,pri,code,stack)
char *name;
int pri;
APTR code;
int stack;
{
struct MsgPort *CreatePort();
struct MsgPort *CreateProc();
unsigned long word1;
unsigned long word2;

word1=(code>>16)&0xFFFF;
word2=(code&0xFFFF)<<16;
my_seg[0]=16;         /** segment length in bytes is 4 long words **/
my_seg[1]=0;          /** BPTR to next seg **/
my_seg[2]=0x4EF90000; /* JMP 0000 **/
my_seg[3]=0x00000000; /** part of jmp 00000**/

my_seg[2] |= word1;   /** make jmp code **/
my_seg[3] |= word2;

gaddfltport=CreateProc(name,pri,(&my_seg[1]>>2,stack);
if (gaddfltport==0) {
   printf("Can't create gadlibproc!\n");
   return(OUT_OF_MEMORY);
   }
}

/**************************************************************/

void delproc_gadlib()
{

/* printf("unload play\n"); */

if (gaddfltport)
   DeleteTask((int)gaddfltport - sizeof(struct Task));
}
