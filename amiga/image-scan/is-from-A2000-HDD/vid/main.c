
void main()
{
register unsigned long loop;
register unsigned char val;
int count,x;

printf("enter count: ");
scanf("%d",&count);

for (loop=0; loop < 10000; loop++) {
   Disable();
   x = digi_pix(count);
   Enable();
   printf("%d\n",x);
   }
}

