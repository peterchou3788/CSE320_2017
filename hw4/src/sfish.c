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

void validateCmd(char* cmd, char** stringArray,char** cwd,char** oldDir)
{
	char** tempArray = stringArray;
	if(cmd == NULL)
		return;

	if(strcmp(cmd,"help")== 0)
	{
		USAGE(EXIT_SUCCESS);
		return;
	}

	if(strcmp(cmd,"cd")==0)
	{
		char* argument = *(tempArray+1);

		if(argument == NULL)						//cd without arguments returns home directory
		{
			*oldDir = *cwd;
			char* newDir = getenv("HOME");
			chdir(newDir);
			*cwd = newDir;
			return;
		}
		if(strcmp(argument,".") == 0)				//cd . returns current directory
		{
			*oldDir = *cwd;
			//setenv("OLDPWD",pwd,1);
			return;
		}

		if(strcmp(argument,"..") == 0)				//cd .. returns parent directory of current
		{
			char temppwd[PATH_MAX+1] = "";
			strcpy(temppwd,*cwd);
			char newDir[PATH_MAX+1] = "";
			setenv("HOLDPWD",temppwd,1);

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

			*oldDir = getenv("HOLDPWD");
			strcpy(newDir,*oldDir);

			//char* removestring = "/" + lastDirectory;
			char* locationOfSubstring = strstr(newDir,lastDirectory);
			locationOfSubstring--;
			//int move = strlen(pwd) - strlen(locationOfSubstring);
			strcpy(locationOfSubstring,locationOfSubstring + strlen(lastDirectory)+1);

			chdir(newDir);
			*cwd = newDir;
			free(splitDirectories);

			return;

		}

	if(strcmp(argument,"-") == 0)
		{
			char temp[PATH_MAX+1] = "";
			strcpy(temp,*oldDir);
			char temp2[PATH_MAX+1] = "";
			strcpy(temp2,*cwd);
			*oldDir = temp2;
			//setenv("OLDPWD",*cwd,1);
			if(strcmp(*oldDir, ""))
				fprintf(stderr,"OldPWD is yet to be defined\n");
			else
			{
				*cwd = temp;
				chdir(*cwd);
			}
			return;

		}


	else
		{
			if(access(argument,F_OK)== 0)
			{
				*cwd = strcat(*cwd,"/");
				*oldDir = *cwd;

				if(chdir(argument)== 0)
				{
					*cwd = strcat(*cwd,argument);
					return;
				}
				else
					fprintf(stderr,"cd : File or folder does not exist\n");
			}
			else
				fprintf(stderr,"cd : File or folder does not exist\n");
		}

		}

	if(strcmp(cmd,"pwd")==0)
	{
		//printf("%s\n",pwd);
		int child_status = 0;

		pid_t child = fork();
		if(child >= 0)
		{
			if(child == 0)					//child process
			{
				printf("%s\n",*cwd);
				exit(0);
			}
			else							//parent process
			{
				wait(&child_status);
			}
			return;
		}
		else
		{
			fprintf(stderr,"pwd : fork failed");
			return;
		}
	}

	if(strcmp(cmd,"exit")==0)
	{
		exit(EXIT_SUCCESS);
	}

	if(strcmp(cmd,"ls") || strcmp(cmd,"grep") || strcmp(cmd,"cat"))
	{

	}

	else
		fprintf(stderr,"Invalid Command\n");

	return;
}


