#include "exec/types.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "laser.h"

static USHORT shift_val;   /** used to implement "light" mode **/
UBYTE bit_table[] = {128,64,32,16,8,4,2,1};
static UBYTE inv_bit_table[] = {~128,~64,~32,~16,~8,~4,~2,~1};

extern UBYTE *fitp_cliptop;   /*** bytes that define the clipping zone **/
extern UBYTE *fitp_clipbot;   /*** used by fits ***/

void dither(g, mem, x_off, dest_w, dest_h, mod, flags)

struct graphic *g;
UBYTE *mem;
int x_off, mod;
int dest_w, dest_h;
int flags;

{
register USHORT x,y;
USHORT val;
static UBYTE ordered[] = {0,8,2,10,12,4,14,6,3,11,1,9,14,7,13,5};
static UBYTE halftone[] = {5,12,14,3,8,0,6,10,13,2,4,14,7,11,9,1};
UBYTE *matrix;
FIXED x_scale, y_scale;
register FIXED x_sum, y_sum;
USHORT get_pix_val();
void set_pix_val();

if (g->numplanes==1) {
   fit(g->planes[0]+RASSIZE(g->w,g->usey),
       g->usex,             /* srcoff */
       g->useh,             /* srch */
       g->usew,             /* srcw */
       RASSIZE(g->w,1),     /* srcmod */
       mem,                 /* dest */
       x_off,
       dest_h,              /* desth */
       dest_w,              /* destw */
       mod);    /* destmod == len in bytes of dest area */
   return;
   }

if (flags & DITHER_HALFTONE)
   matrix = &halftone[0];
else
   matrix = &ordered[0];

set_light(flags);    /** Checks DITHER_LIGHT **/

x_scale = ((g->usew*4) << 16) / dest_w;
y_scale = ((g->useh*4) << 16) / dest_h;

y_sum = g->usey<<16;
for (y=0; y < dest_h; y++) {
   x_sum = g->usex<<16;
   for (x=0; x < dest_w; x++) {     /** x_scale * (x/4) **/
      val =  get_pix_val (g, x_sum >> 16, y_sum >> 16 );

/* printf("pix val %d x %d y %d matrix %d",
        val,x,y, matrix[ (x&3) + ((y&3)<<2)]); */

      if ( matrix[ (x&3) + ((y&3)<<2)] < val) {
         (*(mem + ((x+x_off) >> 3))) |= bit_table[(x+x_off)&7];
         }
      if ((x&3)==3) {x_sum += x_scale;}
      }
   if ((y&3)==3) {y_sum += y_scale;}
   mem += mod;
   }
}   

void clip_dither(g, mem, x_off, dest_w, dest_h, mod, flags)

struct graphic *g;
UBYTE *mem;
int x_off, mod;
int dest_w, dest_h;
int flags;

{
register USHORT x,y;
USHORT val;
static UBYTE ordered[] = {0,8,2,10,12,4,14,6,3,11,1,9,14,7,13,5};
static UBYTE halftone[] = {5,12,14,3,8,0,6,10,13,2,4,14,7,11,9,1};
UBYTE *matrix;
FIXED x_scale, y_scale;
register FIXED x_sum, y_sum;
USHORT get_pix_val();
void set_pix_val();

if (g->numplanes==1) {
   cfit(g->planes[0]+RASSIZE(g->w,g->usey),
       g->usex,             /* srcoff */
       g->useh,             /* srch */
       g->usew,             /* srcw */
       RASSIZE(g->w,1),     /* srcmod */
       mem,                 /* dest */
       x_off,
       dest_h,              /* desth */
       dest_w,              /* destw */
       mod);    /* destmod == len in bytes of dest area */
   return;
   }

if (flags & DITHER_HALFTONE)
   matrix = &halftone[0];
else
   matrix = &ordered[0];

set_light(flags);    /** Checks DITHER_LIGHT **/

x_scale = ((g->usew*4) << 16) / dest_w;
y_scale = ((g->useh*4) << 16) / dest_h;

y_sum = g->usey<<16;
for (y=0; y < dest_h; y++) {
   if (mem >= fitp_cliptop && mem < fitp_clipbot) {
      x_sum = g->usex<<16;
      for (x=0; x < dest_w; x++) {     /** x_scale * (x/4) **/
         val =  get_pix_val (g, x_sum >> 16, y_sum >> 16 );
         if ( matrix[ (x&3) + ((y&3)<<2)] < val) {
            (*(mem + ((x+x_off) >> 3))) |= bit_table[(x+x_off)&7];
            }
         if ((x&3)==3) {x_sum += x_scale;}
         }
      }
   if ((y&3)==3) {y_sum += y_scale;}
   mem += mod;
   }
}


void inv_clip_dither(g, mem, x_off, dest_w, dest_h, mod, flags)

struct graphic *g;
UBYTE *mem;
int x_off, mod;
int dest_w, dest_h;
int flags;

{
register USHORT x,y;
USHORT val;
static UBYTE ordered[] = {0,8,2,10,12,4,14,6,3,11,1,9,14,7,13,5};
static UBYTE halftone[] = {5,12,14,3,8,0,6,10,13,2,4,14,7,11,9,1};
UBYTE *matrix;
FIXED x_scale, y_scale;
register FIXED x_sum, y_sum;
USHORT get_pix_val();
void set_pix_val();

if (g->numplanes==1) {
   cinvfit(g->planes[0]+RASSIZE(g->w,g->usey),
       g->usex,             /* srcoff */
       g->useh,             /* srch */
       g->usew,             /* srcw */
       RASSIZE(g->w,1),     /* srcmod */
       mem,                 /* dest */
       x_off,
       dest_h,              /* desth */
       dest_w,              /* destw */
       mod);    /* destmod == len in bytes of dest area */
   return;
   }

if (flags & DITHER_HALFTONE)
   matrix = &halftone[0];
else
   matrix = &ordered[0];

set_light(flags);    /** Checks DITHER_LIGHT **/

x_scale = ((g->usew*4) << 16) / dest_w;
y_scale = ((g->useh*4) << 16) / dest_h;

y_sum = g->usey<<16;
for (y=0; y < dest_h; y++) {
   if (mem >= fitp_cliptop && mem < fitp_clipbot) {
      x_sum = g->usex<<16;
      for (x=0; x < dest_w; x++) {     /** x_scale * (x/4) **/
         val =  get_pix_val (g, x_sum >> 16, y_sum >> 16 );
         if ( matrix[ (x&3) + ((y&3)<<2)] < val) {
            (*(mem + ((x+x_off) >> 3))) &= inv_bit_table[(x+x_off)&7];
            }
         if ((x&3)==3) {x_sum += x_scale;}
         }
      }
   if ((y&3)==3) {y_sum += y_scale;}
   mem += mod;
   }
}


USHORT get_pix_val (g, x , y)

struct graphic *g;
int x,y;

{

int off;
USHORT val,i;
register UBYTE byte,bit;

val = 0;

off = RASSIZE(g->w, y) + (x>>3);
bit = bit_table[x&7];

for (i=0; i < g->numplanes; i++) {
   byte = *(g->planes[i]+off);
   val = val>>1;
   if (byte&bit) {
      val = val|8;
      }
   }
return((USHORT)(val>>shift_val));
}

set_light(flags)

int flags;

{

if (flags & DITHER_LIGHT)
   shift_val = 1;
else
   shift_val = 0;

}

/* void set_pix_val (mem, x)

UBYTE *mem;
int x;

{
mem = mem + (x>>3);
(*mem) |= bit_table[x&7];
}    */

