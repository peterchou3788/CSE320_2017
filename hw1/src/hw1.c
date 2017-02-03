#include "hw1.h"

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {

	/* code here */
	if(argc < 2 || argc > 6)
		USAGE(EXIT_FAILURE);

	char mode = 0x00;
	int AlphabetSize = lengthOfAlphabet();

	char* h = "-h";
	char* s = "-s";
	char* t = "-t";
	char* e = "-e";
	char* d = "-d";

		char* arg1 = *(argv + 1);		//First Arg (h,s,t)
		char* arg2 = *(argv + 2);		//Second Arg(e,d)
		char* arg3 = *(argv + 3);		//Third Arg(input file)
		char* arg4 = *(argv + 4);		//Fourth Arg(output file)

		if(arg1 == s)
		{
			//*(ptr + 1) = 1;
			mode = mode | 0x40;
			if(arg2 == e || arg2 == d)
			{
				if(arg2 == d)
					mode = mode | 0x20;
					//*(ptr + 2) = 1;
				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				mode += AlphabetSize;
				return mode;
			}
		}

		if(arg1 == t)
		{
			if(arg2 == e || arg2 == d)
			{
				if(arg2 == d)
					mode = mode | 0x20;
				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				mode += AlphabetSize;
				return mode;
			}
		}

		if(arg1 == h)
		{
			mode = 0x80;
			USAGE(0);
			return EXIT_SUCCESS;
		}

		else
		{
			if (mode == 0)
			USAGE(EXIT_FAILURE);

		}


}

int lengthOfAlphabet()
{
	int length=0;
	char* alphabetPtr = Alphabet;
	while (alphabetPtr != '\0')
	{
		length++;
		alphabetPtr++;
	}
	return length;
}



