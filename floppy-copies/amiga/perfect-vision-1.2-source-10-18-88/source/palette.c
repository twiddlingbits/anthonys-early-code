

#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>


palette(screen,x,y,w,h)
struct Screen *screen;
int x,y,h,w;
{

struct Window *win;
struct IntuiMessage *mess;
ULONG mclass;
USHORT mcode;
APTR addr;
struct Gadget *curgadget;
struct PropInfo *prop;
int boxdatax[32];
int boxdatay[32];
USHORT colors[32],oldcolors[32];
static int num[5] = {2,4,8,16,32};
int i,curcolor;
int Mx,My,r,g,b,var1;


static SHORT BorderVectors1[] = {
   0,0,
   96,0,
   96,12,
   0,12,
   0,0
};
static struct Border Border1 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors1,
   NULL
};

static struct IntuiText IText1 = {
   3,0,JAM2,
   22,2,
   NULL,
   "CANCEL",
   NULL
};

static struct Gadget cancelbutton = {
   NULL,
   118,-15,
   93,11,
   GRELBOTTOM,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border1,
   NULL,
   &IText1,
   NULL,
   NULL,
   NULL,
   NULL
};

static SHORT BorderVectors2[] = {
   0,0,
   96,0,
   96,12,
   0,12,
   0,0
};
static struct Border Border2 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors2,
   NULL
};

static struct IntuiText IText2 = {
   3,0,JAM2,
   39,2,
   NULL,
   "OK",
   NULL
};

static struct Gadget okbutton = {
   &cancelbutton,
   11,-15,
   93,11,
   GRELBOTTOM,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border2,
   NULL,
   &IText2,
   NULL,
   NULL,
   NULL,
   NULL
};

static struct PropInfo blueknobSInfo = {
   AUTOKNOB+FREEVERT,
   -1,-16,
   -1,4095,
};

static struct Image Image1 = {
   0,66,
   12,5,
   0,
   NULL,
   0x0000,0x0000,
   NULL
};

static struct Gadget blueknob = {
   &okbutton,
   70,25,
   20,-47,
   GRELHEIGHT,
   GADGIMMEDIATE+FOLLOWMOUSE,
   PROPGADGET,
   (APTR)&Image1,
   NULL,
   NULL,
   NULL,
   (APTR)&blueknobSInfo,
   NULL,
   NULL
};

static struct PropInfo greenknobSInfo = {
   AUTOKNOB+FREEVERT,
   -1,-1,
   -1,0xffff/16,
};

static struct Image Image2 = {
   0,67,
   12,5,
   0,
   NULL,
   0x0000,0x0000,
   NULL
};

static struct Gadget greenknob = {
   &blueknob,
   40,25,
   20,-47,
   GRELHEIGHT,
   GADGIMMEDIATE+FOLLOWMOUSE,
   PROPGADGET,
   (APTR)&Image2,
   NULL,
   NULL,
   NULL,
   (APTR)&greenknobSInfo,
   NULL,
   NULL
};

static struct PropInfo redknobSInfo = {
   AUTOKNOB+FREEVERT,
   -1,-1,
   -1,0xffff/16,
};

static struct Image Image3 = {
   0,67,
   12,5,
   0,
   NULL,
   0x0000,0x0000,
   NULL
};

static struct Gadget redknob = {
   &greenknob,
   10,25,
   20,-47,
   GRELHEIGHT,
   GADGIMMEDIATE+FOLLOWMOUSE,
   PROPGADGET,
   (APTR)&Image3,
   NULL,
   NULL,
   NULL,
   (APTR)&redknobSInfo,
   NULL,
   NULL
};

static struct NewWindow nwin = {
   0,11,
   236,109,
   0,1,
   MOUSEBUTTONS+GADGETUP+GADGETDOWN+MOUSEMOVE,
   WINDOWDRAG+WINDOWDEPTH+ACTIVATE,
   &redknob,
   NULL,
   "palette editor",
   0,
   NULL,
   236,109,
   640,400,
   CUSTOMSCREEN,
};

static struct IntuiText IText3 = {
   1,0,JAM2,
   14,14,
   NULL,
   "R   G   B",
   NULL
};


nwin.Screen = screen;
win = 0;
if (w != 0 && h != 0) {
   nwin.LeftEdge = x;
   nwin.TopEdge = y;
   if (w >= nwin.Width)
      nwin.Width = w;
   if (w >= nwin.Height)
      nwin.Height = h;
   }
i = 0;
while (i<num[screen->BitMap.Depth-1]) 
   colors[i] = GetRGB4(screen->ViewPort.ColorMap,i++);
i = 0;
while (i<num[screen->BitMap.Depth-1]) 
   oldcolors[i] = colors[i++];

nwin.DetailPen = num[screen->BitMap.Depth-1]-1;
nwin.BlockPen = (UBYTE)num[screen->BitMap.Depth-1]-2;
if (nwin.BlockPen >3)
   nwin.BlockPen = (UBYTE) 3;

if ((win = (struct Window *)OpenWindow(&nwin))== NULL)
   return(-1);
PrintIText(win->RPort,&IText3,0,0);

