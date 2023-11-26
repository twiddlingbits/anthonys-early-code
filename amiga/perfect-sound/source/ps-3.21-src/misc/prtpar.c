#include <exec/types.h>


void main()
{
UBYTE save_data2, save_data;
UBYTE save_dir2, save_dir;
UBYTE save_talo, save_tahi;
UBYTE save_cra;

save_data	=*((UBYTE *)0xBFE101);		/* data1 */
save_dir		=*((UBYTE *)0xBFE301);		/* dir1 */
save_data2	=*((UBYTE *)0xBFD000);		/* data 2 */
save_dir2	=*((UBYTE *)0xBFD200);		/* dir 2 */
save_talo	=*((UBYTE *)0xBFE401);		/* timer low */
save_tahi	=*((UBYTE *)0XBFE501);		/* timer high */
save_cra		=*((UBYTE *)0XBFEE01);		/* control reg a */

printf("data 1 %x\n",save_data);
printf("dir 1 %x\n",save_dir);
printf("data 2 %x\n",save_data2);
printf("dir 2 %x\n",save_dir2);
printf("talo %x\n",save_talo);
printf("tahi %x\n",save_tahi);
printf("cra %x\n",save_cra);
}


