/******************************/
/** wait for a key press     **/
/*****************************/

waitkey(fp)
int fp;
{
char xx;
pr(fp,"");
pr(fp,"Press any key to continue...");
Read(fp,&xx,1);
pr(fp,"\014");
}

