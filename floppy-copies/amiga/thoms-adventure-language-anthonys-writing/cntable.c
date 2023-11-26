
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
if (strcmp(thing,"NTY")==0) return(NTYPE);
if (strcmp(thing,"V")==0) return(VERB);
if (strcmp(thing,"N")==0) return(NOUN);
if (strcmp(thing,"A")==0) return(ADJ);
if (strcmp(thing,"P")==0) return(PREP);
if (strcmp(thing,"DONE")==0) return (DONE);
if (strcmp(thing,"ENDIF")==0) return (ENDIF);
if (strcmp(thing,"NEND")==0) return (NEND);
if (strcmp(thing,"AEND")==0) return (ADJEND);
if (strcmp(thing,"VEND")==0) return (VEND);
if (strcmp(thing,"INSTEAD")==0) return (INSTEAD);
if (strcmp(thing,"END")==0) return (END);
if (strcmp(thing,"TOROOM")==0) return (TOROOM);
if (strcmp(thing,"CONNECT")==0) return (CONNECT);
if (strcmp(thing,"RM")==0 && terminator =='+') return (RMPLUS);
if (strcmp(thing,"RM")==0 && terminator =='-') return (RMMINUS);
if (strcmp(thing,"IN")==0 && terminator =='+') return (INPLUS);
if (strcmp(thing,"IN")==0 && terminator =='-') return (INMINUS);
return(ASSIGN);
}

