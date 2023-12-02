

main()
{
unsigned char *icr;
int i;

icr = 0xbfed01;

for (i=0; i < 1000; i++)
   printf("%x\n",*icr);
}

