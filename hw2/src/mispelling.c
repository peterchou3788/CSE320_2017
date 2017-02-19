#include "mispelling.h"
#include "debug.h"

char** gentypos(int n, char* word) {
	srand(time(NULL));
	char** typos = (char**) malloc(sizeof(char*) * n);
	int len = strlen(word);


	while(n--) {
		int i = rand() % len;
		debug("%d\n",i);
		char nc = 97 + rand() % 26;
		debug("%c\n",nc);
		typos[n] = strdup(word);
		typos[n][i] = nc;
		debug("%s\n",typos[n]);
		//if(strcmp(word, typos[n]) != 0)
		//	free(typos[n]);
		//debug("%s\n",typos[n]);
	}

	return typos;
}
