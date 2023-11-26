
/************************/
/** extend filename     */
/************************/

/*
  to   == pointer to string that is a path
  from == pointer to a string containing new segment of path
          (df0:, subdirectory) */

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

