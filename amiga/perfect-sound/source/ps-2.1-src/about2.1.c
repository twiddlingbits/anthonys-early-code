
USHORT BorderVectors1[] = {0,0,28,0,28,11,0,11,0,0};
struct Border Border1 = {
    -2,-1,    /* border XY origin relative to container TopLeft */
    3,0,JAM1,    /* front pen, back pen and drawmode */
    5,    /* number of XY vectors */
    BorderVectors1,    /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText IText1 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    4,1,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "OK",    /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget Gadget1 = {
    NULL,    /* next gadget */
    420,183,    /* origin XY of hit box relative to window TopLeft */
    25,10,    /* hit box width and height */
    GADGHCOMP,    /* gadget flags */
    RELVERIFY,    /* activation flags */
    BOOLGADGET,    /* gadget type */
    (APTR)&Border1,    /* gadget border or image to be rendered */
    NULL,    /* alternate imagery for selection */
    &IText1,    /* first IntuiText structure */
    0,    /* gadget mutual-exclude long word */
    NULL,    /* SpecialInfo structure for string gadgets */
    1,    /* user-definable data (ordinal gadget number) */
    NULL    /* pointer to user-definable data */
};

/* Gadget list */

struct IntuiText IText18 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    307,166,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "(409) 846-1311",    /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct IntuiText IText17 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    101,184,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "College Station, Texas 77841",    /* pointer to text */
    &IText18    /* next IntuiText structure */
};

struct IntuiText IText16 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    150,174,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "P.O. Box 1453 ",    /* pointer to text */
    &IText17    /* next IntuiText structure */
};

struct IntuiText IText15 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    131,165,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "SunRize Industries",    /* pointer to text */
    &IText16    /* next IntuiText structure */
};

struct IntuiText IText14 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    27,150,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "$89.95.  Visa or Mastarcard are welcome.",    /* pointer to text */
    &IText15    /* next IntuiText structure */
};

struct IntuiText IText13 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    27,141,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "digitizer from your dealer, or direct from us for",    /* pointer to text */
    &IText14    /* next IntuiText structure */
};

struct IntuiText IText12 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    27,131,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "Sound stereo sound digitizer.  You may purchase this",    /* pointer to text */
    &IText13    /* next IntuiText structure */
};

struct IntuiText IText11 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    26,122,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "This software was developed to support the Perfect",    /* pointer to text */
    &IText12    /* next IntuiText structure */
};

struct IntuiText IText10 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    35,101,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "3. Destroy all of your copies of Perfect Sound",    /* pointer to text */
    &IText11    /* next IntuiText structure */
};

struct IntuiText IText9 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    34,91,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "2. Purchase the SunRize Audio Digitizer",    /* pointer to text */
    &IText10    /* next IntuiText structure */
};

struct IntuiText IText8 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    34,81,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "1. Purchase this Software by sending $20 to SunRize.",    /* pointer to text */
    &IText9    /* next IntuiText structure */
};

struct IntuiText IText7 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    22,64,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "two weeks: ",    /* pointer to text */
    &IText8    /* next IntuiText structure */
};

struct IntuiText IText6 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    23,55,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "MUST take one of the following three actions within",    /* pointer to text */
    &IText7    /* next IntuiText structure */
};

struct IntuiText IText5 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    22,46,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "friends. After you receive Perfect Sound, you ",    /* pointer to text */
    &IText6    /* next IntuiText structure */
};

struct IntuiText IText4 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    21,37,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "free to give copies of Perfect Sound to your",    /* pointer to text */
    &IText5    /* next IntuiText structure */
};

struct IntuiText IText3 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    20,28,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "by Anthony Wood of SunRize Industries.  You are",    /* pointer to text */
    &IText4    /* next IntuiText structure */
};

struct IntuiText IText2 = {
    3,0,JAM2,    /* front and back text pens and drawmode */
    20,18,    /* XY origin relative to container TopLeft */
    NULL,    /* font pointer or NULL for defaults */
    "Perfect Sound ver 2.1 is (C)  copyrighted 1987",    /* pointer to text */
    &IText3    /* next IntuiText structure */
};

#define ITextList IText2

struct NewWindow NewWindowStructure = {
    91,0,    /* window XY origin relative to TopLeft of screen */
    471,200,    /* window width and height */
    0,1,    /* detail and block pens */
    GADGETUP,    /* IDCMP flags */
    ACTIVATE+NOCAREREFRESH,    /* other window flags */
    &Gadget1,    /* first gadget in gadget list */
    NULL,    /* custom CHECKMARK imagery */
    "About Perfect Sound...",    /* window title */
    NULL,    /* custom screen */
    NULL,    /* custom bitmap */
    5,5,    /* minimum width and height */
    640,200,    /* maximum width and height */
    WBENCHSCREEN    /* destination screen type */
};

/* end of PowerWindows source generation */
