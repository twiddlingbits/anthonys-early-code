/**************************/
/** flip a range of bytes */
/**************************/

flip (addrs,len)

char *addrs;
int len;

{

register char *s,*e;
register char temp;
register int k;

s=addrs;
e=addrs+len-1;
k=len/2;

while (k-->0) {
   temp=*s;
   *s=*e;
   *e=temp;
   s++;e--;
   }
return(0);
}
   
