main()
{
int i;
char *x[]={"1","2","3","4","5",""};
char **y;

y=x;
i=0;

printf("x %d y %d\n",sizeof(x),sizeof(y));

/*
while (*(*(y+i))!=0) {
	printf("%s\n",*(y+i++));
	}
*/

while (y[i][0]!=0) {

	printf("%s\n",y[i++]);
	}

}

