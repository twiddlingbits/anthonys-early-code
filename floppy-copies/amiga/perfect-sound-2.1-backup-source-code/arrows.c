
USHORT arrowsData[] =
{

0x0000,0x0000,0x0018,0x6000,0x0078,0x7800,
0x01F8,0x7E00,0x07F8,0x7F80,0x1FF8,0x7FE0,
0x07F8,0x7F80,0x01F8,0x7E00,0x0078,0x7800,
0x0018,0x6000,0x0000,0x0000,0x776F,0x726B,
0x696E,0x672E,0x2E2E,0x6E6C,0x792E,0x0A0A,
0x2062,0x6520,0x6672,0x6565,0x6C79,0x2064,
0x6973,0x7472,0x6962,0x7574,0x6564,0x2066,
0x6F72,0x206E,0x6F6E,0x2D70,0x726F,0x6669,
0x740A,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x7C0C,0xF03B,0x3C0C,
0x3BF0,0x3C00,0x0000,0x663B,0x633C,0x0018,
};
struct Image arrows =
{
0,0,          /* LeftEdge, TopEdge */
30,11,1,      /* Width, Height, Depth */
&arrowsData[0],
31,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};

