#include "tcipher.h"

int lengthOfTuten(char** tuten);
int isNumberT(int c);
int isSymbolT(int c);
int isUpperT(int c);

int tutEncrypt(FILE* in,FILE* out,char** tuten)
{
	char* ptr = *(tuten);
	int c = fgetc(in);
	int size = lengthOfTuten(tuten);
	printf("%d\n",size);
	printf("%c\n",**(tuten));

	while(c != EOF)
		{
			while(isNumberT(c) == 0 || c == 32)
			{
				printf("%c",c);
				c = fgetc(in);
			}

			if(isUpperT(c) == 1 && isSymbolT(c) == 1)
			{
				c = c - 32;

			}
			if(c == 'A'|| c == 'I'|| c == 'E'|| c == 'O'|| c =='U')
			{
				fputc(c,out);
			}
			else
			{
				for(int i = 0; i<size;i++)
				{
					if(c == *(ptr))
					{
						int count = 0;
						while(*(ptr) != '\0')
						{
							char t = *(*(tuten+i)+count);
							count++;
							fputc(t,out);
						}
					}
				}

			}

			if(c == '\0')
				return 0;


			while(c == '\n')
			{
				printf("\n");
				c = fgetc(in);
			}

		}


	return 0;
}

int tutDecrypt(FILE* in,FILE* out,char** tuten)
{
	return 0;
}

int lengthOfTuten(char** tuten)
{
	int length = 0;
	char* ptr = *(tuten);

	while(ptr!= '\0')
	{
		length++;
		ptr++;
	}
	return length;

}

int isUpperT(int c)
{
	if(c >= 65 && c <= 90)
		return 0;
	else
		return 1;
}

int isNumberT(int c)
{
	if(c >= '0' && c <= '9')
		return 0;
	else
		return 1;
}

int isSymbolT(int c)
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
