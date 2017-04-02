#include "sfish.h"
#include "debug.h"

volatile sig_atomic_t typeflag = true;
volatile sig_atomic_t userflag = false;
volatile sig_atomic_t childpid = 0;
volatile sig_atomic_t childtime = 0;


/*
 * As in previous hws the main function must be in its own file!
 */
void block_handler(int sig)
{
    sigset_t mask_all;
    sigset_t prev_all;
    Sigfillset(&mask_all);

    Sigprocmask(SIG_BLOCK,&mask_all,&prev_all);
    //typeflag = true;
}

void user2_handler(int sig)
{

    if(sig == SIGUSR2)
    {
        Sio_puts("\nWell that was easy\n");
    }
    //userflag = true;

}

void child_handler(int sig,siginfo_t * siginfo,void* something)
{
    //childflag = true;
    Sio_puts("\nChild with PID ");
    Sio_putl((long)siginfo->si_pid);
    Sio_puts(" has died. ");

    float a = siginfo->si_utime;
    float b = siginfo->si_stime;
    float CPUtime = (a+b)/(sysconf(_SC_CLK_TCK)/1000.0);
    Sio_puts("It spent ");
    Sio_putl(CPUtime);
    Sio_puts(" milliseconds utilizing the CPU.\n");

}

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char *cmd = "";
    char *pwd = getenv("PWD");
    char** stringArray = NULL;
    pid_t pidShell;
  //  char* homeEnv = getenv("HOME");
  //  char* logName = getenv("LOGNAME");
    char* path = getenv("PATH");
   // debug("Home Environment : %s\n",homeEnv);
    //debug("LogName Environment : %s\n",logName);
    debug("Paths Environment : %s\n",path);
    debug("PWD : %s\n", pwd);
   /* char** ptr = envp;
    while(ptr != NULL)
    {
        debug("envp contents : %s\n",*ptr);
        ptr = ptr+1;
    }*/
  //  int boolean = chdir(strcat(pwd,"/tests"));
   // debug("change d? : %i\n", boolean);

    char** cwd = (char**) malloc((PATH_MAX+1)*sizeof(char*));
    char buffer[PATH_MAX +1] = "";
    *cwd = getcwd(buffer,PATH_MAX+1);
    debug("Current Directory: %s\n",*cwd);
    setenv("OLDPWD","",1);

    char** oldDir = (char**) malloc((PATH_MAX+1)*sizeof(char*));
     *oldDir = getenv("OLDPWD");

     struct stat *buff = (struct stat*)malloc(sizeof(struct stat));

    signal(SIGTSTP,block_handler);          //ctrl z signal
    signal(SIGUSR2,user2_handler);          //user signal


    //child signal
    struct sigaction newaction;

    newaction.sa_sigaction = &child_handler;
    newaction.sa_flags = SA_SIGINFO;

    if(sigaction(SIGCHLD,&newaction,NULL) < 0)
        unix_error("Signal error");
    //signal(SIGCHLD,child_handler);
    //getcwd()
   // pwd = "/SampleDir $";
    while(1)
    {
        pidShell = getpid();
        pwd = getenv("PWD");
        debug("oldPWD : %s\n",*oldDir);
        debug("PWD : %s\n",pwd);
        debug("PID : %d\n",pidShell);
       // if (strcmp(cmd, "exit")==0)
        //    break;
        fprintf(stdout,"pechou : ");
        printf("%s",*cwd);
        if((cmd = readline(" $ ")) != NULL)
        {
            if(*cmd == '\0')
            {
                errno = ENOENT;
               fprintf(stderr,"%s\n",strerror(errno));
            }

            debug("%s\n",cmd);
            if(*cmd == '/')
            {
                stringArray = parsing(cmd);
                if(stat(cmd,buff)!=0)
                    fprintf(stderr,"%s\n",strerror(errno));

                else
                {

                    int child_status = 0;
                    pid_t pid;

                    if((pid = fork())== 0)
                    {
                        execve(cmd,stringArray,envp);
                        exit(0);
                    }
                    else
                        waitpid(pid,&child_status,0);
                }
            }

            else
            {
                stringArray = parsing(cmd);

                /* All your debug print statements should use the macros found in debu.h */
                /* Use the `make debug` target in the makefile to run with these enabled. */
              //  info("Length of command entered: %ld\n", strlen(cmd));

                char ** tempArray = stringArray;

                while(*tempArray != NULL)
                    {
                         debug("Strings inside cmd: %s\n",*tempArray);
                        tempArray++;
                    }
                tempArray = stringArray;

                if(*tempArray != NULL)
                {
                    char* builtinCmd = *stringArray;                    //get first string which indicates the command
                    validateCmd(builtinCmd,stringArray,cwd,oldDir,path);   //Commences the command, if not a command, print invalid command
                    pwd = *cwd;
                    setenv("PWD",pwd,1);
                    setenv("OLDPWD",*oldDir,1);

                    free(cmd);
                    free(stringArray);
                }
                else{}

            }

            /* You WILL lose points if your shell prints out garbage values. */
        }
    }


    /* Don't forget to free allocated memory, and close file descriptors. */
   // free(cmd);
   // free(stringArray);
        free(oldDir);
        free(cwd);
        free(buff);

    return EXIT_SUCCESS;
}
