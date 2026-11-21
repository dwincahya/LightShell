#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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