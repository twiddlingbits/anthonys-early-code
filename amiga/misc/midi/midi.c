#include "exec/types.h"
#include "exec/exec.h"
#include "hardware/intbits.h"

/*** Remove the printf()s before installing ***/

#define BUFSIZE 512        /** must change in handler.asm also! **/

static struct Interrupt *inter,*old;
char midi_buffer[BUFSIZE];
short current_index = 0;
short last_index    = 0;
short num_bytes     = 0;

midi_init()
{
extern void midi_handler();

inter=AllocMem(sizeof(struct Interrupt),MEMF_PUBLIC);
if (inter==0) {
   printf("not enough memory for interupt server.");
   return(FALSE);
   }

inter->is_Node.ln_Type=NT_INTERRUPT;
inter->is_Node.ln_Pri=0;
inter->is_Node.ln_Name="midi_handler";
inter->is_Data=midi_buffer;
inter->is_Code=midi_handler;


if ((old=SetIntVector(INTB_RBF,inter))!=0) {
   printf("we just replaced the %s interrupt handler\n",old->is_Node.ln_Name);
   return(TRUE);
   }
return(FALSE);
}

midi_end()
{
if ((old=SetIntVector(INTB_RBF,old))!=0) {
   printf("exit: replaced the %s interrupt handler",old->is_Node.ln_Name);
   FreeMem(inter,sizeof(struct Interrupt));
   return(TRUE);
   }
return(FALSE);
}

midi_check()
{
return(num_bytes);
}

midi_grab(data)
char *data;
{
short i,j,k,l;

j = 0;
k = num_bytes;

if (k > BUFSIZE)
   l = BUFSIZE;
else
   l = k;

for(i=0; i < l; i++) {
   data[j++] = midi_buffer[last_index];
   last_index = (last_index+1)&(BUFSIZE-1);
   }

Disable();
num_bytes = num_bytes - k;
Enable();
return(k);
}

