#include "hw1.h"
#include "scipher.h"
#include "tcipher.h"


int main(int argc, char **argv) {

    FILE* in;
    FILE* out;

    char mode;
    int length;
    length = lengthOfAlphabet(Alphabet);
    char* ptrAlphabet = Alphabet;
    /* Note: create a variable to assign the result of validargs */
    mode = validargs(argc, argv, &in, &out);

    if(mode & 64)
    {
    	if(mode & 32)
    		decrypt(mode,in,out,ptrAlphabet,length);
		else
			encrypt(mode,in,out,ptrAlphabet,length);
	}
	else
	{
		if(mode & 32)
			tutDecrypt(in,out,Tutnese);
		else
			tutEncrypt(in,out,Tutnese);
	}

    return EXIT_SUCCESS;
}



