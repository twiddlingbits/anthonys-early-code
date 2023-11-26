
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"


/*****************************/
/**    gettoken             **/
/*****************************/


gettoken(thing,terminator)
char *thing,terminator;
{
if (strcmp(thing,"IF")==0) return(IF);
if (strcmp(thing,"TY")==0) return(TYPE);
if (strcmp(thing,"NTY")==0) return(NUMTYPE);
if (strcmp(thing,"V")==0) return(VERB);
if (strcmp(thing,"N")==0) return(NOUN);
if (strcmp(thing,"A")==0) return(ADJ);
if (strcmp(thing,"P")==0) return(PREP);
if (strcmp(thing,"DONE")==0) return (DONE);
if (strcmp(thing,"ENDIF")==0) return (ENDIF);
if (strcmp(thing,"POEND")==0) return (POEND);
if (strcmp(thing,"NEND")==0) return (NEND);
if (strcmp(thing,"AEND")==0) return (ADJEND);
if (strcmp(thing,"VEND")==0) return (VEND);
if (strcmp(thing,"ELSE")==0) return (INSTEAD);
if (strcmp(thing,"END")==0) return (END);
if (strcmp(thing,"TOROOM")==0) return (TOROOM);
if (strcmp(thing,"CONNECT")==0) return (CONNECT);
if (strcmp(thing,"OR")==0) return (OR);
if (strcmp(thing,"AND")==0) return (AND);
if (strcmp(thing,"DESTROY")==0) return (DESTROY);
if (strcmp(thing,"RM")==0 && terminator =='+') return (RMPLUS);
if (strcmp(thing,"RM")==0 && terminator =='-') return (RMMINUS);
if (strcmp(thing,"IN")==0 && terminator =='+') return (INPLUS);
if (strcmp(thing,"IN")==0 && terminator =='-') return (INMINUS);
if (strcmp(thing,"LOADG")==0) return (LOADG);
if (strcmp(thing,"LOADS")==0) return (LOADS);
if (strcmp(thing,"LOADM")==0) return (LOADM);
if (strcmp(thing,"USEG")==0) return (USEG);
if (strcmp(thing,"USES")==0) return (USES);
if (strcmp(thing,"USEM")==0) return (USEM);
if (strcmp(thing,"MAIN")==0) return (MAIN);
if (strcmp(thing,"BACKGROUND")==0) return (BACKGROUND);
if (strcmp(thing,"GRAPHIC")==0) return (GRAPHIC);
if (strcmp(thing,"TEXT")==0) return (TEXT);

return(ASSIGN);
}

