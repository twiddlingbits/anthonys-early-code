main()
{
char *coded;

coded = code_string("abcd ABCD");
printf("%s\n%s\n",coded,uncode_string(coded));
coded = code_string("This is a Test");
printf("%s\n%s\n",coded,uncode_string(coded));
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

uncode_string(s)
char *s;
{
static char ns[80];
short p;

p=0;
while (*s) {
   if (*s=='[')
      ns[p++] = ' ';
   else
      ns[p++] = *s+1;
   s++;
   }
ns[p]=0;
return(ns);
}



