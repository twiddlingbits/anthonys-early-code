
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

i=strlen(to);

if (*to==0) {
   strcpy(to,from);
   return(0);
   }
if (to[i-1]==':' || to[i-1]=='/') {
   strcpy(to+i,from);
   return(0);
   }

to[i]='/';
strcpy(to+i+1,from);
}

