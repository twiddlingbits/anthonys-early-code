// bin2hex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* argv[])
{
	FILE *fp;
	unsigned char buf[256];
	size_t size;

		if (argc!=2)
	{
		printf("Usage: bin2hex <input file>");
		return 0;
	}

	fp = fopen(argv[1], "rb");
	if (fp==NULL)
	{
		printf("unable to open file\n");
		return 0;
	}

	while (1)
	{
		size = fread(buf, sizeof (buf[0]), 256, fp);

		if (size == 0)
			break;
		
		for (int i=0; size >0; size--, i++)
		{
			printf("0x%02x, ",buf[i]);
			if ((i+1)%16==0)
				printf("\n");

		}
	}

	fclose(fp);
	return 0;

}

