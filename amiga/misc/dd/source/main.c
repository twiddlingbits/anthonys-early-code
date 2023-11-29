

#define READ_ID 1

void main()
{
int choice;
void read_id();

printf("\014\n\n2X disk drive test program.\n");
while(1) {
   printf("\n\n1) read drive id\n");
   printf("2) change cylinder\n");
   printf("3) change track\n");
   printf("4) change sector\n");
   printf("5) read sector\n");
   printf("6) write sector\n");
   printf("99) exit\n\n\n");
   printf("Enter choice --> ");

   scanf("%d\n",&choice);

   switch(choice) {
      case 1:
               read_id();
               break;
      case 99:
            exit(0);
      }
   }
}
/* 2X Command Structure.

   The commands send_byte, get_byte, send_512, get_512 are used to control
   the drive.  Since, timing is critical in the get commands, data can
   be lost if interupts are enabled.  The get commands are also cpu
   intensive since they wait in a polling loop. During the get_512
   commands, interupts are disabled to solve the problem.  However,
   interupts are not disabled during the get_byte command.  The problem
   off missing bytes is solved by having the amiga echo every byte
   that is recieved in get_byte.  If the echo is not recieved by
   the drive, it will re-send the byte.  This is handled automaticly
   byte the drive send_byte command.
   */

void read_id()
{
unsigned char x[45];
unsigned char get_byte();
unsigned short k;

k = 0;

Disable();
send_byte(READ_ID);
while ((x[k++]=get_byte()) && (k < 40)) ;
Enable();
x[k] = 0;
printf("ID: %s (%d)\n",x,strlen(x));
}


