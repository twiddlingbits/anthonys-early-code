/************/
/** alter  **/
/************/

alter(val,maxval,x)

int maxval,x;
int *val;
{
if (x>600 && x<621 && *val>0) *val=*val-1;
if (x>621 && x<640 && *val<maxval) *val=*val+1;
}

