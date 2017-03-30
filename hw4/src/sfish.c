#include "sfish.h"

char** parsing(char* str)
{
	int bufferSize = BUFFERSIZE;
	int position = 0;

	char** strTokens = (char**) malloc(bufferSize * sizeof(char*));


	if(strTokens == NULL)
	{
		fprintf(stderr,"Allocation Error for strTokens.");
		exit(EXIT_FAILURE);
	}

	char* strToken = strtok(str,DELIM);

	while(strToken != NULL)
	{
		*(strTokens + position) = strToken;
		position++;

		if(position >= bufferSize)
		{
			bufferSize += BUFFERSIZE;
			strTokens = realloc(strTokens, bufferSize * sizeof(char*));

			if(strTokens == NULL)
			{
				fprintf(stderr,"Allocation Error for strTokens.");
				exit(EXIT_FAILURE);
			}


		}
		strToken = strtok(NULL,DELIM);					//get next token

	}

	*(strTokens + position) = NULL;					//last position in array will be null-terminated

	return strTokens;
}

char** parsingDirectory(char* path)
{
	int bufferSize = BUFFERSIZE;
	int position = 0;

	char** strTokens = (char**) malloc(bufferSize * sizeof(char*));


	if(strTokens == NULL)
	{
		fprintf(stderr,"Allocation Error for strTokens.");
		exit(EXIT_FAILURE);
	}

	char* temppath = path;
	char* strToken = strtok(temppath,"/");

	while(strToken != NULL)
	{
		*(strTokens + position) = strToken;
		position++;

		if(position >= bufferSize)
		{
			bufferSize += BUFFERSIZE;
			strTokens = realloc(strTokens, bufferSize * sizeof(char*));

			if(strTokens == NULL)
			{
				fprintf(stderr,"Allocation Error for strTokens.");
				exit(EXIT_FAILURE);
			}


		}
		strToken = strtok(NULL,"/");					//get next token

	}

	*(strTokens + position) = NULL;					//last position in array will be null-terminated

	return strTokens;
}

void validateCmd(char* cmd, char** stringArray,char* pwd)
{
	char** tempArray = stringArray;
	if(strcmp(cmd,"help")== 0)
	{
		USAGE(EXIT_SUCCESS);
		return;
	}

	if(strcmp(cmd,"cd")==0)
	{
		char* argument = *(tempArray+1);
		if(strcmp(argument,".") == 0)
		{
			setenv("OLDPWD",pwd,1);
		}

		else if(strcmp(argument,"..") == 0)
		{
			char* temppwd = pwd;
			setenv("OLDPWD",temppwd,1);
			temppwd = pwd;
			char** splitDirectories = parsingDirectory(temppwd);
			int count = 0;
			char* lastDirectory = "";
			while (splitDirectories[count] != NULL)
			{
				if(splitDirectories[count + 1] == NULL)
				{
					lastDirectory = splitDirectories[count];
				}
				count++;
			}
			pwd = getenv("OLDPWD");

			//char* removestring = "/" + lastDirectory;
			char* locationOfSubstring = strstr(pwd,lastDirectory);
			locationOfSubstring--;
			//int move = strlen(pwd) - strlen(locationOfSubstring);
			strcpy(locationOfSubstring,locationOfSubstring + strlen(lastDirectory)+1);
			free(splitDirectories);
			return;

		}

	if(strcmp(argument,"-") == 0)
		{
			char* oldpwd = getenv("OLDPWD");
			setenv("OLDPWD",pwd,1);
			if(oldpwd == NULL)
				fprintf(stderr,"OldPWD is yet to be defined");
			else
			{
				pwd = oldpwd;
			}
			return;

		}

	if(argument == NULL)
		{
			setenv("OLDPWD",pwd,1);
			pwd = getenv("HOME");
			return;
		}

	else
		{
			if(chdir(argument)!= 0)
			setenv("OLDPWD",pwd,1);
			pwd = strcat(pwd,argument);
			return;
		}

	}

	if(strcmp(cmd,"pwd")==0)
	{
		printf("%s\n",pwd);
		return;
	}

	if(strcmp(cmd,"exit")==0)
	{
		exit(EXIT_SUCCESS);
	}

	else
		fprintf(stderr,"Invalid Command\n");

	return;
}


