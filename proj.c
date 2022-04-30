#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 100
#define TOKEN_BUFFER_SIZE 50        // Upper bound since l is a command and you can technically send fifty of them.
#define HESH_TOK_DELIM " \t\r\n\a"  // \t is tab, \r is carriage return, \n is line feed, \a is alert. (I thinks)
#define MAX_MATCHES 1

// Declarations of built-in commands
int hesh_print();
int hesh_help();
int hesh_exit();

// List of built-in commands and their cooresponding functions
char *builtin_str[] = 
{
  "print",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = 
{
  &hesh_print,
  &hesh_help,
  &hesh_exit
};

// Built in commands section
int hesh_print()
{
  printf("PID of current process: %d\n", getpid());
  printf("PPID of current process: %d\n", getppid());
  return 1;
}

int hesh_help()
{
  int i;
  printf("Hunter Barndt's and Eric Capri's hesh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < hesh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  printf("\n");
  printf("Beware - hesh stares into your soul...      ◉_◉\n");
  return 1;
}

int hesh_exit()
{
  return 0;
}

//// Purpose: To return the number of built in functions 
//// Returns: An int representing the amount of built in functions
int hesh_num_builtins() 
{
  return sizeof(builtin_str) / sizeof(char *);
}

//// Purpose: Print char array
//// Returns: Void
void print_char_array(char* arr)
{
  for(int i = 0; i < strlen(arr); i++)
  {
    printf("%c", arr[i]);
  }
  printf("\n");
}

//// Purpose: Print string array of length 5
//// Returns: Void
void print_string_array_5(char** arr)
{
  int c = 0;
  for(int i = 0; i < 5; i++)
  {
    c = 0;
    while(*(arr[i] + c) != '\0')
    {
        printf("%c", *(arr[i] + c));
        c++;
    }
    printf(" ");
  }  
}

//// Purpose: Function to trim a char array given a char array, original size, and new size.
//// Returns: Void
void trim_char_array(char* line, int og_amount, int cut_amount)
{
  printf("---------------------------------------------\n");
  printf("Entering trim_char_array function...\n");
  printf("\n");
  printf("The input line of length %d is:\n", og_amount);
  print_char_array(line);
  char* temp_line = malloc(cut_amount + 1);
  memcpy(temp_line, line, cut_amount);
  memset(line, 0, og_amount);
  memcpy(line, temp_line, cut_amount);
  printf("The new cut line of length %ld is:\n", strlen(line));
  print_char_array(line);
  printf("\n");
  free(temp_line);
  printf("Exiting trim_cut_array function...\n");
  printf("---------------------------------------------\n");
  printf("\n");
}

//// Purpose: Function to read the full line of input from stdin
//// Returns: Char array of read line
char *hesh_read_line(void)
{
  // Defines the line and buffer size variable
  char *line = NULL;
  size_t buffsize = 0; // getline will allocate a buffer for us
  
  // Defining regular expression and the regex_t object
  char* nono_chars = "[^ &a-zA-Z0-9._/-]+";
  regex_t re;
  regmatch_t matches[MAX_MATCHES];

  // Stores the white space deliminated line into the line variable
  // If the getline returns -1 error in getline
  if (getline(&line, &buffsize, stdin) == -1)
  {
    // If end-of-file indicator is set, exit 
    if (feof(stdin)) 
      exit(EXIT_SUCCESS);  
    else  
    {
      perror("readline\n");
      exit(EXIT_FAILURE);
    }
  }

  // If the input line is greater than 100 characters then trim the array
  int og_line_length = strlen(line);
  if(og_line_length > INPUT_BUFFER_SIZE + 1)
  {
    printf("Error: Too many arguments in input.\n");
    printf("Cutting the input to 100 characters.\n");
    trim_char_array(line, og_line_length, INPUT_BUFFER_SIZE);
  }

  // If the regular expression compilation fails, print error and exit
  if(regcomp(&re, nono_chars, REG_EXTENDED) != 0)
  {
    fprintf(stderr, "Error: regcomp in hesh_read_line\n");
    exit(EXIT_FAILURE);
  }
  else
    printf("Regex compilation successful!\n");

  // Execute finding regular expressions in the inputted line, if there is a matched character then trim to the chracter before it.
  if(regexec(&re, line, MAX_MATCHES, matches, 0) == 0)
  {
    printf("Error: Typed an erroneous character, will cut input line to before that character\n");
    printf("The erroneous substring \"%.*s\" is found at position %d to %d.\n", matches[0].rm_eo - matches[0].rm_so, &line[matches[0].rm_so], matches[0].rm_so, matches[0].rm_eo - 1);
    trim_char_array(line, strlen(line), matches[0].rm_so);
  }
  else
    printf("No matches were found.\n");

  regfree(&re);
  return line;
}

//// Purpose: Function to split the full line of input sent to the shell
//// Returns: An array of char arrays - the tokens
char **hesh_split_line(char *line)
{
  // Defines the variating buffersize, position variable for token array, a token (char array), and allocates an array of tokens (an array of arrays of characters)
  int token_buffsize = TOKEN_BUFFER_SIZE;
  int position = 0;
  char **tokens = malloc(token_buffsize * sizeof(char*));
  char *token;

  // If tokens isn't allocated/malloc fails
  if (!tokens) 
  {
    fprintf(stderr, "Error: malloc in hesh_split_line\n");
    exit(EXIT_FAILURE);
  }

  // Creates an array of tokens by adding a null terminator after each whitespace deliminated token
  // Returns the pointer to the first one in list.
  token = strtok(line, HESH_TOK_DELIM);

  // While the token char array isn't empty, store tokens into tokens array
  while (token != NULL) 
  {
    // Stores a token into the tokens array 
    tokens[position] = token;
    position++;

    // If our position has become bigger than what we allocated then expand it and reallocate
    if (position >= token_buffsize) 
    {
      token_buffsize += TOKEN_BUFFER_SIZE;
      tokens = realloc(tokens, token_buffsize * sizeof(char*));
      
      // If tokens isn't allocated/malloc fails
      if (!tokens) 
      {
        fprintf(stderr, "Error: realloc in hesh_split_line\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, HESH_TOK_DELIM);
  }

  // Sets the last position to be NULL
  tokens[position] = NULL;
  return tokens;
}

//// Purpose: Purpose is to launch a child process that will excecute a command
//// Returns: Returns an int based on successful launch - 1 if it works. Will error out if something else happens
int hesh_launch(char **args)
{
  // Defines variables for pid and returned status
  pid_t pid, wpid;
  int status;

  // Calls fork system call
  pid = fork();
  if (pid == 0) 
  {
    printf("In child process - executing commands\n");
    printf("\n");
    // Child process
    if (execvp(args[0], args) == -1)
      perror("hesh");
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0)
  {
    // Error forking
    perror("hesh"); 
  }
  else 
  {
    // Parent process
    do 
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

//// Purpose: Used to call built-in functions if applicable or calls the launch function
//// Returns: An integer result on either the built-in function call or launch function, 1 the shell is still active, 0 and the shell will quit
int hesh_execute(char **args)
{
  // Iteration variables
  int j = 0;
  int c = 0;

  if (args[0] == NULL)  
  {
    // An empty command was entered.
    printf("An empty command was entered.\n");
    return 1;
  }
  if(*(args[0]) == ';' || *(args[0]) == '&')
  {
    // The & or ; command was sent first.
    printf("Invalid use of \"&\" or \";\" characters\n");
    printf("Please use the characters correctly - after a command is given\n");
    return 1;
  }

  //// SECTION UNFINISHED
  // Attempting to create lists for commands that come before the & and ; deliminators and process them that way.
  // Might try a better way later if this doesn't work well.
  /*
  char** and_command_list;
  char** semicol_command_list;
  int and_iter = 0;
  int semicol_iter = 0;
  do
  {
    if(*(args[j]) == '&')
    {
      char* temp_command;   
      strcpy(temp_command, *(args[j-1]));
      and_command_list[and_iter] = temp_command;
      and_iter++;
    }
    else if(*(args[j]) == ';')
    {
      char* temp_command;   
      strcpy(temp_command, *(args[j-1]));
      and_command_list[semicol_iter] = temp_command;
      semicol_iter++;
    }
    c = 0;
    while(*(args[j] + c) != '\0')
    {
        printf("%c", *(args[j] + c));
        curr_command[c] = *(args[j] + c);
        c++;
    }
    printf("\n");
    j++;
  } 
  while(args[j] != NULL);
  */

  for (int i = 0; i < hesh_num_builtins(); i++) 
  {
    if (strcmp(args[0], builtin_str[i]) == 0) 
    {
      printf("Built in function detected. Executing the %s function\n", builtin_str[i]);
      printf("\n");
      return (*builtin_func[i])(args);
    }
  }

  printf("Entering hesh_launch\n");
  return hesh_launch(args);
}

//// Purpose: Function for main shell loop
//// Returns: Void
void hesh_loop()
{
    // Define variables for read line, array of arguments/commands, and the status excecuted command
    char *line;
    char **args;
    int status;
    int c = 0;
    int s = 0;

    // Start of loop
    do {
        printf("(¬_¬) - ");
        line = hesh_read_line();
        printf("Returned from hesh_read_line:\n");
        print_char_array(line);
        args = hesh_split_line(line);
        printf("Returned from hesh_split_line:\n");
        s = 0;
        do
        {
          c = 0;
          while(*(args[s] + c) != '\0')
          {
              printf("%c", *(args[s] + c));
              c++;
          }
          printf("\n");
          s++;
        } while(args[s] != NULL);
        printf("\n");
        printf("Entering hesh_execute\n");
        status = hesh_execute(args);
        free(line);
        free(args);
    } while (status);
};

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run shell loop.
  hesh_loop();

  // Perform any shutdown/cleanup.

  return 0;
}