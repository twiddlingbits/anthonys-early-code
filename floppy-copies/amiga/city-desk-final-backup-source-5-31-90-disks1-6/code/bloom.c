main()
{
int p,i;
char *s,news[80];

printf("%d %d %d\n",'a','A','a'-'A');

for (i=-300; i < 300; i++) {
   s = uncode_string("UTZAAKE EQLR",i);
   p = strlen(s);
   do {
      news[strlen(s)-p] = s[p-1];
      p--;
      } while (p);
   news[strlen(s)]=0;
   printf("%4d %s %s\n",i,s,news);
   }
}

code_string(s)
char *s;
{
static char ns[80];
short p;

p=0;
while (*s) {
   if (*s==' ')
      ns[p++] = '[';
   else
      ns[p++] = *s-1;
   s++;
   }
ns[p]=0;
return(ns);
}

uncode_string(s,offset)
int offset;
char *s;
{
static char ns[80];
short p;

p=0;
while (*s) {
      ns[p++] = *s-offset;
   s++;
   }
ns[p]=0;
return(ns);
}



