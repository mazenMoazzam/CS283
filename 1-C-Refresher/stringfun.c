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

//The setup_buff function processes a string by transferring characters from user_str to buff, stripping out
//the extra whitespaces and tabs to leave only single spaces between words. This function ensures that the 
//output string does not exceed the specified length, len. If the input string is longer than len, it returns
//-1 to indicate the buffer overflow. If the user inputted string is null or empty, it returns a -2 status code.
//If everything processes, it returns the length of the inputted string. The function after validation, 
//pads the buffer with dot characters, if the 
//processed input is shorter than the len (which is essentially the buffer size), which will prepare it for 
//consistent outputs and formatting for the other commands available.

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    if (user_str == NULL || *user_str == '\0') {
        return -2; 
    }

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
    return (ptrBuff - buff);  
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



//The count words function below is used to count the number of words in the provided buffer in the function. 
//To effectively count the words in the buffer string, ptrBuff is created first to start at the first character of the
//buffer to begin the process, endOfWord variable acts as a boolean variable to indicate or track when it exits a word
//boundary, which is defined by spaces or periods. This variable also helps correctly increment the word count. A for
//loop is initilized to iterate through each character in the string to determine the number of words using pointers.
//The loop checks if the current character is not a space or a period for the sole reason to distinguish it from the 
//end of a word, if the conditions are met, end of word is set to a different state (1) and the 
//wordcount is incremented. If the word is a space or period, it indicates the end of a word, 
//and the loop resets the endOfWord variable. The loop is until it reaches the end of the string in the buffer and 
//the word count variable is returned to obtain the value.

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



//The purpose of the getLengthOfString function is to determine the length of the string in the buffer disregarding 
//the filler dots in the buffer. By using pointers start at the first character of the buffer, a counter is set to 0
//and a loop is made to loop until the current character pointer reaches a period, which indicates the end of the 
//string in the buffer, in this loop, the character pointer is incremented to traverse through the string and the 
//length counter is increment by one to indicate an increase in string length, the length counter is returned at the
//end of the function to indicate the true length of the string.

int getLengthOfString(char* buff, int totalLength) {
	int len = 0;
	char* currentChar = buff;
	while ((currentChar - buff) < totalLength && *currentChar != '.') {
		currentChar++;
		len++;
	}
	return len;
}


// The reverse function above, takes two parameters and is used to reverse the string in the buffer. Start and end variables are initialized to 
// indicate the first character in the word and the last character in the word. A while loop is made to loop until both start and end reach
// the middle of the string, in this loop, the characters are swapped with a temp variable to reverse the string and the variables are incremented 
// and decremented to move to the next character. StrLen parameter is used to calculate the last character of the string by adding the buff address
// to the length of the string minus 1 to give the last character address. 

void reverse(char* buff, int strLen) {
	char* start = buff;
	char* end = buff + strLen - 1;
	while (start < end) {
		char temp = *start;
		*start = *end;
		*end = temp;
		start++;
		end--;
	}

	printf("Reversed: ");
	char* current = buff;
    	while (current < buff + strLen) {
        	putchar(*current);
        	current++;
    	}
    	putchar('\n'); 
}


//The word print function below is made to analyze the given buffer in the function parameter containing a string,
//it also identifies each word, and prints the detailed information about each word, including length and the word
//itself. It initilizes counters and pointers to track the number of words, starts at the current word and its length.
//As it iterates through the string, it identifies the characters that belong to a word (not a space, period or etc.) 
//and counts them. When it encounters a specific delimeter  (space or punctuation), it checks if it has finished
//processing a word. If it has, it prints the words position in text, the word, and its length before resetting the 
//counter. This behavior continues until it has reached the end of the string. 

void wordPrint(char* buff, int strLen) {
    int wordNum = 1;  
    char* start = buff;  
    char* currentChar = buff;  
    int wordLen = 0;  
    printf("Word Print\n");
    printf("----------\n");
    while (currentChar - buff < strLen) {
        if ((*currentChar != ' ' && *currentChar != '.' && *currentChar != '\0')) {
            if (wordLen == 0) { 
                start = currentChar; 
            }
            wordLen++;  
        } else {
            if (wordLen > 0) {
                printf("%d. ", wordNum++);
                while (start != currentChar) {
                    putchar(*start);
                    start++;
                }
                printf(" (%d)\n", wordLen);  
                wordLen = 0;
            }
        }
        currentChar++; 
    }

    if (wordLen > 0) {
        printf("%d. ", wordNum);
        while (start != currentChar) {
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
        printf("Error: Provided input string is too long\n");
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
