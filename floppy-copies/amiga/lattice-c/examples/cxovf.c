#include "exec/types.h"
#include "intuition/intuition.h"

char *IntuitionBase;
extern char *_ProgramName;

struct IntuiText Text2 = {-1,-1,	/* pen numbers */
			     0,		/* draw mode */
			     14,14,	/* starting offsets */
			     NULL,	/* text attribute pointer */
			     NULL,
			     NULL };

struct IntuiText BodyText = {-1,-1,	/* pen numbers */
			     0,		/* draw mode */
			     4,4,	/* starting offsets */
			     NULL,	/* text attribute pointer */
			     "** Stack Overflow **",
			     &Text2 };

struct IntuiText ResponseText = {-1,-1,	/* pen numbers */
			     0,		/* draw mode */
			     4,4,	/* starting offsets */
			     NULL,	/* text attribute pointer */
			     "EXIT",
			     NULL };
cxovf()
{
IntuitionBase = OpenLibrary("intuition.library",0);
Text2.IText = _ProgramName;
AutoRequest(NULL,&BodyText,NULL,&ResponseText,0,0,250,40);
exit(20);
}
