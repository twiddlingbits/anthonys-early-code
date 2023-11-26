/**
*
* This file contains information used by the decimal arithmetic package.
*
* A floating decimal number is a byte array consisting of a two-byte
* header followed by a byte for each two digits.  The header has the
* following format:
*
*	Byte 0, bit 7:		Set if negative number
*	Byte 0, bits 0 to 6:	Number of digit bytes (array length - 2)
*	Byte 1			Decimal exponent (-128 to +127)
*
**/


#define D_DIG 8			/* Maximum number of digit bytes */
#define D_MAX (D_DIG+2)		/* Maximum number of bytes */

extern char I0[],I1[],I2[];	/* Integer constants 0, 1, 2 */
extern char D5[],D05[],D005[];	/* Decimal constants 0.5, 0.05, 0.005 */
#ifndef PI
extern char PI[];               /* Constant PI   */
#endif
#ifndef PID2
extern char PID2[];             /* Constant PI/2 */
#endif
extern char PIM2[];		/* Constant PI*2 */
#ifndef E
extern char E[];		/* Constant E (base of natural logs) */
#endif
extern char M[];		/* Constant log10(E) */
extern char DPR[],RPD[];	/* Degrees per radian, radians per degree */
extern char SR10[];		/* Square root of 10 */
extern char X[],Y[],Z[];	/* Work areas */
extern char X1[],Y1[],Z1[];	/* Work areas */

extern char FDEDIT;		/* Set to include leading dollar sign */
extern char FDTYPE;		/* Set if last cvfd input was exponential */
extern char FDDECP;		/* decimal point character */
extern char FDMONY;		/* money symbol */

extern char *cvfd(),*cvfdx(),*vcfd(),*vcfdi(),*vcfde(),*vcfddc();

