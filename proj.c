#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h> //only preinstalled on unix environments

#define INPUT_BUFFER_SIZE 100
#define TOKEN_BUFFER_SIZE 64
#define HESH_TOK_DELIM " \t\r\n\a" // \t is tab, \r is carriage return, \n is line feed, \a is alert. (I thinks)
#define MAX_MATCHES 1

//// Purpose: Function to split the full line of input sent to the shell
//// Returns: An array of char arrays
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

//// Purpose: Function to read the full line of input from stdin
//// Returns: Char array
char *hesh_read_line(void)
{
  // Defines the line and buffer size variable
  char *line = NULL;
  size_t buffsize = 0; // getline will allocate a buffer for us (for now)
  
  // Defining regular expression and the regex_t object
  char* nono_chars = "[^a-zA-Z0-9._/-]+";
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
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  printf("About to compile RegEx...\n");
  // If the regular expression compilation fails, print error and exit
  if(regcomp(&re, nono_chars, REG_EXTENDED) != 0)
  {
    fprintf(stderr, "Error: regcomp in hesh_read_line");
    exit(EXIT_FAILURE);
  }
  else
    printf("RegEx Compilation Successful!\n");

  // Execute finding regular expressions in the inputted line, store
  if(regexec(&re, line, MAX_MATCHES, matches, 0) == 0)
  {
    printf("Match 0 so - %d\n", matches[0].rm_so);
    printf("Match 0 eo - %d\n", matches[0].rm_eo);
    printf("With whole expression, A matched substring \"%.*s\" is found at position %d to %d.\n", matches[0].rm_eo - matches[0].rm_so, &line[matches[0].rm_so], matches[0].rm_so, matches[0].rm_eo - 1);
    // Regex Sub Expressions
    //printf("Match 1 so - %d\n", matches[1].rm_so);
    //printf("Match 1 eo - %d\n", matches[1].rm_eo);
    //printf("With sub expression, A matched substring \"%s\" is found at position %d to %d.\n", &line[matches[1].rm_so], matches[1].rm_so, matches[1].rm_eo - 1);
  }
  else
    printf("No matches were found.\n");

  regfree(&re);

  return line;
}

//// Purpose: Function for main loop
//// Returns: Void
void hesh_loop()
{
    // Define variables for read line, array of arguments/commands, and the status excecuted command
    char *line;
    char **args;
    int status;

    // Start of loop
    do {
        printf("$ ");
        line = hesh_read_line();
        //args = hesh_split_line(line);
        //status = hesh_execute(args);
        wait();
        //free(line);
        //free(args);
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