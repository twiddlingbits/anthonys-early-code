#include "exec/types.h"
#include "stdlib.h"
#include "desk.h"


assign_fonts(screen)

struct Screen *screen;

{
extern struct Window *clipboard;
static char fname[34]={0};
static char drawer[35]={0};
char cmd[80];
int cancel;


cancel=get_fname(clipboard,screen,"Pick Font Directory",fname,drawer);
if (cancel==NULL) return(CANCEL);

sprintf(cmd,"assign >nil: fonts: %s",drawer);
system(cmd);
return(AOK);
}

