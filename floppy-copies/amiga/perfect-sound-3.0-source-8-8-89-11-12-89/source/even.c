
/***********/
/** even  **/
/***********/

even(i)
int i;
{
return(0xFFFFFFFE&i);
}

odd(i)
int i;
{
return(i|1);
}

