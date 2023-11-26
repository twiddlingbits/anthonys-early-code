#include "exec/types.h"
#include "intuition/intuition.h"

/** returns TRUE if this is the last message, else returns FALSE **/

int last_message(message)

struct IntuiMessage *message;

{
if (message->ExecMessage.mn_Node.ln_Succ->ln_Succ)
   return(FALSE);

return(TRUE);
}

