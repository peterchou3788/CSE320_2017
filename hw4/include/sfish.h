#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <debug.h>
#include <stdbool.h>
#include "csapp.h"

#define BUFFERSIZE 128
#define DELIM " \t\n\r"



#define USAGE(return_code) do{                                                                                                   \
    fprintf(stderr, "%s\n",                                                  \
        "Personal Shell\n"                          						 \
        "Bottom information list builtin commands and their basic uses\n\n"  \
        "\thelp: \tDisplays this usage.\n"                                   \
        "\texit: \tExits the shell\n"                                   	 \
        "\tcd: \tChange directories based on the arguemnts\n"           	 \
        "\t\t cd - \treturns to previous accessed directory\n"				 \
        "\t\t cd   \treturns to Home directory\n"							 \
        "\t\t cd . \treturns current directory\n"							 \
        "\t\t cd .. \treturns one directory moved up\n"						\
        "\tpwd: \tPrints absolute path of current directory\n");          \
   break;                                                                                 \
}while(0)

char** parsing(char *str);
void validateCmd(char* cmd,char** stringArray,char** cwd,char** oldDir,char* pathV);
char** parsingPathVar(char* path);
char** parsingDirectory(char* path);
const char* getexePath(char** tokens,char* cmd);
void alarmhandler(int sign);
void alarmcmd(int time);


#endif
