//DASH source code - CS 4348 Project 1 - Samuel Chen - sxc144230

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//global variable for error message
char error_message[30] = "An error has occurred\n";

int main(int argc, char **argv)
{
  //declare variables to hold input
  char *input; //holds entire command line input
  char **tokens; //holds a list of tokens of the command line input
  int num_commands = 0; //holds number of commands 
  
  //variable that governs the while loop
  int loop = 1;

  //function prototypes
  char *reader();
  char **tokenizer(char*, int*);
  int execute(char**, int);
  
  //begin while loop
  while(loop){
    printf("dash> ");
    input = reader(); //call reader function to read user input
    tokens = tokenizer(input, &num_commands); //split input into tokens
    loop = execute(tokens, num_commands); //execute tokenized commands
    num_commands = 0; //reset counter
    //free previously allocated memory for input and tokens
    free(input);
    free(tokens);
  }
    
  return 0;
}

//reader function
char *reader(){
  char *input = NULL;
  size_t bufsize = 0;
  getline(&input, &bufsize, stdin);
  return input;
}

//tokenizer function
char **tokenizer(char *input, int *num_commands){

  //variable to hold position in array of char pointers
  int position = 0;

  //array of char pointers - contains list of C strings
  char **tokens;

  //variable that saves C string returned by strtok
  char *token;

  //allocate memory for "tokens"
  tokens = calloc(1, sizeof(char *));
  token = strtok(input, "&\r");  

  //begin tokenizing the command line
  while (token != NULL){
    (*num_commands)++; //increment command count 
    tokens[position] = token; //save token into array of pointers
    position++; //increment the position in the array of pointers
    //allocate enough memory for the next token
    tokens = realloc(tokens, sizeof(char *) * (position + 1));
    token = strtok(NULL, "&\r"); //pass NULL to advance to the next token
  }

  tokens[position] = NULL; //set final element to NULL to terminate the array

  return tokens;
}

//execute function
int execute(char** tokens, int num_commands){
  int i;
  int position = 0;
  char *token2;
  char **tokens2;

  for(i = 0; i < num_commands; i++){
    int pid = fork(); //call fork to create new process

    if (pid < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }else if (pid == 0){ //now inside child process
      tokens2 = calloc(1, sizeof(char *));
      token2 = strtok(tokens[i], " \t\n");
      while (token2 != NULL){
	tokens2[position] = token2;
	position++;
	tokens2 = realloc(tokens2, sizeof(char *) * (position + 1));
	token2 = strtok(NULL, " \t\n");
      }
      tokens2[position] = NULL;
      if((execvp(tokens2[0], tokens2)) == -1){ //use execvp to execute command
	write(STDERR_FILENO, error_message, strlen(error_message));
      }
     }
  }

  for (i = 0; i < num_commands; i++){
    wait(NULL); //wait for child process to finish
    free(tokens2); //free tokens2 after child finishes
  }
  
  return 1;
}
    

