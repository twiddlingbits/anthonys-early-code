
#include <exec/types.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
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
if (strcmp(thing,"DISG")==0) return (DG);
if (strcmp(thing,"DISS")==0) return (DS);
if (strcmp(thing,"ACTIVATE")==0) return (ACTS);
if (strcmp(thing,"ENDS")==0) return (ENDS);
if (strcmp(thing,"INJECT")==0) return (INJECT);
if (strcmp(thing,"LOADSOUND")==0) return (LOADS);
if (strcmp(thing,"PLAYSOUND")==0) return (PLAYS);
if (strcmp(thing,"STOPSOUND")==0) return (STOPS);
if (strcmp(thing,"CLEARSOUND")==0) return (CLEARS);
if (strcmp(thing,"CHANGERMG")==0) return (CRMG);
if (strcmp(thing,"CHANGERMS")==0) return (CRMS);
if (strcmp(thing,"CHANGEOBJG")==0) return (COBJG);
if (strcmp(thing,"SLAVE")==0) return (SLAVE);
if (strcmp(thing,"UNSLAVE")==0) return (UNSLAVE);
if (strcmp(thing,"SPEAK")==0) return (SPEAK);
if (strcmp(thing,"OPEN")==0) return (OPEN);
if (strcmp(thing,"CLOSE")==0) return (CLOSEF);
if (strcmp(thing,"READ")==0) return (READ);
if (strcmp(thing,"WRITE")==0) return (WRITE);
if (strcmp(thing,"WAIT")==0) return (WAIT);
if (strcmp(thing,"MAIN")==0) return (MAIN);
if (strcmp(thing,"SDONE")==0) return (SDONE);
if (strcmp(thing,"NDONE")==0) return (NDONE);
if (strcmp(thing,"REDRAW")==0) return (REDRAW);
if (strcmp(thing,"RND")==0) return (RND);
if (strcmp(thing,"STOREGAME")==0) return (STOREG);
if (strcmp(thing,"LOADGAME")==0) return (LOADG);
if (strcmp(thing,"BACKGROUND")==0) return (BACKGROUND);
if (strcmp(thing,"GRAPHIC")==0) return (GRAPHIC);
if (strcmp(thing,"ENABLE")==0) return (ENABLE);
if (strcmp(thing,"DISABLE")==0) return (DISABLE);


return(ASSIGN);
}

