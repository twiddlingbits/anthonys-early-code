#ifndef GRAPHICS_GRAPHINT_H
#define GRAPHICS_GRAPHINT_H
/********************************************************************/
/*               Commodore-Amiga, Inc.                              */
/*               graphint.h                                         */
/********************************************************************/

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif

/* structure used by AddTOFTask */
struct Isrvstr
{
    struct Node is_Node;
    struct Isrvstr *Iptr;   /* passed to srvr by os */
    int (*code)();
    int (*ccode)();
    int Carg;
};

#endif
