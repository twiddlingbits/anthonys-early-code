#include <exec/types.h>

/** saves all parallel port cia registors that perfect sound
	 trashes **/


#define INTENA 	((USHORT *)(0xDFF09A))
#define INTENAR 	((USHORT *)(0xDFF01C))
#define INTREQ 	((USHORT *)(0xDFF09C))
#define INTREQR 	((USHORT *)(0xDFF01E))
#define CIAAICR	((UBYTE *)(0xbfed01))

UBYTE *data2,*data,*direction,*direction2;
UBYTE save_data2, save_data;
UBYTE save_dir2, save_dir;
UBYTE save_talo, save_tahi;
UBYTE save_cra;

short sp_k=0;

UBYTE cia_a_icr=0;		/** also see new_monright() **/

void save_par()
{
sp_k++;

if (sp_k==1) {
	save_data	=*((UBYTE *)0xBFE101);		/* data1 */
	save_dir		=*((UBYTE *)0xBFE301);		/* dir1 */
	save_data2	=*((UBYTE *)0xBFD000);		/* data 2 */
	save_dir2	=*((UBYTE *)0xBFD200);		/* dir 2 */
	save_cra		=*((UBYTE *)0XBFEE01);		/* control reg a */
	}
}

void restore_par()
{
APTR Cia;

sp_k--;
if (sp_k==0) {
	*((UBYTE *)0xBFE101)=save_data;		/* data1 */
	*((UBYTE *)0xBFE301)=save_dir;		/* dir1 */
	*((UBYTE *)0xBFD000)=save_data2;		/* data 2 */
	*((UBYTE *)0xBFD200)=save_dir2;		/* dir 2 */
	*((UBYTE *)0XBFEE01)=save_cra	;		/* control reg a */

/* for some unknown reason, an A2000 with an A2090 hard disk
	controllor used to hang up sometimes after monitoring and then
	trying to save.  This was caused by new_monleft()
	and new_monright() clearing cia a's icr with a btst #3,icr.
	This had the effect of clearing timer interupts that happened
	during the monitoring (which has Enable()/Disable()) arround it.
	To fix the problem, new_monright() and new_monleft() OR icr into
	the global variable cia_a_icr. Below I force the generation
	of the inerupts that were accidently cleared **/

	if (Cia=(APTR)OpenResource("ciaa.resource",0)) {	/** No close?? **/
		SetICR(Cia, 0x80|(cia_a_icr&0x17));			/** set ints **/
		cia_a_icr=0;									/** except keyboard SP */
		}
	}
}
