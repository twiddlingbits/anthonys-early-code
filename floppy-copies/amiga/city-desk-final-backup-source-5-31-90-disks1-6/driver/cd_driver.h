

struct cd_port {
   struct MsgPort real_port;
   BYTE  *buf_start;       /** points to memory where graphic goes **/
   short x_off;            /** bit offset in x direction **/
   short modulo;           /** line lenght in bytes. **/
   long MaxXDots;          /** pixels to render in x direction **/
   long MaxYDots;          /** pixels to render in y direction **/
   short XDotsInch;        /** Current printer dpi **/
   short YDotsInch;
   };

