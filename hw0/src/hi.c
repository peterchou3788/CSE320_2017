#include "hi.h" /* This is our headerfile */

/**
 * Here's the actual definition of our say_hi() function
 * as you can see it simply returns the string "Hi" Or does it??
 */

char hi[5] = "Hi";
char* say_hi(){
	printf("%s\n",hi );
    return hi;
}
/* Back over to main to finish our program */
