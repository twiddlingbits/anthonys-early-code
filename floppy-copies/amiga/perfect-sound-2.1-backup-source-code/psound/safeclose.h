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

#ifndef SAFECLOSE

#define SAFECLOSE


/* CloseWindowSafely(window), window is pointer to struct Window */

extern void CloseWindowSafely();

#endif

