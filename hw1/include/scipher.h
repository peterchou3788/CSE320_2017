#ifndef SCIPHER_H
#define SCIPHER_H

#include <stdlib.h>
#include <stdio.h>

#include "const.h"

int encrypt(char mode,FILE* in, FILE* out,char* Alphabet,int length);
int decrypt(char mode,FILE* in, FILE* out,char* Alphabet,int length);

#endif
