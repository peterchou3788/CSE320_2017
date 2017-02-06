/*#include <stdlib.h>
#include <stdio.h>
typedef int boolean;
#define FALSE 0
#define TRUE 1


boolean strcomp(char* string1, char* string2)
{
	int count = 0;
	while(*(string1+count) == *(string2+count))
	{
		if(*(string1+count) == '\0' || *(string2+count) == '\0')
			break;
		count++;
	}

	if(*(string1+count) == '\0' && *(string2+count) == '\0')
		return TRUE;
	else
		return FALSE;

}

int main()
{
	char* c1 = "Hello";
 	char* c2 = "Hello";
 	boolean b;
	b = strcomp(c1,c2);

	printf("%i\n",b);
	return EXIT_FAILURE;

}*/