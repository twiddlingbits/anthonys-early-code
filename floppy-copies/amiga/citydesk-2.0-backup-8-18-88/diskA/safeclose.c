/* CloseWindowSafely()
*       This module should be used whenever you are sharing an IDCMP
* message port with more than one window.  Rather than calling CloseWindow(),
* you should use CloseWindowSafely().  This will keep Intuition from
* Guru Meditation, and thus is considered a good thing.  You should still
* use CloseWindow for the very last window you close.
*       The reason this is needed, is because Intuition will re-claim
* any outstanding messages for a window when the window is closed. But...
* it can't take them out of your message port. Thus, you will receive
* invalid messages and bad things will happen.  Very bad things.
*       This code is a slightly condensed version of the same routine
* written by Neil Katin of Amiga for the April '86 Commodore Developers
* Newsletter, Amiga Mail (tm).
*/

/*
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/ports.h>
#include <intuition/intuition.h>
#include <functions.h>
*/
#include "standard.h"

void CloseWindowSafely( p_wind )
   struct Window   *p_wind;
   {
   register struct IntuiMessage    *msg;
   register struct IntuiMessage    *succ;
   register struct Window          *win = p_wind;
   register struct MsgPort         *mp = (struct MsgPort *)win->UserPort;

   Forbid();

   msg = (struct IntuiMessage *)mp->mp_MsgList.lh_Head;

   while ( succ=(struct IntuiMessage *)msg->ExecMessage.mn_Node.ln_Succ )
      {
      if ( msg->IDCMPWindow == win )
         {
         Remove ( msg );
         ReplyMsg( msg );
         }
      msg = succ;
      }
   win->UserPort = NULL;
   ModifyIDCMP( win, 0L );
   Permit();
   CloseWindow( win );
   }


