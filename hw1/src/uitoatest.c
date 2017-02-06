/*#include <stdio.h>
#include <stdlib.h>

int uitoa(char* number)
{
	int base = 10;
	int size = 0;
	int num = 0;
	char* ptr = number;

	while(*(ptr) != '\0')
	{
		size++;
		ptr++;
	}

	for(int i = 0;i<size;i++)
	{
		num *= base;
		num += (int) (*(number) - '0');
		number++;
	}
	printf("%d\n",num);
	return num;
}

int main()
{
	char* aNum = "320";
	uitoa(aNum);
	return EXIT_SUCCESS;
}*/