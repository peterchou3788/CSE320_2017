#include "hw1.h"
typedef int boolean;
#define FALSE 0
#define TRUE 1

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

int lengthOfAlphabet(char*);
boolean strcomp(char* , char*);


char validargs(int argc, char** argv, FILE** in, FILE** out) {

	/* code here */
	printf("%d\n", argc);
	if(argc < 2 || argc > 6)
		USAGE(EXIT_FAILURE);

	char mode = 0;
	int n = 320;
	int AlphabetSize = lengthOfAlphabet(Alphabet);

	printf("Size of Alphabet : %d\n", AlphabetSize);


		char* arg1 = *(argv + 1);		//First Arg (h,s,t)
		char* arg2 = *(argv + 2);		//Second Arg(e,d)
		char* arg3 = *(argv + 3);		//Third Arg(input file)
		char* arg4 = *(argv + 4);		//Fourth Arg(output file)
		char* arg5 = *(argv + 5);		//Value for n , No n => 320


		printf("%s\n",arg1);
		//printf("%c\n",arg5);
		//printf("%s\n",(char*)0x40);
		//printf("%s\n",arg2);



		if(strcomp(arg1,"-s") == TRUE)		//check for s as first arg
		{

			if(strcomp(arg5,"") == TRUE)		//Check for n
		{
			char c = *(arg5);
			n = c - '0';
			printf("%d\n",n);
		}
		int length = n%AlphabetSize;
		printf("length : %d\n",length);

			mode = mode | 0x40;
			if(strcomp(arg2,"-e") || strcomp(arg2, "-d") == TRUE)
			{
				if(strcomp(arg2,"-d") == TRUE)
					mode = mode | 0x20;

				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				mode += (char)length;
				return mode;
			}
		}


		if(strcomp(arg1,"-t")==TRUE)		//check for t as first arg
		{
			if(strcomp(arg2,"-e") || strcomp(arg2, "-d") == TRUE)
			{
					if(strcomp(arg2,"-d") == TRUE)
					mode = mode | 0x20;

				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				mode = mode | 0x01;
				return mode;
			}
		}

		if(strcomp(arg1,"-h")==TRUE)		//check for h as first arg
		{
			mode = 0x80;
			USAGE(EXIT_SUCCESS);
		}

		else
		{
			if (mode == 0)					//if all else fails, return failure
			USAGE(EXIT_FAILURE);

		}

return mode;
}

int lengthOfAlphabet(char *Alpha)
{
	int length=0;
	char* alphabetPtr = Alpha;
	while (*(alphabetPtr) != '\0')
	{
		length++;
		alphabetPtr++;
	}
	return length;
}

/*void createBooleansArgs(boolean* b,int argc)
{
	for(int i = 0; i < argc;i++)
	{
		*b = FALSE;
		b++;
	}
}

void fillBooleanArgs(boolean* b,int argc, char** argv)
{
	char* ptr = *argv;
	for(int i = 0; i < argc;i++)
	{
		if (strcomp(ptr,"-h") == TRUE)
		{

		}

	}
}*/

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





