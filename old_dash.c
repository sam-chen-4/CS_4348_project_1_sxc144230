//DASH source code - CS 4348 Project 1 - Samuel Chen - sxc144230

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//global variable for error message
char error_message[30] = "An error has occurred\n";

//array for holding built-in commands
const char *built_ins[] = {"exit", "cd", "path"};

int main(int argc, char **argv)
{
  char *PATH = getenv("PATH");
  printf("%s\n", PATH);
  //declare variables to hold input
  char *input; //holds command line input
  char **tokens; //holds a list of tokens of the command line
  int num_commands = 0; //holds number of commands 
  FILE *fptr; //for file operations

  //variable that governs the while loop
  int loop = 1;

  //function prototypes
  char *reader();
  char **tokenizer(char*, int*);
  int execute(char**, int);

  //prototypes for shell built-in commands
  void cd(char **);
  // void path(char **);

  //start in batch mode if argument is passed to main
  if (argc == 2){
    if ((fptr = fopen(argv[1], "r")) == NULL){
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }
    
    //read entire content of input file
    fseek(fptr, 0, SEEK_END);
    long num_bytes = ftell(fptr); //get number of bytes of input file
    fseek(fptr, 0, SEEK_SET); //reset file position to beginning
    char *buffer = calloc(num_bytes, sizeof(char)); //allocate memory for buffer
    fread(buffer, sizeof(char), num_bytes, fptr); //copy text into buffer
    fclose(fptr);
    
    tokens = tokenizer(buffer, &num_commands); //call tokenizer
    execute(tokens, num_commands); //execute tokens from batch file
    free(buffer);
    free(tokens);
    exit(0);
  }
  else if (argc > 2){
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
  }
  
  //begin while loop - interactive mode
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
  token = strtok(input, "&\r:");  

  //begin tokenizing the command line
  while (token != NULL){
    (*num_commands)++; //increment command count 
    tokens[position] = token; //save token into array of pointers
    position++; //increment the position in the array of pointers
    //allocate enough memory for the next token
    tokens = realloc(tokens, sizeof(char *) * (position + 1));
    token = strtok(NULL, "&\r:"); //pass NULL to advance to the next token
  }

  tokens[position] = NULL; //set final element to NULL to terminate the array

  return tokens;
}

//execute function
int execute(char** tokens, int num_commands){
  int i = 0;
  int position = 0;
  char *token2;
  char **tokens2;
  char *path = getenv("PATH");
  char **path_tokens;
  char *path_buffer;
  int status;

  //function declarations
  void cd(char **); //for cd built-in command
  char **tokenizer(char*, int*);

  //note: working on getting PATH and tokenizing to pass the path to execv
  path_tokens = tokenizer(path, &num_commands);   

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
      
      path_buffer = malloc(strlen(path_tokens[2]) + strlen(tokens2[0]) + strlen("/") + 1);
      if (path_buffer != NULL){
	strcpy(path_buffer, path_tokens[2]);
	strcat(path_buffer, "/");
	strcat(path_buffer, tokens2[0]);
      }
      
      printf("%s\n", path_buffer);

      //Check for built-in commands
      /* for(i = 0; i < 3; i++){
	if (strcmp(tokens2[0], built_ins[i]) == 0){
	  if(strcmp(tokens2[0], "exit") == 0)
	    exit(0);
	  else if(strcmp(tokens2[0], "cd") == 0){
	    cd(tokens2);
	    return 1;
	  }
	  /*else if(strcmp(tokens2[0], "path") == 0){
	    path(tokens2);
	    return 1;
	    }*/

      if((execv(path_buffer, tokens2)) == -1){ //use execv to execute command
	write(STDERR_FILENO, error_message, strlen(error_message));
      }
    }
  }

  for (i = 0; i < num_commands; i++){
    wait(NULL); //wait for child process to finish
    /* if(WIFEXITED(status))
       return 0;*/
    free(tokens2); //free tokens2 after child finishes
    //    free(path_buffer);
  }
  
  return 1;
}
    
//built-in command cd
void cd(char **tokens2){
  if(tokens2[2] != NULL){
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
  else if(chdir(tokens2[1]) != 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
}

//built-in command path

