
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
-------------cut here----------------
#include <intuition/intuition.h>
#include "fcntl.h"
     
#define ABS(x) (((x)>=0) ? (x) : (-(x)))
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
     
#define NLINES 400
     
#define NBUFS  200
     
#define NSAMPS 320
     
#define LINELEN 320
static struct NewScreen ns = {
   0, 0, 320, NLINES,   /* size */
   6,                   /* depth */
   1, 2,                /* pens */
   HAM,                 /* mode */

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
   CUSTOMSCREEN, 0, 0, 0, 0
};
     
static struct NewWindow nw = {
   0, 0, 320, NLINES,
   1, 2,
   0,
   BORDERLESS|BACKDROP,
   0,0,0,0,0,
   0,0,0,0,
   CUSTOMSCREEN
};
     
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
     
struct Window *window;
struct Screen *screen;
     

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
int rf, gf, bf;
     
main()
{
     
USHORT colormap[16];
static USHORT zeromap[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int colorfreq[16][16][16];
static int colordelta[16][16][16];
static int colorval[16][16][16];
int i, j, k, line;
int red, green, blue;
int rd, gd, bd;
int testrd, testgd, testbd;
int testdiff, mindiff;
int rpix, gpix, bpix;
int color, creg;
char fname[80], rname[80], gname[80], bname[80];
static UBYTE rbuf[NBUFS][LINELEN], gbuf[NBUFS][LINELEN], bbuf[NBUFS][LINELEN];

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
int bufn;
/* struct IntuiMessage *message; */
     
rf=-1;
gf=-1;
bf=-1;
     
if (NLINES==400)
   ns.ViewModes |= LACE;
     
IntuitionBase = OpenLibrary("intuition.library", 0);
GfxBase = OpenLibrary("graphics.library", 0);
     
screen = (struct Screen *)OpenScreen(&ns);
if (!screen)
   error("can't open screen");
     
nw.Screen = screen;
window = (struct Window *)OpenWindow(&nw);

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
if (!window)
   error("can't open window");
     
/* initialize color map to b/w */
     
for (i=0; i<16; i++) {
   colormap[i] = (i<<8) + (i<<4) + i;
}
     
screen->ViewPort.ColorMap = GetColorMap(16);
     
LoadRGB4(&screen->ViewPort, colormap, 16);
     
printf("Name:");
scanf("%s\n", fname);
     
strcpy(rname, fname);
strcat(rname, ".red");
strcpy(gname, fname);

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
strcat(gname, ".grn");
strcpy(bname, fname);
strcat(bname, ".blu");
     
rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't open red");
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't open green");
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't open blue");
     
for (i=0; i<16; i++) {
   for (j=0; j<16; j++) {
      for (k=0; k<16; k++) {
         colorfreq[i][j][k] = 0;
         colordelta[i][j][k] = 0;

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
         colorval[i][j][k] = (i<<8)+(j<<4)+k;
      }
   }
}
     
/* First time through: build tables, display with no color regs */
     
bufn = NBUFS;
     
for (line=0; line<NLINES; line++) {
     
   readline(&bufn, rbuf, gbuf, bbuf);
     
   red = 0;
   green = 0;
   blue = 0;
     
   for (i=0; i<NSAMPS; i++) {
     

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
      rpix = rbuf[bufn][i]>>4;
      gpix = gbuf[bufn][i]>>4;
      bpix = bbuf[bufn][i]>>4;
     
      colorfreq[rpix][gpix][bpix]++;
     
      rd = ABS(red-rpix);
      gd = ABS(green-gpix);
      bd = ABS(blue-bpix);
     
      colordelta[rpix][gpix][bpix] += (rd+gd+bd-MAX(rd,MAX(gd,bd)));
     
      if (rd >= gd && rd >= bd) {
         red = rpix;
         SetAPen(window->RPort, 0x20 | red);
      }
      else if (gd >= rd && gd >= bd) {
         green = gpix;
         SetAPen(window->RPort, 0x30 | green);

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
      }
      else {
         blue = bpix;
         SetAPen(window->RPort, 0x10 | blue);
      }
      WritePixel(window->RPort, i, line);
   }
}
     
/* Sort based on most delta... colorfreq currently not used */
     
sort(&colordelta[0][0][0], &colorfreq[0][0][0], &colorval[0][0][0], 16*16*16);
     
for (i=1; i<16; i++) {
   colormap[i] = *(&colorval[0][0][0]+i-1);
   printf("colormap[%d]=%x, delta=%d, freq=%d\n", i, colormap[i], *(&colordelta[
     
0][0][0]+i-1), *(&colorfreq[0][0][0]+i-1));
}

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
     
LoadRGB4(&screen->ViewPort, colormap, 16);
     
/* Second time: Sort via delta only */
     
close(rf);
rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't reopen red");
close(gf);
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't reopen green");
close(bf);
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't reopen blue");
     
for (line=0; line<NLINES; line++) {

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
     
   readline(&bufn, rbuf, gbuf, bbuf);
     
   red = 0;
   green = 0;
   blue = 0;
     
   for (i=0; i<NSAMPS; i++) {
     
      rpix = rbuf[bufn][i]>>4;
      gpix = gbuf[bufn][i]>>4;
      bpix = bbuf[bufn][i]>>4;
     
      color = (rpix<<8) + (gpix<<4) + bpix;
     
      rd = ABS(red-rpix);
      gd = ABS(green-gpix);
      bd = ABS(blue-bpix);
     

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
      mindiff = rd+gd+bd - MAX(rd,MAX(gd,bd));
     
      creg = -1;
     
      for (j=0; j<16; j++) {
         testrd = ABS((int)((colormap[j]>>8)&0x0F) - rpix);
         testgd = ABS((int)((colormap[j]>>4)&0x0F) - gpix);
         testbd = ABS((int)(colormap[j]&0x0F) - bpix);
         testdiff = testrd+testgd+testbd;
         if (testdiff < mindiff) {
            mindiff = testdiff;
            creg = j;
         }
      }
     
      if (creg >= 0) {
         SetAPen(window->RPort, creg);
         red = (colormap[creg]>>8)&0x0F;
         green = (colormap[creg]>>4)&0x0F;

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
         blue = colormap[creg]&0x0F;
      }
     
      else {
     
         if (rd >= gd && rd >= bd) {
            red = rpix;
            SetAPen(window->RPort, 0x20 | red);
         }
         else if (gd >= rd && gd >= bd) {
            green = gpix;
            SetAPen(window->RPort, 0x30 | green);
         }
         else {
            blue = bpix;
            SetAPen(window->RPort, 0x10 | blue);
         }
      }
      WritePixel(window->RPort, i, line);

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
   }
}
     
*fname = 0;
while (*fname != 'q') {
   scanf("%s\n", fname);
   if (*fname == '1')
      LoadRGB4(&screen->ViewPort, colormap, 16);
   if (*fname == '2')
      LoadRGB4(&screen->ViewPort, zeromap, 16);
}
     
#if 0
for (i=0; i<16; i++)
   for (j=0; j<16; j++)
      for (k=0; k<16; k++)
         printf("r=%x, g=%x, b=%x, freq=%d\n", i, j, k, colorfreq[i][j][k]);
#endif
     

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
#if 0
wait (1<<window->UserPort->mp_SigBit);
message = (struct IntuiMessage *)GetMsg(window->UserPort);
ReplyMsg(message);
#endif
     
error("Done");
}
     
     
     
sort(delta, freq, table, n)
int *delta, *freq, *table;
int n;
{
int i, j, temp;
int max;
     
printf("sorting...\n");

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
for (i=0; i<16; i++) {
   max = *(delta+i);
   for (j=i; j<n; j++) {
      if (*(delta+j) > max) {
         temp = *(delta+j);
         *(delta+j) = *(delta+i);
         *(delta+i) = temp;
         temp = *(freq+j);
         *(freq+j) = *(freq+i);
         *(freq+i) = temp;
         max = *(delta+i);
         temp = *(table+j);
         *(table+j) = *(table+i);
         *(table+i) = temp;
      }
   }
}
printf("done sorting\n");
}

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
     
sortfreq(delta, freq, table, n)
int *delta, *freq, *table;
int n;
{
int i, j, temp;
int max;
     
printf("sorting...\n");
for (i=0; i<16; i++) {
   max = *(delta+i) / *(freq+i);
   for (j=i; j<n; j++) {
      if (*(delta+j) / *(freq+j) > max) {
         temp = *(delta+j);
         *(delta+j) = *(delta+i);
         *(delta+i) = temp;
         temp = *(freq+j);
         *(freq+j) = *(freq+i);
         *(freq+i) = temp;

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
         max = *(delta+i);
         temp = *(table+j);
         *(table+j) = *(table+i);
         *(table+i) = temp;
      }
   }
}
printf("done sorting\n");
}
     
     
readline(bufn, rb, gb, bb)
int *bufn;
UBYTE *rb, *gb, *bb;
{
int stat;
     
if (++(*bufn) >= NBUFS) {
   stat = read(rf, rb, LINELEN*NBUFS);

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
   if (stat == -1)
      error("error reading red");
   stat = read(gf, gb, LINELEN*NBUFS);
   if (stat == -1)
      error("error reading green");
   stat = read(bf, bb, LINELEN*NBUFS);
   if (stat == -1)
      error("error reading blue");
   *bufn = 0;
}
}
     
     
error(s)
char *s;
{
     
printf("%s\n", s);
if (rf != -1)

Press RETURN for more...

MAIL> 
[;H[2J    #2           3-AUG-1987 13:52:57                                        MAIL
   close(rf);
if (gf != -1)
   close(gf);
if (bf != -1)
   close(bf);
if (window)
   CloseWindow(window);
if (screen)
   CloseScreen(screen);
if (GfxBase)
   CloseLibrary(GfxBase);
if (IntuitionBase)
   CloseLibrary(IntuitionBase);
     
exit(0);
}
     

MAIL> 
[;H[2J    #3           3-AUG-1987 20:04:02                                        MAIL
From:	Bitnet%"rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet"
To:	E0QHELP
Subj:	Not good news.

Received: From HAMLET(MAILER) by TAMVENUS with RSCS id 4004
          for E0QHELP@TAMVENUS; Mon,  3 Aug 87 20:03 CDT
Received: from Mipl3.JPL.Nasa.Gov by Hamlet.Caltech.Edu with INTERNET ;
          Mon, 3 Aug 87 17:11:55 PDT
Date:    Mon, 3 Aug 87 16:33:07 PDT
From:     rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet
Message-Id: <870803163308.00g@Mipl3.JPL.Nasa.Gov>
Subject: Not good news.
To:       e0qhelp@tamvenus.bitnet
X-ST-Vmsmail-To: ST%"e0qhelp@tamvenus.bitnet",RGD059
     
Hi Evelyn...
     
Please forward this to Anthony IMMEDIATELY.  Strongly suggest to him that he
call up Commodore and bitch severely at them.  I mean, GET REAL UPSET!

Press RETURN for more...

MAIL> 
[;H[2J    #3           3-AUG-1987 20:04:02                                        MAIL
     
-----------cut here----------------
Topic: amiga, Entry # 465
Path: elroy!ames!hao!husc6!cca!rtolly
From: rtolly@cca.CCA.COM (Bob Tolly)
Newsgroups: comp.sys.amiga
Subject: Re: Bundled Software
Message-Id: <18543@cca.CCA.COM>
Date: 3 Aug 87 14:36:44 GMT
References: <17701UH2@PSUVM> <1583@ulowell.cs.ulowell.edu>
Reply-To: rtolly@CCA.CCA.COM.UUCP (Bob Tolly)
Organization: Computer Corp. of America, Cambridge, MA
Lines: 32
To: amiga@mipl3.jpl.nasa.gov
     
The packages as shown on 2 Commodore flyers are:
     
Package A:  $99  (list is $599)
     

Press RETURN for more...

MAIL> 
[;H[2J    #3           3-AUG-1987 20:04:02                                        MAIL
   Textcraft Plus
   Aegis Animator
   Deluxe Paint II
   Pagesetter
   Marble Madness
   Epyx 500XJ Joystick
     
Package B:  $199  (list is $1200)
     
   Pagesetter Deluxe
   WordPerfect
   Superbase
   Maxiplan 500
   CLImate
   Deluxe Video
   Diga
     
     
This will be offered to members of Amiga User Groups in conjunction

Press RETURN for more...

MAIL> 
[;H[2J    #3           3-AUG-1987 20:04:02                                        MAIL
with their Amiga 500 purchase  (although I have
to believe we will see it opened up to just about anyone).
     
I agree with Bob Page - this will not encourage people to develop
software for the Amiga. The prices are such that it would be
hard not to buy one or the other package, even if one doesn't
want everything in it, and that will close a good portion
of the market for competitors of these products.
     

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
From:	Bitnet%"rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet"
To:	E0QHELP
Subj:	bundling stuff

Received: From HAMLET(MAILER) by TAMVENUS with RSCS id 4080
          for E0QHELP@TAMVENUS; Wed,  5 Aug 87 11:34 CDT
Received: from Mipl3.JPL.Nasa.Gov by Hamlet.Caltech.Edu with INTERNET ;
          Wed, 5 Aug 87 08:29:08 PDT
Date:    Wed, 5 Aug 87 08:24:36 PDT
From:     rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet
Message-Id: <870805082436.001@Mipl3.JPL.Nasa.Gov>
Subject: bundling stuff
To:       e0qhelp@tamvenus.bitnet
X-ST-Vmsmail-To: ST%"e0qhelp@tamvenus.bitnet",RGD059
     
Hi Evelyn...
     
please forward this to Anthony.  He (or MicroSearch) might want to take
advantage of the lists mentioned in the second paragraph.  Thanx.

Press RETURN for more...

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
-------------cut here----------------
Topic: amiga, Entry # 501
Path: elroy!ames!rutgers!cbmvax!grr
From: cbmvax!grr@elroy.Jpl.Nasa.Gov (George Robbins)
Newsgroups: comp.sys.amiga
Subject: Re: Bundled Software
Message-Id: <2172@cbmvax.UUCP>
Date: 4 Aug 87 21:40:30 GMT
References: <17701UH2@PSUVM> <1583@ulowell.cs.ulowell.edu> <18543@cca.CCA.COM>
Reply-To: grr@cbmvax.UUCP (George Robbins)
Organization: Commodore Technology, West Chester, PA
Lines: 69
To: amiga@mipl3.jpl.nasa.gov
     
There seems to be a lot of confusion and distress about the new user's group
promotion.  Please read the words!  This is a nice, but very limited promotion
designed to put an A500 and software in the claws of each "Commodore" (ya know,
C64/C128/etc) users group and their members.  One demo system per group, one
software bundle for each member that buys an A500 within the 75 day period.

Press RETURN for more...

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
This may not sound like much, but note that there are something like 750 such
groups in existance.
     
Lists of the participating dealers and user's groups are available for the cost
of postage for any developers or marketers not included who would like to gain
some direct benefit from the promotion.
     
Anyhow, here is a slightly edited description of the promotion that was
intended to explain things to dealers and developers:
--------------------------------------------------------------------------------
     
     
     
COMMODORE USER GROUP PROMO
==========================
     
Commodore USA has announced two exclusive offers being made to recognized
Commodore User groups and their members.
     

Press RETURN for more...

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
THE USER GROUP OFFER
====================
     
The GROUP is offered the opportunity to buy ONE Amiga system per group
consisting of:
     A500 computer with
          A501 (512k extra memory)
          A1080 monitor
          A1020 5.25in drive with Transformer s/w
          A1680 modem
Price is $999.95.  Purchase is direct from CBM, certified cheque in advance
and is only being offreed until 30 Aug 1987.  To be eligible, the group must
have provided an up-to-date list of members.
Each group will also receive 10 PD disks, three free magazines, some technical
info, a software list and AmigaMail.  About 750 User's Groups are eligible.
     
THE USER GROUP MEMBER OFFER
===========================
     

Press RETURN for more...

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
Each member of a participating User Group will be notified of the member offer
by direct mail within about the next two weeks.  (Mailings to dealers and the
group presidents have all ready been made.)  The member who purchases an Amiga
500 from a participating dealer is eligible to buy ONE of two special software
packs at a special offer price.
     Pack A costs the member $99 and consists of DPaint II, Aegis Animator,
Marble Madness, Texcraft Plus, Pagesetter and an Epyx joystick.
     Pack B costs the member $199 and contains Pagesetter Deluxe, Maxiplan 500,
Superbase Personal, Word Perfect, CLI Mate, Diga, and Deluxe Video.
     
This promotion will run from 15 Aug to 31 Oct 1987 at Participating dealers.
     
WHAT'S IN IT FOR YOU
====================
     
The objective of the User Group Promo is to develop a large installed
base of Amiga 500s fast.  This means more potential customers for your own
products.  Remember that it is one or the other on the software packs.
     

Press RETURN for more...

MAIL> 
[;H[2J    #4           5-AUG-1987 11:34:45                                        MAIL
To help you reach this audience we have prepared the lists of participating
groups and dealers.  To get your copy, send a large (9x12inch) SAE (self-
adressed envelop) stamped with 4 first class stamps to Lauren Brown at CATS
usual address.  Mark the envelop CUG LISTS.
--
George Robbins - now working for,       uucp: {ihnp4|seismo|rutgers}!cbmvax!grr
but no way officially representing      arpa: cbmvax!grr@seismo.css.GOV
Commodore, Engineering Department       fone: 215-431-9255 (only by moonlite)

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
From:	Bitnet%"rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet"
To:	E0QHELP
Subj:	Siggraph summary

Received: From HAMLET(MAILER) by TAMVENUS with RSCS id 4081
          for E0QHELP@TAMVENUS; Wed,  5 Aug 87 13:26 CDT
Received: from Mipl3.JPL.Nasa.Gov by Hamlet.Caltech.Edu with INTERNET ;
          Wed, 5 Aug 87 08:42:00 PDT
Date:    Wed, 5 Aug 87 08:38:44 PDT
From:     rgd059%Mipl3.JPL.Nasa.Gov@Hamlet.Bitnet
Message-Id: <870805083844.006@Mipl3.JPL.Nasa.Gov>
Subject: Siggraph summary
To:       e0qhelp@tamvenus.bitnet
X-ST-Vmsmail-To: ST%"e0qhelp@tamvenus.bitnet",RGD059
     
Hi Ev..
     
Again, please forward this to Anthony.  Also tell him he needs to get his
account working again.

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
     
tnx.
----------------cut here----------------
Topic: amiga, Entry # 509
Path: elroy!ames!ll-xn!husc6!umb!ileaf!io!carlos
From: io!carlos@elroy.Jpl.Nasa.Gov (Carlos Smith)
Newsgroups: comp.sys.amiga
Subject: Amiga at Siggraph
Message-Id: <354@io.UUCP>
Date: 4 Aug 87 23:32:06 GMT
Reply-To: carlos (Carlos Smith)
Distribution: world
Organization: Interleaf, Cambridge, MA
Lines: 117
To: amiga@mipl3.jpl.nasa.gov
     
Several recent postings have given tantalizing hints of the Amiga presence at
Siggraph. For the benefit of those who did not attend, this is an
incomplete summary of Amiga-related highlights.

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
     
First of all, every Amigan should be overjoyed at the continuing Amiga
presence at Siggraph (I first saw an Amiga at Siggraph '85). This is THE
computer graphics show, and the fact that the Amiga comes off very well
says a lot, especially since most of the equipment there is in the 10's of
Kbucks range. It becomes clear that the Amiga provides amazing capabilities
for its price. Of course, we all know that. But now, commercial animators,
production houses, TV and movie effects people, scientific graphics users etc.
know that too. This can only expand the Amiga's penetration into all graphics
markets.
     
The Amiga really came into it's own this year, with large thanks to NewTek
(for their DigiView/DigiPaint combo, and Maxine Headroom (gee, I need Hyper-
text here...), Byte-by-Byte with Sculpt-3D, and Aegis with VideoScape 3D
(see footnote). So much of Siggraph is focused on animation and rendering
capabilities that now that Amiga can do 3D modeling, animation and video
effects it HAS to be taken seriously as a useful, cost-effective tool.
     
Amiga had a good medium sized booth, and it was mobbed. Among the people in

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
it were (sorry about those I miss...):
CSA - showing turbo-tower (compatible with A2000 boards) and an '020 board
        for the A500
NewTek - With Digiview, DigiPaint and the greatest demo I've ever seen,
        "Maxine Headroom". Digitized from videotape with a product under
        development is a cute NewTek representative of the female gender
        doing a Max-Headroom type blurb for DigiView, complete with synced
        sampled sound. This was running forwards, backwards, looping and
        stuttering, shown on a big monitor visible from quite a distance.
Aegis - Running demo videos from VideoScape 3D, and later, in real-time, a
        demo Leo Schwab slapped together (though it looked as if it must
        have taken a month) which was amazingly similar to a character
        appearing in Pixar's movie from the film and video show. This featured
        a well-modeled unicycle (spokes and seat springs and all) wobbling
        back and forth juggling balls from its pedals (one of the balls was,
        of course (Leo did this, remember) the Boing ball), while the scene
        rotated continuously in front of the camera. Meanwhile, in the
        background (literally) Sonix was running playing a score. A great demo
        though it did point up a flaw in the way Showanim double-buffers (when

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
        trying to get to the background screen to adjust Sonix, the display
        flickered horribly between frames from VScape and the Sonix screen).
        This should be easy to fix, for instance, by double buffering the
        bitplanes rather than screens, which seems to be what they did. But
        it still worked, and was very impressive. In fact they had to put up
        a "This is NOT a video tape" sign.
Byte-by-Byte - with Sculpt 3D. A very nice 3D modeler and renderer (NOT
        animator). It generates very nice ray-traced images, and has some
        very powerful modeling capabilities. An animator is due "in a few
        months". Hopefully utilities will appear to allow model interchange
        with VideoScape. Byte-By-Byte also had their own booth.
MicroIllusions - was showing a very early demo from a forthcoming modeling/
        animation package they are developing. They also had their own
        booth.
Gary Samad (sorry, I don't remember the name of his company) was demoing
        MicroFiche Filer, and seemed to be getting a lot of interest. This is
        a great package BTW, if you're into databases or just great graphic
        techniques.
Also shown were a TV station package, PageSetter, ULowell's image proecessing

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
        board, and at least 5 other products/companies I don't remember in
        detail.
     
Media Magic(?) had their own booth (luckily right next to Amiga's) showing
Forms-in-Flight, another very nice 3D modeler that is somewhat different
from Sculpt 3D, and is stronger on animation than rendering. (I'm buying
both). It will be important to develop utilities to convert formats between
these different modelers so that they can work with each other, and with
VideoScape, which is very weak in its modeling.
     
The Amiga also had a film in the video show! "Dancing Stumblers" (?) was a
nice little piece done with Aegis Animator that was well enough designed and
animated (much credit to the artist, when I bring the catalog to work I
will post his name) to be shown right up there with the best of Pixar and
Disney! A milestone in Amiga history.
     
Amiga Friends (an LA users group?) had a meeting there with large attendance.
Much of the above software was demoed, along with others (The Director by ???
is definitely something to watch for - perhaps someone with more info will

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
post a summary of this AMAZING animation playing program. Briefly, it lets
you run showanim type animations in various portions of the display.) Also
featured was Dale Luck with his infamous Amiga videos... Two Commodore honchos
discussed their plans for the future, including a major pre-christmas
advertising campaign for the A500 and A2000. They also said that Commodore has
already committed to exhibit at Siggraph '88!
     
Apple was at Siggraph for the first time this year. Atari sure wasn't (I
don't think they ever have...). For me the highlight of the exhibition
happened when I went by the Apple booth. Someone was demoing McMovie or some
such, and a guy was asking him (undoubtably after coming from the Amiga booth)
"What about sound? Can't it do sound?" The Apple guy replied "Well, its a
trade-off. If you want sound, it degrades the video. If you want more sound
you have to have less video..." I had to choke myself to keep from giggling.
     
This is getting long (oh, you noticed already huh?) but I wanted to mention
that I talked with Aegis representatives about the problems I encountered
trying VideoScape 3D. They said that they have heard it doesn't work with
the C-Ltd hard disk (why?), but know of no other problems. I had them go

Press RETURN for more...

MAIL> 
[;H[2J    #5           5-AUG-1987 13:26:12                                        MAIL
through the EXACT sequence of commands I had tried at the store, no problem.
Today, I went back to the store to try it again. However, this time they had
an ASDG with 8Meg (drool drool) hooked up to the same A1000. Again the EXACT
same sequence of commands we had tried earlier WORKED FINE. So something
mysterious happened. Maybe it WAS the Starboard memory (quiet, Perry). They
are going to hook that back up and try again.
     
I want to add that, perhaps with some flaws, VideoScape 3D is capable of
AMAZING results, though they will take some labor to achieve. But the Amiga
CAN do impressive 3D animation TODAY, and that is saying a lot. Congrats
to Allen Hastings (and thanks for your prompt and very informative response
to the previous postings regarding VideoScape).
     
Sorry this is so long, but I hope many of you will find it informative and
interesting.
--
                        Carlos Smith
                        uucp:...!harvard!umb!ileaf!carlos
                        Bix:    carlosmith

MAIL> 
%MAIL-E-NOMOREMSG, no more messages

MAIL> 