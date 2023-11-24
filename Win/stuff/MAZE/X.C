main()
{
short i;

for (i=0; i < 100; i++)
	printf("%x\n",myrand());
}

myrand()
{
static unsigned short seed = 9546;
unsigned short x,old,y;

old = seed;

x = seed&1;
y = seed&8;
y = y >>3;
x = (x^y)&1;

seed = seed>>1;
seed |= x<<15;

return(seed);
}
