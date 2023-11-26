

main()
{
int i;
int fp;


fp = fopen("alphabet.txt","w");

for (i=32; i < 126; i++)
   fprintf(fp,"%c",i);

fclose(fp);
}


