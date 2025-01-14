// Jackie Cheng
// jc4653
// 1/12/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);

//prototypes for functions to handle required functionality
int  setup_buff(char *, char *, int);
int  count_words(char *, int, int);
void reverse_user_string(char* buff, int stringLength);
void print_all_words(char *buff, int bufferLength, int stringLength);
//add additional prototypes here

  /** Copies the users string into a buffer that properly cleans consecutive whitespaces
   * and fills the remaining space with periods.
   * 
   * 
   * @param buff A pointer to the character buffer for the string that will be processed.
   * @param user_str A pointer to the users string that needs to be processed.
   * @param len The length of the buffer.
   * @return The length of the string that the entered upon executed or
   * a negative return code to indicate some listed error condition, if
   * the user supplied string is too large
   */
int setup_buff(char *buff, char *user_str, int len) {
    int lengthOfUserString = 0;
    char* pointerOfEachLetter = user_str;
    char* bufferPointerToEnd = buff;
    int consecutiveWhitespace = 0;
    while (*pointerOfEachLetter != '\0'){
        if(*pointerOfEachLetter == ' ' || *pointerOfEachLetter == '\t'){
            if (consecutiveWhitespace == 0){
                // Adds space for the provided string
                *bufferPointerToEnd = ' ';
                bufferPointerToEnd++;
                lengthOfUserString++;
                consecutiveWhitespace = 1;
            }
            // This is under the condition that there are consecutive whitespace.
        }else{
            *bufferPointerToEnd = *pointerOfEachLetter;
            bufferPointerToEnd++;
            lengthOfUserString++;
            consecutiveWhitespace = 0;
        }
        pointerOfEachLetter++;
    }
    if (lengthOfUserString >= len){
        exit(-1);
    }
    while (bufferPointerToEnd < buff + len){
        *bufferPointerToEnd = '.';
        bufferPointerToEnd++;
    }
    return lengthOfUserString;
    //TODO: #4:  Implement the setup buff as per the direction
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

  /** Counds the number of words in a buffered string.
   * 
   * 
   * @param buff A pointer to the character buffer for the string that will be processed.
   * @param len The length of the buffer.
   * @param str_len The string to be processed.
   * @return The number of words that were counted in the buffered string
   */
int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    // this doesnt fully work
    if (str_len > len){
        printf("The length of the provided string is larger than the buffer length");
        exit(-1);
    }
    int wordCounter = 0;
    int startOfWord = 1;
    for (int eachCharacter = 0; eachCharacter < str_len; eachCharacter++){
        if (startOfWord){
            wordCounter++;
            startOfWord = 0;
        }
        char currentBufferCharacter = buff[eachCharacter];
        if(currentBufferCharacter == ' '){
           startOfWord = 1;
        }else{
            continue;
        }
    }
    return wordCounter;
}

/**
 * Reverses the letters in the provided buffer and prints the reversed string.
 * 
 * @param buff A pointer to the character buffer for the string that will be processed.
 * @param stringLength The length of the string to be reversed.
 */
void reverse_user_string(char* buff, int stringLength){
    //poss cant use putchar
    printf("Reversed String: ");
    for (int eachLetter = stringLength - 1; eachLetter >= 0; eachLetter--){
        if (buff[eachLetter] != '.'){
            putchar(buff[eachLetter]);
        }
    }
    putchar('\n');
}

/**
 * Prints all words from the provided buffer along with their lengths.
 * A word is defined as a sequence of characters separated by spaces or punctuation.
 * 
 * @param buff A pointer to the character buffer for the string that will be processed.
 * @param bufferLength The length of the buffer.
 * @param stringLength The length of the string to be processed.
 */
void print_all_words(char *buff, int bufferLength, int stringLength){
    if (stringLength > bufferLength){
        printf("The length of the provided string is larger than the buffer length");
        exit(-1);
    }
    if (stringLength == 0){
        printf("Word Count: 0\n");
        exit(0);
    }
    printf("Word Print\n");
    printf("----------\n");
    int wordCounter = 0;
    int startOfWord = 1;
    int characterCounter = 0;
    int indexOfNewWord = 0;
    for (int eachCharacter = 0; eachCharacter < stringLength; eachCharacter++){
        char currentBufferCharacter = buff[eachCharacter];
        if (currentBufferCharacter != ' ' && currentBufferCharacter != '.'){
            if (startOfWord){
            indexOfNewWord = eachCharacter;
            wordCounter++;
            startOfWord = 0;
            }
        characterCounter++;
        }else{
            if (startOfWord == 0){
                printf("%d. ", wordCounter);
                for (int startingWordIndex = indexOfNewWord; startingWordIndex < eachCharacter; startingWordIndex++){
                    printf("%c", buff[startingWordIndex]);
                }
                printf(" (%d)\n", characterCounter);
                startOfWord = 1;
                characterCounter = 0;
            }
        }
    }
    // This prints the last word, as the initial traversal does not account for the length of the last word
    if (startOfWord == 0){
        printf("%d. ", wordCounter);
        for (int startingWordIndex = indexOfNewWord; startingWordIndex < stringLength; startingWordIndex++){
            printf("%c", buff[startingWordIndex]);
        }
        printf(" (%d)\n", characterCounter);
    }
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    /*
    This conditional statement is safe because we are preventing potential segmentation faults from accessing memory
    of non-existent indexes. For instance, if we simply executed the program with one argument and the second arugment
    does not exist, which should be our option flag, given that we did not add this error handling, the pointer to this arugment
    would be deferenced, leading to memory-related errors. Instead, if we check for the number of arguments being over or equal to 2,
    as well as ensuring that if the second argument did exist, the option would be indicated by the proper option delimiter (-), so
    proper error handling can be provided to the user.
    */

    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }


    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /* The purpose of this if-statement is to check for the proper number of arguments. Regardless of 
    of the flag that we are trying to execute with our program, there needs to be at least 3 arguments. One of
    the arugments is for the name of the executable, another argument keeps track of the option flag for the program.
    Last but not least, the third argument needs to be the string that the users provides for the program. Without the user
    explicitily adding 3 arguments, our program would not have enough information to work properly.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL){
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            reverse_user_string(buff, BUFFER_SZ);
            break;
        case 'w':
            print_all_words(buff, BUFFER_SZ, user_str_len);
            break;
        case 'x':
            printf("Not Implemented!\n");  //you need to implement
            break;
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }
    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
/*
It is a good practice to provide the pointer and the length of the buffer as parameters to these helper functions
because we cannot always promise that the buffer size will be 50 bytes, as the program starts to scale. Suppose
that we want to have a buffer size of 100 or a buffer size that actively changes from the length of the word,
it would be wiser to provide the pointer and the length, so that we do not have to make many changes to the function
and the program would still work properly. Since we know the length of the array, we can still work with the buffer and
reduce the risk of writing beyond the allocated buffer size, so that possible crashes can be avoided as well.
From looking at this issue from a production perspective, it is important to create code that can be reusable because
aspects like tech debt and optimizations become much harder if the source code contains non-dynamic functionality. Expect
the unexpected.
*/

