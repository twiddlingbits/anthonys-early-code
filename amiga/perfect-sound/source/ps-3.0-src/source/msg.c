#include "exec/types.h"
#include "psound.h"

/**********************************/
/**  write msg to cmd line       **/
/**********************************/

void msg (Window,message)
char *message;
struct Window *Window;
{
int i,start;

start = strlen(message);
start = start*8 + 30;

putxy(Window,30,189,message,WHITE,BLACK);
for (i=start; i<610; i=i+8)
   putxy(Window,i ,189, " ",BLACK,BLACK);
}

