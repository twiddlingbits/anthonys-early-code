#include "exec/types.h"
#include "psound.h"

/**********************************/
/**  write msg to cmd line       **/
/**********************************/
msg (Window,message)
char *message;
struct Window *Window;
{
int i;

putxy(Window,23,190,message,RED,BLACK);
for (i=23+strlen(message)*8;i<610;i=i+8)
   putxy(Window,i,190," ",BLACK,BLACK);
}

