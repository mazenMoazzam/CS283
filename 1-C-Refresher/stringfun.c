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


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int i = 0;
    int bufferIndex = 0;
    char lastCharacter = 0;
    
    while (user_str[i] != '\0' && bufferIndex < len) {
	    if (user_str[i] != ' ' && user_str[i] != '\t') {
	    	buff[bufferIndex++] = user_str[i];
	    	lastCharacter = user_str[i];
	    } else if ((lastCharacter != ' ' && lastCharacter != 0) && bufferIndex > 0) {
		    buff[bufferIndex++] = ' ';
		    lastCharacter = ' ';
	    }
	    i++;
    }

    if (user_str[i] != '\0') {
	    return -1;
    }

    while(bufferIndex < len) { 
	    buff[bufferIndex++] = '.';
    }

    return i; //for now just so the code compiles. 
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

int count_words(char *buff, int len, int str_len){
    int wordCount = 0;
    int endOfWord = 0;


    for (int i = 0; i < str_len; i++) {
	    if ((buff [i] != ' ' && buff[i] != '.') && !endOfWord) {
		    endOfWord = 1;
		    wordCount++;
	    } else if (buff[i] == ' ' || buff[i] == '.') {
		    endOfWord = 0;
	    }
    }
    return wordCount;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int getLengthOfString(char* buff, int totalLength) {
	int len = 0;
	while (len < totalLength && buff[len] != '.') {
		len++;
	}
	return len;
}

void reverse(char* buff, int strLen) {
	char temp;
	for (int i = 0; i < strLen / 2; i++) {
		temp = buff[i];
		buff[i] = buff[strLen - 1 - i];
		buff[strLen - 1 - i] = temp;
	}
	printf("Reversed: ");
	
	for (int i = 0; i < strLen; i++) {
        	if (buff[i] == '.') break;
        	putchar(buff[i]);
    	}
    	printf("\n");

}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
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
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string
    buff = malloc(BUFFER_SZ * sizeof(char));

    if (buff == NULL) {
	    printf("Memory allocation failed");
	    return 99;
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
