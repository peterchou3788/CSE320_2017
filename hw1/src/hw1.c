#include "hw1.h"

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {

	/* code here */
	if(argc < 2 || argc > 6)
		USAGE(EXIT_FAILURE);

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
			if(arg2 == e || arg2 == d)
			{
				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				USAGE(EXIT_SUCCESS);
			}
		}

		if(arg1 == t)
		{
			if(arg2 == e || arg2 == d)
			{
				*in = fopen(arg3,"r");
				*out = fopen(arg4,"w");
				USAGE(EXIT_SUCCESS);
			}
		}

		if(arg1 == h)
		{
			USAGE(EXIT_SUCCESS);
		}

		else
			USAGE(EXIT_FAILURE);

}



