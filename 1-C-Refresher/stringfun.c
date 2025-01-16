#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFER_SZ 50 


//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);
//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int getLengthOfString(char* buff, int totalLength);
void reverse(char* buff, int strLen);
void wordPrint(char* buff, int strLen);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char* ptrBuff = buff;
    const char *ptrUserStr = user_str; 
    char lastCharacter = 0;   
    while (*ptrUserStr != '\0' && (ptrBuff - buff) < len) {
	    if (*ptrUserStr != ' ' && *ptrUserStr != '\t') {
	    	*ptrBuff++ = *ptrUserStr;
	    	lastCharacter = *ptrUserStr;
	    } else if (lastCharacter != ' ' && lastCharacter != 0) {
		    *ptrBuff++ = ' ';
		    lastCharacter = ' ';
	    }
	    ptrUserStr++;
    }

    if (*ptrUserStr != '\0') {
	    return -1;
    }

    while((ptrBuff - buff) < len) { 
	    *ptrBuff++ = '.';
    }
    return (ptrBuff - buff); //for now just so the code compiles. 
}


void print_buff(char *buff, int len){
    printf("Buffer: ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}


void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}


int count_words(char *buff, int len, int str_len){
    int wordCount = 0;
    int endOfWord = 0;
    char* ptrBuff = buff;
    for (int i = 0; i < str_len; i++, ptrBuff++) {
	    if ((*ptrBuff != ' ' && *ptrBuff != '.') && !endOfWord) {
		    endOfWord = 1;
		    wordCount++;
	    } else if (*ptrBuff == ' ' || *ptrBuff == '.') {
		    endOfWord = 0;
	    }
    }
    return wordCount;
}



//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int getLengthOfString(char* buff, int totalLength) {
	int len = 0;
	char* currentChar = buff;
	while ((currentChar - buff) < totalLength && *currentChar != '.') {
		currentChar++;
		len++;
	}
	return len;
}


// The reverse function above, takes two parameters and is used to reverse the string in the buffer.
// Achieves this using pointer notation by calculating the character in the string using strLen and the buffer,
// A while loop is created to iterate from the end of the buff to the start of the buff which is a pointer to the 
// first character, putchar is used to print the current character in reverse as it starts the loop from the last
// character.

void reverse(char* buff, int strLen) {
	printf("Reversed: ");
	char* endOfBuff = buff + strLen - 1;
	while(endOfBuff >= buff) {
		putchar(*endOfBuff);
		endOfBuff--;
	}
	printf("\n");
}


void wordPrint(char* buff, int strLen) {
    int wordNum = 1;  
    char* start = buff;  
    char* current = buff;  
    int wordLen = 0;  

    printf("Word Print\n");
    printf("----------\n");
    while (current - buff < strLen) {
        if ((*current != ' ' && *current != '.' && *current != '\0')) {
            if (wordLen == 0) { 
                start = current; 
            }
            wordLen++;  
        } else {
            if (wordLen > 0) {
                printf("%d. ", wordNum++);
                while (start != current) {
                    putchar(*start);
                    start++;
                }
                printf(" (%d)\n", wordLen);  
                wordLen = 0;
            }
        }
        current++; 
    }
    if (wordLen > 0) {
        printf("%d. ", wordNum);
        while (start != current) {
            putchar(*start);
            start++;
        }
        printf(" (%d)\n", wordLen);
    }
}



int main(int argc, char *argv[]){
    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TDO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    //      The code below is safe because if arv[1] does not exist or does not contain the - symbol character
    //      it will result into an error and the code will result into an error, this is safe due to the fact that
    //      this will prevent a segmentation fault with the source code and will prevent from accessing data from
    //      a non-existent area in memory.
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
    //      The purpose of the if statement below is to check if the correct number of command line arguments
    //      were provided by the user and their input. The if-statement check is to ensure the program behaves
    //      as it is suppose to and for all neccessary inputs for the program, it behaves and operates correctly. 
    //      The less than 3 means the minimum number of arguments for the program to execute correctly is three.
    //      If user doesn't provide the correct number of arguments, the program calls the usage function to display
    //      how to use the program or other helpful information to get the correct input. After this, it exits with
    //      a status code of 1, which indicates it ended with an error (missing x number of arguments etc.).
    
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string
    buff = malloc(BUFFER_SZ * sizeof(char));

    if (buff == NULL) {
	    printf("Memory allocation failed");
	    exit(99);
    }
    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
	case 'r':
	    user_str_len = getLengthOfString(buff, user_str_len);
	    reverse(buff, user_str_len);
	    break;
	case 'w':
	    user_str_len = getLengthOfString(buff, user_str_len);
	    wordPrint(buff, user_str_len);
	    break;
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
//          PLACE YOUR ANSWER HERE---
//          Providing both the pointer and the length is a good practice, even after knowing the buff variable will
//          have 50 bytes, and with passing the length of the buffer, the helper functions are provided with important
//          context about how much data the function can safely access. Because of this action, it will prevent a buffer
//          overflow. In addition, with the functions knowing the length of the data they are working with, it can 
//          handle a wide variety of data sizes more efficiently. This idea is useful when the data size might 
//          dynamically change at runtime or any other scenarios. In addition to efficienty, knowing the length and 
//          having the buffer allows for more efficient processing of the source code, as the function can avoid
//          processing data beyond the intended size or optimize the processing of the function on data size.
