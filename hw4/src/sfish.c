#include "sfish.h"

volatile sig_atomic_t alarmflag = false;

char** parsing(char* str)
{

	if(*str == '\0')
	{
		return NULL;
	}
	int bufferSize = BUFFERSIZE;
	int position = 0;

	char** strTokens = (char**) malloc(bufferSize * sizeof(char*));


	if(strTokens == NULL)
	{
		errno = ENOMEM;
		fprintf(stderr,"%s\n",strerror(errno));
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
				errno = ENOMEM;
				fprintf(stderr,"%s\n",strerror(errno));
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
		errno = ENOMEM;
		fprintf(stderr,"%s\n",strerror(errno));;
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
				errno = ENOMEM;
				fprintf(stderr,"%s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}


		}
		strToken = strtok(NULL,"/");					//get next token

	}

	*(strTokens + position) = NULL;					//last position in array will be null-terminated

	return strTokens;
}

char** parsingPathVar(char* path)
{
	int bufferSize = BUFFERSIZE;
	int position = 0;

	char** strTokens = (char**) malloc(bufferSize * sizeof(char*));


	if(strTokens == NULL)
	{
		errno = ENOMEM;
		fprintf(stderr,"%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

	char* temppath = path;
	char* strToken = strtok(temppath,":");

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
				errno = ENOMEM;
				fprintf(stderr,"%s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}


		}
		strToken = strtok(NULL,":");					//get next token

	}

	*(strTokens + position) = NULL;					//last position in array will be null-terminated

	return strTokens;
}

void validateCmd(char* cmd, char** stringArray,char** cwd,char** oldDir,char* pathV)
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
			char temp[PATH_MAX+1];
			strcpy(temp,*oldDir);
			char temp2[PATH_MAX+1];
			strcpy(temp2,*cwd);
			*oldDir = temp2;
			//setenv("OLDPWD",*cwd,1);
			if(strcmp(*oldDir, "") == 0)
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
				int cwdlength = strlen(*cwd);
				int arglength = strlen(argument);
				char temp[PATH_MAX+1];
				strcpy(temp,*cwd);
				char temp2[PATH_MAX+1];
				strcpy(temp2,*cwd);
				*oldDir = temp2;
				//strcpy(temp2,*cwd);
				strcat(temp,"/");


				if(chdir(argument)== 0)
				{
					strcat(temp,argument);
					temp[cwdlength+arglength+2] = '\0';
					*cwd = temp;
					return;
				}
				else
				{
					errno = ENOENT;
					fprintf(stderr,"%s\n",strerror(errno));
					return;
				}
			}
			else
			{
				errno = ENOENT;
				fprintf(stderr,"%s\n",strerror(errno));
				return;
			}
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
				for(int i = 0; i < 100000000; i++);
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
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
	}

	if(strcmp(cmd,"exit")==0)
	{
		exit(EXIT_SUCCESS);
	}

	if(strcmp(cmd,"alarm") == 0)
	{
		char *argument = *(tempArray+1);

		if(argument == NULL)
		{
			fprintf(stderr,"Invalid argument: alarm requires a number\n");
			return;
		}

		char numc = *argument;
		int numi = atoi(argument);
		if(numc <= '0' || numc >= '9')
		{
			fprintf(stderr,"Invalid argument: alarm requires a number\n");
			return;
		}

		else
		{
			alarmcmd(numi);
		}
		return;

	}

	if(strcmp(cmd,"ls") == 0 || strcmp(cmd,"grep")== 0 || strcmp(cmd,"cat") == 0)
	{
		pid_t pid;
		int status = 0;
		//char* hello = "hello";

		if((pid = fork())==0)
		{
			char** pathTokens = parsingPathVar(pathV);
			const char* AbsPath = getexePath(pathTokens,cmd);

			debug("%s\n",AbsPath);
			for(int i = 0; i < 100000000; i++);
			if(execv(AbsPath,stringArray) == -1)
			{
				//debug("%s\n",hello);
				fprintf(stderr,"%s\n",strerror(errno));
				free(pathTokens);
				return;
			}

			free(pathTokens);
		}
		else
			waitpid(pid,&status,0);
	}

	else
		fprintf(stderr,"Invalid Command\n");

	return;
}

const char* getexePath(char** pathvariables,char* cmd)
{
	char* currentpath = *pathvariables;
	int lengthcmd = strlen(cmd);
	int lengthpath = 0;
	char buffer[PATH_MAX];
	struct stat statbuf;
	const char *hold;


	while(currentpath != NULL)
	{
		lengthpath = strlen(currentpath);
		memcpy(buffer,currentpath,lengthpath);
		buffer[lengthpath] = '/';
		memcpy(buffer + lengthpath +1,cmd,lengthcmd);
		buffer[lengthcmd+lengthpath+1] = '\0';

		hold = buffer;
		//hold = strncpy(hold,buffer,lengthcmd+lengthpath+1);

		if(stat(hold, &statbuf)==0 && (statbuf.st_mode & S_IXUSR))
		{
			//hold = strcpy(hold,buffer);
			//free(statbuf);
			return hold;
		}
		else
		{
			pathvariables++;
			currentpath = *pathvariables;
		}

	}
	//free(statbuf);
	return NULL;
}

void alarmcmd(int time)
{
	int timeDone = 0;
	signal(SIGALRM,alarmhandler);

	timeDone = alarm(time);
	sleep(time);

	info("Your alarm finished with %i seconds\n",timeDone);

	if(alarmflag == true)
	{
		printf("Your %i second timer has finished!\n",time);
		alarmflag = false;
	}
	return;

}

void alarmhandler(int sign)
{
	alarmflag = true;
}



