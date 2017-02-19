#include "hw2.h"

int main(int argc, char *argv[])
{
    FILE* DEFAULT_INPUT = stdin;
   // FILE* DEFAULT_OUTPUT = stdout;
    char DEFAULT_DICT_FILE[]= "rsrc/dictionary.txt";
     debug("%s\n",DEFAULT_DICT_FILE);

   /*  struct dictionary Diction;
     Diction.num_words = 0;
     Diction.word_list = NULL;*/

    //create dictionary
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
    //dict = &Diction;
    if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word*))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
   m_list = NULL;


 if((dict->word_list = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }


    //m_list->misspelled = 0;
    //m_list->correct_word = NULL;
    //m_list->next = NULL;

   // dict = NULL;
    //dict->num_words = 0;
    dict->word_list = NULL;

    struct Args args;
    // Set struct default values
    args.h = false;
    args.d = false;
    args.i = false;
    args.o = false;
    args.An= false;
    strcpy(args.dictFile, DEFAULT_DICT_FILE);

    char line[MAX_SIZE] = "\0";
    //Declare Files
    FILE* dFile = NULL;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = stderr;

   /* char opt = '\0';
    for(int i = 1; i<= argc; i++)
    {
        char* currArg = argv[i];
        //there's a flag
        if(opt != '\0')
        {
            if(opt == 'h')
            {
                USAGE(EXIT_SUCCESS);
                args.h = true;
            }
            if(opt == 'd')
            {
                strcpy(args.dictFile, currArg);
                args.d = true;
            }
            if(opt == 'i')
            {
                strcpy(args.input, currArg);
                args.i = true;
                iFile = fopen(currArg, "r");
            }
            if(opt == 'o')
            {
                strcpy(args.output, currArg);
                args.o = true;
                oFile = fopen(currArg, "w");
            }
            if(opt == 'A')
            {
                args.An = true;

            }
            opt = '\0';
        }
        else
        {
            if(strcmp(currArg, "-h") == 0)
                opt = 'h';
            if(strcmp(currArg, "-d") == 0)
                opt = 'd';
            if(strcmp(currArg, "-i") == 0)
                opt = 'i';
            if(strcmp(currArg, "-o") == 0)
                opt = 'o';
            if(strcmp(currArg, "-A") == 0)
                opt = 'A';
        }
    }*/
    char opt = '\0';
    while((opt = getopt(argc,argv,"hd:i:o:A:"))!= -1)
    {
        //printf("%c",opt);
        switch(opt)
        {
            case 'h':
                args.h = true;
                break;
            case 'd':
                strcpy(args.dictFile, optarg);
                args.d = true;
                break;
            case 'i':
                strcpy(args.input, optarg);
                args.i = true;
                break;
            case 'o':
                strcpy(args.output, optarg);
                args.o = true;
                break;
            case 'A':
                args.n = atoi(optarg);
                args.An = true;
                break;
            //default: /* ? */ USAGE(EXIT_FAILURE);
        }

    }

    if(args.h == true)
    {
        USAGE(EXIT_SUCCESS);
    }

    int numofmispellings;
    if(args.An == true)
    {
        if(args.n > 5)
            return EXIT_FAILURE;
        numofmispellings = args.n;
    }

    iFile  = fopen(args.input, "r");
    if(iFile == NULL && args.i == true)
    {
        USAGE(EXIT_FAILURE);
       /* printf("Unable to open: %s.\n", args.input);
        return EXIT_FAILURE;*/
    }
    dFile = fopen(args.dictFile, "r");
    if(dFile == NULL)
    {
        USAGE(EXIT_FAILURE);
        /*printf("Unable to open: %s.\n", args.dictFile);*/
    }
    else
    {
        processDictionary(dFile);

    }

    strcpy(line,"\n--------INPUT FILE WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);

    while(!feof(iFile))
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;

        fgets(line, MAX_SIZE+1, iFile);

      //  printf("%s\n",line);
        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        //replaces spaces within a line with new lines
        while(*character != '\0')
        {
            debug("%c", *character);
            if(*character == ' ' || *character == '\n')
            {
                /*char* punct = wdPtr-1;
                    printf("char:%c",punct);
                while(!((*punct>='a' && *punct<='z') || (*punct>='A' && *punct<='Z')))
                {
                    punct--;
                }
                punct++;
                printf("%d", strlen(wdPtr)-strlen(punct));
                */


                *wdPtr = '\0';
                wdPtr = word;

                processWord(wdPtr,numofmispellings);

                strcat(wdPtr, " ");
                fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);
            }
            else
            {
                int lowered = tolower((int)*character);
                *(wdPtr++) = (char)lowered;
            }
            character++;
        }

       // if(iFile == stdin)
        //    break;
    }
    if(oFile == stdout)
    {
        oFile = stderr;
    }

    strcpy(line, "\n--------DICTIONARY WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);
    printWords(dict->word_list , oFile);
    FILE* newDict = NULL;
    //char filename[MAX_SIZE] = {0};
    //sprintf(filename,"rsrc/new_dictionary.txt");
    if(args.An == true)
    {

  //  char *base = basename(DEFAULT_DICT_FILE);       //take dictionary file name

    //char *prefix = dirname(DEFAULT_DICT_FILE);      //take the directory that leads to dictionary
   // char *prefix = "rsrc/new_";
  //  *(prefix + strlen(prefix)) = '/';
    //char* newprefix = strcat(prefix,"/");    //cat them together
    //char* newprefix2 = strcat(prefix,"new_");
   // char* newbase = strcat("new_",base);
   // char *newDictname = strcat(newprefix,newbase);
    newDict = fopen("rsrc/new_dictionary.txt","w");
    struct dict_word* wordlistptr = dict->word_list;

    while(wordlistptr != NULL)
    {
    char* Dword = dict->word_list->word;
    fprintf(newDict,"%s",Dword);
   // fwrite(Dword,1,sizeof(Dword) ,newDict);
    int numofmispellings = (dict->word_list->num_misspellings);
    while(numofmispellings!= 0)
    {
        char *mispelledword = dict->word_list->misspelled[numofmispellings -1]->word ;
        if(numofmispellings >1)
        {
        fprintf(newDict, "%s",mispelledword);
        }
        else
        {
        fprintf(newDict,"%s\n",mispelledword);
        }
       // fwrite(mispelledword,1,sizeof(,newDict);
        numofmispellings--;
    }
    //fprintf(newDict,"%c",'\n');
    //fwrite("\n",1,sizeof(char),newDict);
    wordlistptr = wordlistptr->next;
    }
    }

  //  createNewDictFile(newDict,dict->word_list);

    //f("\n--------FREED WORDS--------\n");
    freeWords(dict->word_list);
    //free dictionary
    free(dict);
    //free m_list
    free(m_list);

   // fclose(newDict);
    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}

