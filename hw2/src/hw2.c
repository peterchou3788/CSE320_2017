#include "hw2.h"

/* Great filename. */

void processDictionary(FILE* f){
    dict->num_words = 0;
    while(!feof(f))
    {
        //initialize the current word.
        struct dict_word* currWord = NULL;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE] = "\0";
        char* wdPtr = word;
        char line[MAX_SIZE] = "\0";
        char* character = line;
        //dict->word_list[MAX_MISSPELLED_WORDS+1][MAX_SIZE];
        //removed word_list because unused
        int counter = 0;
        int firstWord = 1;

        char * fgetptr = NULL;
        fgetptr = fgets(line, MAX_SIZE+1, f);    //streams one line into line
        if(fgetptr == NULL)
        {
           *character = '\0';
        }
       // printf("%s\n",line);
        //if there isn't a space at the end of the line, put one there
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");

        while(*character != '\0')
        {
            //debug("%s\n" ,character);
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            //if the character is a space, add the word in word_list and make word NULL.
            if(*character == ' ')
            {
                *wdPtr = '\0';
                wdPtr = word;
                if(firstWord)
                {
                    addWord(currWord, wdPtr);
                    debug("%s\n",dict->word_list->word);
                    dict->num_words++;

                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling = NULL;
                    if((currMisspelling = (struct misspelled_word*)malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                    wdPtr = word;
                    counter++;
                    //free(currMisspelling);
                }
            }
            //if the character isn't a space or a new line, add the character to word.
            else if(*character != '\n')
            {
                *wdPtr = *character;
                wdPtr++;
            }
            character++;

            //counter++;
        }

    }
  //  dict->word_list->next = NULL;
}

void addWord(struct dict_word* dWord, char* word){
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    strcpy(dWord->word, word);

    if(dict->word_list == NULL)
    {
        dict->word_list = dWord;
    }
    else
    {
       dWord->next = dict->word_list;
        dict->word_list = dWord;
    }
    //dWord->next = dict->word_list;
    //dict->word_list = dWord;

}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    if(m_list == NULL)      //if m_list is empty, assign first mispelled word
    {
        m_list = misspelledWord;
    }
    else                    //else update next pointer from previous mispelled
    {

        m_list[(correctWord->num_misspellings)-1].next = misspelledWord;
    }
    //misspelledWord->next = m_list +1;
    (correctWord->misspelled)[(correctWord->num_misspellings)] = misspelledWord;
    (correctWord->num_misspellings)++;

}

void freeWords(struct dict_word* currWord){
    if(currWord != NULL)
    {

        freeWords(currWord->next);

        //deleted "int i;"
        //free word
        printf("FREED %s\n", currWord->word);
        free(currWord);
    }
}

void printWords(struct dict_word* currWord, FILE* f){
    if(currWord != NULL)
    {
        printWords(currWord->next, f);

        char line[MAX_SIZE];
        //deleted "int i;"

        sprintf(line, "%s\n", currWord->word);
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF TIMES WORD IS MISSPELLED: %d\n", currWord->misspelled_count); // puts string into buffer
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF MISSPELLINGS: %d\n", currWord->num_misspellings);
        fwrite(line, strlen(line)+1, 1, f);

        for(int i = 0; i<currWord->num_misspellings; i++)
        {
            sprintf(line, "\tMISPELLED WORD #%d: %s\n", i,((currWord->misspelled)[i])->word);
            fwrite(line, strlen(line)+1, 1, f);

            sprintf(line,"\t\tMISPELLED?: %d\n", ((currWord->misspelled)[i])->misspelled);
            fwrite(line, strlen(line)+1, 1, f);

            sprintf(line, "\t\tACTUAL WORD: %s\n", (((currWord->misspelled)[i])->correct_word) ->word);
            fwrite(line, strlen(line)+1, 1, f);

            if(((currWord->misspelled)[i])->next->word != NULL)
            {
                sprintf(line, "\t\tNEXT MISPELLED WORD: %s\n", ((currWord->misspelled)[i])->next->word);
                fwrite(line, strlen(line)+1, 1, f);
            }
        }

        if((currWord->next)->word != NULL)
        {
            sprintf(line,"\tNEXT WORD: %s\n", (currWord->next)->word);
            fwrite(line, strlen(line)+1, 1, f);
        }
    }
}

void processWord(char* inputWord, int numofmispellings){
    if(foundMisspelledMatch(inputWord))
        return;
    if(foundDictMatch(inputWord))
        return;
    else
    {
        //char ch;

        //char conf;

       /* do
        {
            printf("\"%s\" was not found in the dictionary. Do you want to add it (Y/N)? ", inputWord);
            scanf("%c", &conf);
            while ((ch = getchar()) != '\n' && ch != EOF);
        }while(conf!='Y' && conf!='N');

        if(conf == 'Y')
        {*/
            struct dict_word* newWord;
            //int counter = 0;
            //removed counter variable

            if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
            {
                printf("ERROR: OUT OF MEMORY.\n");
                return;
            }

            addWord(newWord, inputWord);
            dict->word_list = newWord;
           // printf("Added \"%s\" to Dictionary. Add misspellings (Y/N)? ", inputWord);

            /*do
            {
                scanf("%c", &conf);
                while ((ch = getchar()) != '\n' && ch != EOF);
            }while(conf!='Y' && conf!='N');*/

            //if(conf=='Y')
            //{
                char** typos = gentypos(numofmispellings,inputWord);
                /*do
                {
                    printf("How many misspellings (1-5)?");
                    scanf("%d", &numMisspellings);
                    while ((ch = getchar()) != '\n' && ch != EOF);
                }while(numMisspellings<1 || numMisspellings>5);*/

                while(numofmispellings > 0)
                {
                    char *word = typos[numofmispellings-1];
                    char* wdPtr = word;
                    struct misspelled_word* newMWord;

                    if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }
                    /*if((newMWord->correct_word = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }
                    if((newMWord->next = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }*/
                   // printf("Enter misspelling: ");
                    //scanf("%s", word);
                    addMisspelledWord(newMWord, newWord, wdPtr);
                    printf("Misspelling added\n");
                   // while ((ch = getchar()) != '\n' && ch != EOF);
                    numofmispellings--;
                  /*  free(newMWord->next);
                    free(newMWord->correct_word);
                    free(newMWord);*/
                }

            //}
        //}
    }
}

bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}

