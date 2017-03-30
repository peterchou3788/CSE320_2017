#include "sfish.h"
#include "debug.h"

/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char *cmd = "";
    char *pwd = getenv("PWD");
    char** stringArray = NULL;
    char* homeEnv = getenv("HOME");
    char* logName = getenv("LOGNAME");
    char* path = getenv("PATH");
    debug("Home Environment : %s\n",homeEnv);
    debug("LogName Environment : %s\n",logName);
    debug("Paths Environment : %s\n",path);
    debug("PWD : %s\n", pwd);
    int boolean = chdir(strcat(pwd,"/tests"));
    debug("change d? : %i\n", boolean);
    char* buffer = (char*) malloc(128 * sizeof(char*));
    buffer = getcwd(buffer,128);
    debug("Current Directory: %s\n",buffer);
    //getcwd()
   // pwd = "/SampleDir $";
    while((cmd = readline("pechou : ")) != NULL) {
       // if (strcmp(cmd, "exit")==0)
        //    break;
        printf("%s\n",cmd);
        stringArray = parsing(cmd);

        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        info("Length of command entered: %ld\n", strlen(cmd));

        char ** tempArray = stringArray;
        while(*tempArray != NULL)
        {
            debug("Strings inside cmd: %s\n",*tempArray);
            tempArray++;
        }

        char* builtinCmd = *stringArray;                    //get first string which indicates the command
        validateCmd(builtinCmd,stringArray,pwd);            //Commences the command, if not a command, print invalid command



        /* You WILL lose points if your shell prints out garbage values. */
    }



    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    free(stringArray);

    return EXIT_SUCCESS;
}
