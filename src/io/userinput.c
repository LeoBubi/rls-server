#include "includes.h"


#define UIN_INITBUFSIZ 8


char* 
userinput(char* prompt) 
{
    // print the prompt if it is not NULL
    if (prompt != NULL) {
        printf("%s", prompt);
    }

    size_t input_maxsize = UIN_INITBUFSIZ;  // initial size of input buffer
    size_t input_size    = 0;               // initial size of input

    // allocate initial buffer for the input string
    char *input = (char*)malloc(input_maxsize);
    if (!input) {
#ifdef __DEBUG
        perror("userinput: malloc");
#endif
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) 
    {
        if (input_size == input_maxsize)
        {
            input_maxsize *= 2;
            char *tmp = (char*)realloc(input, input_maxsize);
            if (!tmp) {
#ifdef __DEBUG
                perror("userinput: realloc");
#endif
                free(input);
                return NULL;
            }
            input = tmp;
        }

        input[input_size++] = c;
    }

    input[input_size] = '\0'; // null-terminate the input string

    return input;
}
