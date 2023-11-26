#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

void BorderLo();
void BorderHi();
void GadgetRefresh();

/*********************************************************************/
/*  Name:   OpenWind(wind)
/*  Purpose:   To Open a window and set up the gadgets in the list.
/*  Variables: wind - &NewWindow 
/*
/*
/*********************************************************************/
extern struct studioBase *studioBase;
void closelibraries();

struct Window *OpenWind(wind)
struct NewWindow *wind;
{
struct Window *wp;

wind->Screen=studioBase->screen;
wp=(struct Window *)OpenWindow(wind);
if (wp==0) {
   telluser("OpenWind: can't open window",0,0);
/*   setdown();*/
   exit(10);
   }

wp->UserData=(BYTE *)wind;

SetAPen(wp->RPort, BROWN);
RectFill(wp->RPort, 2,11, wind->Width-3,wind->Height-2);

GadgetRefresh(wp);
return(wp);
}

/*********************************************************************/
/*  Name:   GadgetRefresh(wind)
/*  Purpose:   Refresh The Gadgets attached to the 'wind' structure
/*  Variables: wind - struct *Window 
/*
/*
/*********************************************************************/
void GadgetRefresh(wind)
struct Window *wind;
{
struct Gadget *gad;
struct NewWindow *NewWind=(struct NewWindow *)wind->UserData; 

gad=NewWind->FirstGadget;

while (gad!=NULL)
   {
   BorderHi(gad,wind);
   gad=gad->NextGadget;
   }

RefreshGadgets(NewWind->FirstGadget,wind,0);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static SHORT pts1[] = {2,1,  2,10, 3,9, 3,1, 9,1};
static SHORT pts2[] = {3,10, 9,10, 9,1, 8,2, 8,10};
static struct Border bardHI = {0,0, DARK_BROWN,DARK_BROWN,JAM2, 5,pts2, NULL};  
static struct Border brdHI = {0,0, WHITE,WHITE,JAM2, 5,pts1, &bardHI};  
static struct Border bardLO = {0,0, WHITE,WHITE,JAM2, 5,pts2, NULL};  
static struct Border brdLO = {0,0, DARK_BROWN,DARK_BROWN,JAM2, 5,pts1, &bardLO};  

/*********************************************************************/
/*  Name:   BorderHi(gad)
/*  Purpose:   Draws Border around gadget
/*  Variables: gad - struct *Gadget
/*
/*  ©1990 Todd Modjeski
/*********************************************************************/
void BorderHi(gad,windo)
struct Gadget *gad;
struct Window *windo;
{
pts1[3]=gad->Height;pts1[5]=gad->Height-1;
pts1[8]=gad->Width;
pts2[1]=gad->Height;pts2[3]=gad->Height;pts2[9]=gad->Height;
pts2[2]=gad->Width+1;pts2[4]=gad->Width+1;pts2[6]=gad->Width;pts2[8]=gad->Width;

DrawBorder(windo->RPort,&brdHI,gad->LeftEdge-2,gad->TopEdge-1);
SetAPen(windo->RPort, LIGHT_BROWN);
RectFill(windo->RPort, gad->LeftEdge+2,gad->TopEdge+1,gad->LeftEdge+gad->Width-3,gad->TopEdge+gad->Height-2);

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*********************************************************************/
/*  Name:   
/*  Purpose:   
/*  Variables:  Not Convertered
/*
/*
/*********************************************************************/
void BorderLo(gad,wind)
struct Gadget *gad;
struct Window *wind;
{
pts1[3]=gad->Height+1;pts1[5]=gad->Height;
pts1[8]=gad->Width+3;
pts2[1]=gad->Height+1;pts2[3]=gad->Height+1;pts2[9]=gad->Height+1;
pts2[2]=gad->Width+3;pts2[4]=gad->Width+3;pts2[6]=gad->Width+2;pts2[8]=gad->Width+2;
DrawBorder(wind->RPort,&brdLO,gad->LeftEdge-2,gad->TopEdge-1);
}
/*********************************************************************/
/*  Name:   
/*  Purpose:   
/*  Variables:  
/*
/*
/*********************************************************************/
