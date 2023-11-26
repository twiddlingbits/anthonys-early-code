/******************/
/** power (**)   **/
/******************/

powr(x,y)
register int x,y;
{
register int i,z;
if (y==0) return (1);
z=1;
for (i=0;i<y;i++)
   z=z*x;
return (z);
}

