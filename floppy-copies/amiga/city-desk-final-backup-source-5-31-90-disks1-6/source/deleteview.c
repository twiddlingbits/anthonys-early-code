#include "exec/types.h"
#include "intuition/intuition.h"
#include "libraries/dosextens.h"
#include "desk.h"

deleteview(page)

struct pageinfo *page;

{
extern APTR temp_ptr;
struct Process *mproc;

if (page->view==0) return(0);

ClearMenuStrip(page->view->window);

if (page->pn!=CB) {
/*   page->view->window->UserPort=0; */
   FreeMem(page->view->window->Title,strlen(page->view->window->Title)+1);
/*   page->view->window->Title=NULL; */
   remove_clip(page->view);
   CloseWindowSafely(page->view->window);
   }
/* ModifyIDCMP(page->view->window,NULL); */

else {
   mproc = (struct Process *)FindTask(0L);
   mproc->pr_WindowPtr = temp_ptr;        /** restore original pointer **/
   CloseWindow(page->view->window);
   }

FreeMem(page->view, sizeof(struct pageview));
page->view=0;
return(0);
}
