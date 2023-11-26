
/**************************************************/
/** Make a copy of a string using amiga AllocMem **/
/**************************************************/

newstr(oldstr)

char *oldstr;

{
char *str;
int len;

len = strlen(oldstr)+1;

str = AllocMem(len,0);
if (str)
   movmem(oldstr,str,len);

return(str);
}
