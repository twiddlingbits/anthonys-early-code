#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

deleteview(page)

struct pageinfo *page;

{
if (page->view==0) return(0);

if (page->pn!=CB) {
/*   page->view->window->UserPort=0; */
   FreeMem(page->view->window->Title,strlen(page->view->window->Title)+1);
/*   page->view->window->Title=NULL; */
   remove_clip(page->view);
   CloseWindowSafely(page->view->window);
   }
/* ModifyIDCMP(page->view->window,NULL); */

else {
   CloseWindow(page->view->window);
   }

FreeMem(page->view, sizeof(struct pageview));
page->view=0;
return(0);
}
