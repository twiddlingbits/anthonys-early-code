
#define MAX_CELLS 10
#define MAX_SOUNDS 5
#define MAX_SEQUENCES 50


struct anim_pointer {
   unsigned short *cell_data[MAX_CELLS];
   unsigned char cell_order[MAX_SEQUENCES];
   unsigned short h,w;
   unsigned char planes;
   unsigned char cmap[12];
   };


#define AOK 0
#define CANCEL 1
#define FILENOTFOUND 2
#define NOTIFF 3
#define BADIFF 4
#define OUTOFMEM 5

/** Wait flags. **/

#define PAUSE  0x1      /** Just let time pass **/
#define RMBD   0x2      /** Right mouse button down **/
#define RMBU   0x4      /** Right mouse button up **/
#define LMBD   0x8      /** Left mouse button down **/
#define LMBU   0x10     /** Left mouse button up **/
#define QUIET  0x20     /** No input events for time period **/

/** Action flags **/

#define CLEAR        0x01     /** Clear Pointer (use pref's default **/
#define NEWPOINTER   0x02     /** Switch to a new pointer **/
#define TRAVEL       0x04     /** Travel pointer to new position **/


