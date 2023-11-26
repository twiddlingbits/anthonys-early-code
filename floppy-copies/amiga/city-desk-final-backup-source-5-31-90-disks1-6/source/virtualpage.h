
#ifndef LIBRARIES_VIRTUALPAGE_H
#define LIBRARIES_VIRTUALPAGE_H   1

/* ------------------------------------------------------------------- *
 *                                                                     *
 * VirtualPage.h -- Virtual Page Library Structures                    *
 *                                                                     *
 *                     Wayne E. Miller                                 *
 *                     June, 1988                                      *
 *                                                                     *
 * Copyright (C) 1988, Wayne E. Miller - All rights reserved.          *
 *                                                                     *
 * ------------------------------------------------------------------- */

/* --- Define Library Name ------------------------------------------- */

#define VPLIBNAME "VirtualPage.library"
#define VPLIBVERSION   1

/* --- Virtual Page Description Structure ---------------------------- */

extern struct VirPageDesc {            /* Virtual Page Descriptor      */
   struct VirPageDesc *vpd_Prev;       /* Previous Structure Pointer   */
   struct VirPageDesc *vpd_Next;       /* Next Structure Pointer       */
   struct BitmapList *vpd_BitmapList;  /* BitmapList Struct Pointer    */
   UWORD  vpd_Width;                   /* Width Of This Page Type      */
   UWORD  vpd_Height;                  /* Height Of This Page Type     */
   UWORD  vpd_Depth;                   /* Depth Of This Page Type      */
   UWORD  vpd_DPI_X;                   /* Horizontal Resolution (DPI)  */
   UWORD  vpd_DPI_Y;                   /* Vertical Resolution (DPI)    */
};

/* --- Virtual Page Bitmap List Structure  --------------------------- */

extern struct BitmapList {             /* Bitmap List Structure        */
   struct BitmapList *vpbl_Prev;       /* Previous Structure Pointer   */
   struct BitmapList *vpbl_Next;       /* Next Structure Pointer       */
   char   *vpbl_Name;                  /* Pointer To Name Of This Page */
   struct VPBitmap *vpbl_VPBitmap;     /* First Bitmap Pointer         */
   struct VPColorMap *vpbl_VPColorMap; /* First Color Map Pointer      */
   struct VirPageDesc *vpbl_VirPageDesc;   /* VirPageDesc Pointer      */
   UWORD  vpbl_OpenCount;              /* Number Of Active Opens       */
   UWORD  vpbl_UserCount;              /* Number Of Active Users       */
   UBYTE  vpbl_Flags;                  /* Flags                        */
   UBYTE  vpbl_Pad;                    /* Pad Byte                     */
};

/* --- Virtual Page Bitmap Structure --------------------------------- */

 extern struct VPBitmap {          /* Vitrual Page Bitmap Structure    */
   struct VPBitmap *vpb_Prev;      /* Pointer To Previous VPB Struct   */
   struct VPBitmap *vpb_Next;      /* Pointer To Next VPB Struct       */
   union  {
       char *vpb_BitmapC;          /* Contiguous Type Bitmap Pointer   */
       char **vpb_BitmapS;         /* Scattered Type Bitmap Pointer    */
   };
   UBYTE  vpb_Layer;       /* Layer Number Of This Page (0, 1, ..., n) */
   UBYTE  vpb_Type;        /* Type, 0 - Contiguous, 1 - Scattered      */
};

/* --- Virtual Page Color Map Structure ------------------------------ */

extern struct ColorEntry {         /* Color Map Entry Definition       */
   UBYTE  vpce_Red;                /* Amount Of Red                    */
   UBYTE  vpce_Green;              /* Amount Of Green                  */
   UBYTE  vpce_Blue;               /* Amount Of Blue                   */
   UBYTE  vpce_Flags;              /* General Purpose Flags            */
};

extern struct VPColorMap {         /* Vitrual Page Color Map Struct    */
   struct VPColorMap *vpc_Prev;    /* Pointer To Previous VPB Struct   */
   struct VPColorMap *vpc_Next;    /* Pointer To Next VPB Structure    */
   UWORD  vpc_Count;               /* Number Of Colors Represented     */
   struct ColorEntry vpc_ColorEntry[2^16]; /* Array Of Color Entries   */
};

/* --- New Virtual Page Request Structure  --------------------------- */

extern struct NewVirPage {             /* Open Virtual Page Request    */
   struct BitmapList *nvp_Reference;   /* Pointer To Existing VP       */
   UWORD  nvp_Width;                   /* Width Of This Page Type      */
   UWORD  nvp_Height;                  /* Height Of This Page Type     */
   UWORD  nvp_Depth;                   /* Depth Of This Page Type      */
   UWORD  nvp_DPI_X;                   /* Horizontal Resolution (DPI)  */
   UWORD  nvp_DPI_Y;                   /* Vertical Resolution (DPI)    */
   UBYTE  nvp_VirFlags;                /* VP Allocation Flags          */
   char   nvp_Reserved[8];             /* Reserved, Must Be Null       */
};

