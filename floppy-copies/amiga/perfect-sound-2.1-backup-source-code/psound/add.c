/************************/
/** extend filename     */
/************************/

add(to,from)
char *to,*from;
{

int i;

if (*to==0) {
   strcpy(to,from);
   return(0);
   }
if (to[strlen(to)-1]==':') {
   strcpy(to+strlen(to),from);
   return(0);
   }
to[(i=strlen(to))]='/';
strcpy(to+i+1,from);
}
