#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

/******************************/
/*** set up gadget structures */
/******************************/

makegadgets()

{
static struct Gadget gadget_s,gadget_e,gadget_p;
static struct PropInfo prop_s,prop_e,prop_p;
static struct Image image_s,image_e,image_p;

/* define a propertional gadget to use for curers */

gadget_s.NextGadget=NULL;
gadget_s.LeftEdge=50;
gadget_s.TopEdge=GSP;
gadget_s.Width=-90;
gadget_s.Height=10;
gadget_s.Flags=GADGHNONE|GRELWIDTH;
gadget_s.Activation=FOLLOWMOUSE|GADGIMMEDIATE;
gadget_s.GadgetType=PROPGADGET;
gadget_s.GadgetRender=(APTR)&image_s;
gadget_s.SelectRender=NULL;
gadget_s.GadgetText=NULL;
gadget_s.MutualExclude=NULL;
gadget_s.SpecialInfo=(APTR)&prop_s;
prop_s.Flags=FREEHORIZ|AUTOKNOB;
prop_s.HorizPot=0;
prop_s.HorizBody=0;
prop_s.VertBody=MAXBODY;

movmem(&gadget_s,&gadget_e,sizeof(struct Gadget));
movmem(&prop_s,&prop_e,sizeof(struct PropInfo));

movmem(&gadget_s,&gadget_p,sizeof(struct Gadget));
movmem(&prop_s,&prop_p,sizeof(struct PropInfo));

gadget_e.TopEdge=GEP;
gadget_e.GadgetRender=(APTR)&image_e;
gadget_e.SpecialInfo=(APTR)&prop_e;
gadget_s.NextGadget=&gadget_e;
gadget_e.NextGadget=&gadget_p;
gadget_p.NextGadget=NULL;
gadget_p.TopEdge=GPP;
gadget_p.GadgetRender=(APTR)&image_p;
gadget_p.SpecialInfo=(APTR)&prop_p;

gadget_e.GadgetID=END;
gadget_s.GadgetID=START;
gadget_p.GadgetID=POSN;
return((int)&gadget_s);   /*!!!!! This is a boo boo !! */
}

