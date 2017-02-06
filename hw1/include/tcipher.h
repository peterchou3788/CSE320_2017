#ifndef TCIPHER_H
#define TCIPHER_H

#include <stdlib.h>
#include <stdio.h>

#include "const.h"


int tutEncrypt(FILE* in, FILE* out,char** tuten);
int tutDecrypt(FILE* in, FILE* out,char** tuten);


#endif