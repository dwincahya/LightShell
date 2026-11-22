#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHELL_RL_BUFSIZE 1024

int main(int argc, char **argv) {
    shell_loop();
    return EXIT_SUCCESS;
}

void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = shell_read_lines();
        args = shell_split_args(line);
        status = shell_execute(args);
        
        free(line);
        free(args);
    } while (status);
    
}


char *shell_read_lines(void)
{
  int bufsize = SHELL_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= bufsize) {
      bufsize += SHELL_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}