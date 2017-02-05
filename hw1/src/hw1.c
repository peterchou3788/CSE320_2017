#include "hw1.h"
typedef int boolean;
#define FALSE 0
#define TRUE 1

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

int lengthOfAlphabet(char*);
boolean strcomp(char* , char*);
int uitoa(char*);


char validargs(int argc, char** argv, FILE** in, FILE** out) {

	/* code here */
	printf("%d\n", argc);
	if(argc < 2 || argc > 6)
		USAGE(EXIT_FAILURE);

	unsigned char mode = 0;
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


		if(argc == 6)					//Check for n
		{
			n = uitoa(arg5);			//convert string into int
		}
		printf("%d\n",n);			//print value of n

		int length = n%AlphabetSize;		//LSB 5 bits
		printf("length : %d\n",length);

		if(strcomp(arg1,"-s") == TRUE)		//check for s as first arg
		{
			mode = mode | length;			//setting last 5 LSB
			mode = mode | 64;				//bit 1 for s
			if(strcomp(arg2,"-e") || strcomp(arg2, "-d") == TRUE)
			{
				if(strcomp(arg2,"-d") == TRUE)
					mode = mode | 32;

				if(strcomp(arg3,"-"))
					*in = stdin;
				else
					*in = fopen(arg3,"r");

				if(strcomp(arg4,"-"))
					*out = stdout;
				else
					*out = fopen(arg4,"w");

				printf("%d\n",mode);

				return mode;
			}
		}


		if(strcomp(arg1,"-t")==TRUE)		//check for t as first arg
		{
			mode = mode | length;
			if(strcomp(arg2,"-e") || strcomp(arg2, "-d") == TRUE)
			{
				if(strcomp(arg2,"-d") == TRUE)
					mode = mode | 32;

				if(strcomp(arg3,"-"))
					*in = stdin;
				else
					*in = fopen(arg3,"r");

				if(strcomp(arg4,"-"))
					*out = stdout;
				else
					*out = fopen(arg4,"w");

				printf("%d\n",mode);

				return mode;
			}
		}

		if(strcomp(arg1,"-h")==TRUE)		//check for h as first arg
		{
			mode = mode |128;
			printf("%d\n",mode);
			//printf("%c\n",mode);
			USAGE(EXIT_SUCCESS);
		}


			//if (mode == 0)					//if all else fails, return failure
			//USAGE(EXIT_FAILURE);

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
	return num;
}





