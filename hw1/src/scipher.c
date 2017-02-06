#include "scipher.h"


int position(char* Alpha,int c);
int isUpper(int c);
int isNumber(int c);
//int printIn(int c, FILE* in);
int isSymbol(int c);

int encrypt(char mode, FILE* in, FILE* out, char* Alpha, int length)
{
	int shift = mode & 31;
	//char* shiftedAlphabet = Alpha + shift;
	//printf("%d\n",shift);
	//printf("%c\n",'!');

		char* ptr = Alpha;
		//printf("%s\n",(in));
		int c = fgetc(in);
		//printIn(c,in);

		char shiftc;

		while(c != EOF)
		{
			while(isNumber(c) == 0 || c == 32)
			{
				printf("%c",c);
				c = fgetc(in);
			}
			//printf("Letter : %c\n",c);
			///printf("%c",c);
			if(isUpper(c) == 1 && isSymbol(c) == 1)
			{
				c = c - 32;
			//	printf("%c\n",c);
			}
		int pos = position(Alpha,c);
		//printf("Move by this many spaces : %d\n",pos);
		 //int pos = 0;
		 int place = pos + shift;
		 if(place > length)
		 	place = place % length;
		 shiftc = *(ptr+ place);

		/*if(out == stdout)
			printf("%c",shiftc);
		else*/
			fputc(shiftc,out);

			c = fgetc(in);

			while(c == '\n')
			{
				printf("\n");
				c = fgetc(in);
			}

//			printf("%c\n",c);
		}
		//}
		//printf("\n");

	return 0;
}

int decrypt(char mode, FILE* in, FILE* out, char* Alpha, int length)
{

	int shift = mode & 31;
	//char* shiftedAlphabet = Alpha + shift;
	//printf("%d\n",shift);
	//printf("%c\n",'!');

		char* ptr = Alpha;
		//printf("%s\n",(in));
		int c = fgetc(in);
		//printIn(c,in);

		char shiftc;

		while(c != EOF)
		{
			while(isNumber(c) == 0 || c == 32)
			{
				//printf("%c",c);
				c = fgetc(in);
			}
			//printf("Letter : %c\n",c);
			///printf("%c",c);
			if(isUpper(c) == 1 && isSymbol(c) == 1)
			{
				c = c - 32;
			//	printf("%c\n",c);
			}
		int pos = position(Alpha,c);
		//printf("Move by this many spaces : %d\n",pos);
		 //int pos = 0;
		 int place = pos - shift;
		 //printf("%d\n",place);
		 if(place < 0)
		 	place = place + length;
		 shiftc = *(ptr+place);

		/*if(out == stdout)
			printf("%c",shiftc);
		else*/
			fputc(shiftc,out);

			c = fgetc(in);

			while(c == '\n')
			{
				printf("\n");
				c = fgetc(in);
			}

//			printf("%c\n",c);
		}
		//}
		printf("\n");
	return 0;
}

int position(char* Alpha,int c)
{
	if(c == '\0')
	{
		printf("%c",'\0');
		return 1;
	}
	int pos = 0;
	char* ptr = Alpha;
	while(c != *(ptr))
	{
	//	printf("%c",*(ptr));
		ptr = ptr + 1;
		pos++;
	}
	//printf("\n");
	return pos;
}

int isUpper(int c)
{
	if(c >= 65 && c <= 90)
		return 0;
	else
		return 1;
}

int isNumber(int c)
{
	if(c >= '0' && c <= '9')
		return 0;
	else
		return 1;
}

/*int printIn(int c,FILE* in)
{

	while(c != EOF)
		{
			while(isNumber(c) == 0 || c == 32)
			{
				printf("%c",c);
				c = fgetc(in);
			}
			if(isUpper(c) == 1 && isSymbol(c) == 1)
			{
				c = c - 32;
			//	printf("%c\n",c);
			}
			//printf("%c",c);
			///printf("%c",c);


			fputc(c,stdout);

			c = fgetc(in);

			while(c == '\n')
			{
				printf("\n");
				c = fgetc(in);
			}
		}

	return 0;
}*/

int isSymbol(int c)
{
	if(c >= 0x21 && c <= 0x2F)
		return 0;
	if(c >= 0x3A && c <= 0x40)
		return 0;
	if(c >= 0x7B && c <= 0x7E)
		return 0;
	else
		return 1;
}
