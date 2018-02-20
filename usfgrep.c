#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int multiFile = 0;
int currLine = 0;

struct line {
    int len;
    int done;
    int lineNum;
    int locations[512];
    char *buffer;
};


void printBuffer(char buffer[], int len, int location[], char *grep);

//Generate and return a file with failure check.
int openFile(char *filename){
    int file;

    file = open(filename, O_RDONLY);

    if (file < 0) {
        printf("Unable to open %s.\n", filename);
        exit(-1);//status
    }
    
    return file;
}


//If possible read line and return line if contains GREP request
struct line readLine(int file, char *grep){
    char currChar = 0; //holds char value
    char buffer[512]; //holds line
    int bytes_read; //holds read bytes
    int len = 0; //len of line
    int chars_read = 0; //how many chars we have read so far
    int find_location = -1; //start of the most recent grep find
    int found = 0; //have we found grep yet
    int grep_len = strlen(grep); //size of grep
    int curr_grep = 0; //current location in grep check
    struct line line; //line struct
    currLine++;

    while (chars_read++, (bytes_read = read(file, &currChar, 1)) > 0) { //returns -1 if failure otherwise bytes read
        if (currChar == '\n') { //check if done with line
            break;
        }
        buffer[len++] = currChar;  

        if(grep[curr_grep] == currChar && curr_grep+1 == grep_len){ //if we have finished the grep check 
            line.locations[found] = find_location;
            find_location = -1;
            found++;
            curr_grep = 0;
        }else if(grep[curr_grep] == currChar && curr_grep+1 != grep_len){ //set find location grep check is continuing 
            if(find_location == -1){
                find_location = chars_read-1;
            }   
            curr_grep++;
        }else{ //failed grep check keep trying
            find_location = -1;
            curr_grep = 0;
        }
    }

   
    line.buffer = buffer;
    if(found != 0 || len == 0){
        line.len = len;
        line.lineNum = currLine;
    }
    else{
        line.len = -1;
    }
    return line;

}

//reads each line of a given file and outputs to console 
void readFile(int file, char *grep, char *file_name){
    struct line line;
    while((line = readLine(file, grep)), line.len != 0){

        if(line.len != -1){
            if(multiFile != 0){
                printf(ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_RESET, file_name);
                printf(ANSI_COLOR_BLUE":"ANSI_COLOR_RESET);
                printf("%d", line.lineNum);
                printf(ANSI_COLOR_BLUE":"ANSI_COLOR_RESET);
            }else{
                printf("%d:", line.lineNum);
            }printBuffer(line.buffer, line.len, line.locations, grep);
        }
    }currLine = 0;
}



//prints out provided buffer
void printBuffer(char buffer[], int len, int locations[], char *grep){
    int x, started;
    int loc = 0;
    for(x = 0; x < len; x++){
        if(x >= locations[loc] && x < locations[loc]+strlen(grep)){
            started = 1;
            printf(ANSI_COLOR_RED"%c"ANSI_COLOR_RESET, buffer[x]);
        }else{
            if(started==1){
                started = 0;
                loc++;
            }
            printf("%c", buffer[x]);
        }
    }printf("\n");
}


//main function env
int main(int argc, char *argv[]){
    int x, file; 
    
    if(argc >= 3){
        if(argc > 3){
            multiFile = 1;
        }
        char *grep = argv[1];
        for(x = 2; x < argc; x++){ //loop file args
            char *file_name = argv[x];
            file = openFile(argv[x]);
            readFile(file, grep, file_name);
            close(file);


        }
    }else{
        printf("%s\n", "Insufficient Args");
        printf("%s\n", "Usage: usfgrep <string> <file1> [<file2> ...]");
    }
   
    return 0;
}