/* Virtual Page Structure (User's) ----------------------------------- */

extern struct VirtualPage {            /* Virtual Page Structure       */
   struct VirtualPage *vp_Prev;        /* Previous Structure           */
   struct VirtualPage *vp_Next;        /* Next Structure               */
   struct tcb *vp_Task;                /* Task That Owns This Open     */
   struct VirPageDesc *vp_VirPageDesc;     /* Descriptor Structure     */
   struct BitmapList *vp_BitmapList;   /* Bitmap List Structure        */
   struct VPBitmap *vp_VPBitmap;       /* First Bitmap Structure       */
   struct VPColorMap *vp_VPColorMap;   /* First Color Map Structure    */
   struct MsgPort *vp_PrtPort;         /* Printer I/O Message Port     */
   struct IOLaserPrt *vp_PrtRequest;   /* Printer I/O Request Struct   */
   ULONG  vp_SignalMask;               /* Signal Mask (For Wait())     */
   UBYTE  vp_SignalNumber;             /* Signal Number                */
   UBYTE  vp_Flag;                     /* Flag Byte                    */
};

/* --- Printer Information Structures -------------------------------- */

extern struct StdSizes {           /* Standard Bitmap Size     */
   UWORD  Std_Width;               /* Width In Pixels          */
   UWORD  Std_Height;              /* Height In Pixels         */
};

   /* --- Note: This Structure Has No Defined Maximum Size ----------- */

extern struct PrinterInfo {        /* Printer Info                     */
   struct PrinterInfo *vppi_Prev;  /* Pointer To Previous Structure    */
   struct PrinterInfo *vppi_Next;  /* Pointer To Next Structure        */
   APTR   vppi_Link;               /* Link To Internal Printer Info    */
   UWORD  vppi_Width;              /* Max Usable Printer Width (Pels)  */
   UWORD  vppi_Height;             /* Max Usable Printer Height (Pels) */
   UWORD  vppi_Colors;             /* Number Of Colors Possible        */
   UWORD  vppi_DPI_X;              /* Horizontal Resolution (DPI)      */
   UWORD  vppi_DPI_Y;              /* Vertical Resolution (DPI)        */
   UWORD  vppi_PhyWidth;           /* Physical Printer Width           */
   UWORD  vppi_PhyHeight;          /* Physical Printer Height          */
   UBYTE  vppi_PrtNumber;          /* Printer Number                   */
   UBYTE  vppi_Pad;                /* Pad Byte                         */
   struct StdSizes vppi_StdSizes[999]; /* Array Of Standard Sizes      */
};

/* --- Printer Type Definitions -------------------------------------- */

#define    PRTYP_PLP       1           /* General Computer PLP         */

/* --- Page Type Definitions ----------------------------------------- */

#define    VPTYP_LETTER    0           /* Letter                       */
#define    VPTYP_LEGAL     1           /* Legal                        */
#define    VPTYP_HLETTER   2           /* Half Letter                  */
#define    VPTYP_HLEGAL    3           /* Half Legal                   */

/* --- NewVirPage Request Flags -------------------------------------- */

/* --- Must Be Contiguous ---------------- */

#define    VPRQB_CONTIGUOUS    (7)
#define    VPRQF_CONTIGUOUS    (1<<7)

/* ---  Must Stay In Memory Always ------- */

#define    VPRQB_RESIDENT      (6)
#define    VPRQF_RESIDENT      (1<<6)

/* ---  Set Permanent Flag --------------- */

#define    VPRQB_PERMANENT     (5)
#define    VPRQF_PERMANENT     (1<<5)

/* ---  Private Page --------------------- */

#define    VPRQB_PRIVATE       (4)
#define    VPRQF_PRIVATE       (1<<4)

/* ---  Create New Page Always ----------- */

#define    VPRQB_NEWPAGE       (1)
#define    VPRQF_NEWPAGE       (1<<1)

/* ---  Check DPI For Match Also --------- */

#define    VPRQB_CHECKDPI      (0)
#define    VPRQF_CHECKDPI      (1<<0)

/* --- Open Printer Flag Definitions --------------------------------- */

/* ---  Initialize I/O Request Structure - */

#define    VPPRB_INIT          (7)
#define    VPPRF_INIT          (1<<7)

/* ---  Auto Adjust (DPI / Full Page) ---- */

#define    VPPRB_AUTOF         (6)
#define    VPPRF_AUTOF         (1<<6)

/* ---  Auto Adjust Horizontal ----------- */

#define    VPPRB_AUTOX         (5)
#define    VPPRF_AUTOX         (1<<5)

/* ---  Auto Adjust Vertical ------------- */

#define    VPPRB_AUTOY         (4)
#define    VPPRF_AUTOY         (1<<4)

/* ---  Auto Center Horizontal ----------- */

#define    VPPRB_CENTRX        (3)
#define    VPPRF_CENTRX        (1<<3)

/* ---  Auto Center Vertical ------------- */

#define    VPPRB_CENTRY        (2)
#define    VPPRF_CENTRY        (1<<2)