colorboxtable(win,screen->BitMap.Depth,&boxdatax[0],&boxdatay[0]);
makeboxes(win,screen->BitMap.Depth,&boxdatax[0],&boxdatay[0]);

curcolor = 0;
r = (colors[curcolor] & 0x0f00)>>8;
g = (colors[curcolor] & 0x00f0)>>4;
b = colors[curcolor] & 0x000f;
prop = (struct PropInfo *) redknob.SpecialInfo;
ModifyProp(&redknob,win,NULL,prop->Flags, prop->HorizPot,
           (0xffff*(r))/(15), prop->HorizBody,
           prop->VertBody);
prop = (struct PropInfo *) greenknob.SpecialInfo;
ModifyProp(&greenknob,win,NULL,prop->Flags, prop->HorizPot,
           (0xffff*(g))/(15), prop->HorizBody,
           prop->VertBody);
prop = (struct PropInfo *) blueknob.SpecialInfo;
ModifyProp(&blueknob,win,NULL,prop->Flags, prop->HorizPot,
           (0xffff*(b))/(15), prop->HorizBody,
           prop->VertBody);



while (TRUE) {
   Wait(1<<win->UserPort->mp_SigBit);
   while(mess = GetMsg(win->UserPort)) {
      mclass = mess->Class;
      mcode = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;
      addr = mess->IAddress;
      ReplyMsg(mess);

      if (mclass == MOUSEBUTTONS && mcode == SELECTDOWN) {
         if (Mx >99 && Mx<win->Width-9 && My >12 && My<win->Height-18 ) {
            var1 = num[screen->BitMap.Depth-1];
            while (var1-- > 0 && (boxdatax[var1] >Mx || boxdatay[var1] >My))
              ;
            curcolor = var1;
            r = (colors[curcolor] & 0x0f00)>>8;
            g = (colors[curcolor] & 0x00f0)>>4;
            b = colors[curcolor] & 0x000f;
            prop = (struct PropInfo *) redknob.SpecialInfo;
            ModifyProp(&redknob,win,NULL,prop->Flags, prop->HorizPot,
                       (0xffff*(r))/(15), prop->HorizBody,
                       prop->VertBody);
            prop = (struct PropInfo *) greenknob.SpecialInfo;
            ModifyProp(&greenknob,win,NULL,prop->Flags, prop->HorizPot,
                       (0xffff*(g))/(15), prop->HorizBody,
                       prop->VertBody);
            prop = (struct PropInfo *) blueknob.SpecialInfo;
            ModifyProp(&blueknob,win,NULL,prop->Flags, prop->HorizPot,
                       (0xffff*(b))/(15), prop->HorizBody,
                       prop->VertBody);

            }
         }

      if (mclass == MOUSEMOVE) {
         changecolor:
         i = ((prop->VertPot * (15-0) + (1<<15)) >>16);
         r = (colors[curcolor] & 0x0f00)>>8;
         g = (colors[curcolor] & 0x00f0)>>4;
         b = colors[curcolor] & 0x000f;
         if (curgadget == &redknob)
            r = i;
         if (curgadget == &greenknob)
            g = i;
         if (curgadget == &blueknob)
            b = i;
         SetRGB4(&(screen->ViewPort),curcolor,r,g,b);
         colors[curcolor]=(((r<<4)+g)<<4)+b;
         
         }

      else if (mclass == GADGETDOWN) {
         curgadget = addr;
         prop = (struct PropInfo *) curgadget->SpecialInfo;
         goto changecolor;
         }

      if (mclass == GADGETUP) {
         if (addr == (APTR)&okbutton) {
            CloseWindow(win);
            return(0);
            }
         if (addr == (APTR)&cancelbutton) {
            LoadRGB4(&(screen->ViewPort),&oldcolors[0],num[screen->BitMap.Depth-1]);
            CloseWindow(win);
            return(1);
            }
         }



      }
   }
}


makeboxes(win,planes,boxdatax,boxdatay)
struct Window *win;
int planes,*boxdatax,*boxdatay;
{

int w,h;
int i,k;
struct RastPort *rp;
static int num[5] = {2,4,8,16,32};
static int xoff[5] = {1,2,2,4,4};
static int yoff[5] = {2,2,4,4,8};

k = num[planes-1];
w = ((win->Width-109) / xoff[planes-1]);
h = ((win->Height-31) / yoff[planes-1]);

i = 0;
rp = win->RPort;
while(i<k) {
   SetAPen(rp,i);
   RectFill(rp,boxdatax[i],boxdatay[i],boxdatax[i]+w,boxdatay[i]+h);
   i++;
   }

}


colorboxtable(win,planes,boxdatax,boxdatay)
struct Window *win;
int planes,*boxdatax,*boxdatay;
{

int w,h;
int i,k;
static int num[5] = {2,4,8,16,32};
static int xoff[5] = {1,2,2,4,4};
static int yoff[5] = {2,2,4,4,8};

k = num[planes-1];
h = ((win->Height-31) / yoff[planes-1]);
w = ((win->Width-109) / xoff[planes-1]);

i=0;
while (i < k) {
   boxdatay[i] = 13 + h * (i/xoff[planes-1]);
   boxdatax[i] = 100 + w * (i - (i/xoff[planes-1]*xoff[planes-1]));
   i++;
   }

}



