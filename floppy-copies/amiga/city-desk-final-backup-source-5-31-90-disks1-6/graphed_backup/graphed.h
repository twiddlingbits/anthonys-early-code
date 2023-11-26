/*         graphics editor include file         */

#define YES         1
#define NO          0

#define DRAWA       100
#define DRAWB       0
#define LINEA       1
#define LINEB       2
#define BOXA        3
#define BOXB        4
#define FILLA       5
#define PASTEA      6
#define PASTEB      7
#define PATB        8
#define PASTEM      69
#define PASTEN      70
#define PASTEO      71
#define ERASEA      72
#define ERASEB      73
#define ERASEC      74
#define CUTA        66
#define CUTB        67
#define ROTA        9
#define ROTB        10
#define FLIPA       12
#define FLIPB       13
#define CUSTBRUSH   14
#define MAGA        15
#define MAGB        16
#define BRUSHF      17
#define RESIZEA     18
#define RESIZEB     19
#define CIRCLEA     20
#define CIRCLEB     21


#define BRUSHA      0
#define BRUSHB      1
#define BRUSHC      2
#define BRUSHD      3
#define BRUSHE      4

#define GETPIX(ptr,bpr,x,y) (((*((ptr)+(bpr)*(y)+((x)>>3))) >> (7-(x)&7))&1)


