/* echo a line back to the display */
main(argc,argv)
int argc;
char *argv[];
{
while (--argc!=0) printf("%s",*++argv);
printf("\n");
}

