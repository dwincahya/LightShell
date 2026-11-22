#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHELL_RL_BUFSIZE 1024
#define TOK_DELIM " \t\r\n\a"

void shell_loop(void);
char *shell_read_lines(void);
char **shell_split_args(char *line);
int shell_execute(char **args);
int shell_launch(char **args);
int shell_cd(char **args);

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    shell_loop();
    return EXIT_SUCCESS;
}

void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        fflush(stdout);

        line = shell_read_lines();
        if (line == NULL) {  /* Ctrl+D */
            break;
        }

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

        if (c == EOF && position == 0) {
            free(buffer);
            return NULL;
        }

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }

        buffer[position] = c;
        position++;

        if (position >= bufsize) {
            bufsize += SHELL_RL_BUFSIZE;
            char *tmp = realloc(buffer, bufsize);
            if (!tmp) {
                free(buffer);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
            buffer = tmp;
        }
    }
}

char **shell_split_args(char *line)
{
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += 64;
            char **tmp = realloc(tokens, bufsize * sizeof(char *));
            if (!tmp) {
                free(tokens);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
            tokens = tmp;
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int shell_execute(char **args)
{
    if (args[0] == NULL) {
        return 1;
    }

    if (strcmp(args[0], "exit") == 0) {
        return 0;
    }

    if (strcmp(args[0], "cd") == 0) {
        return shell_cd(args);
    }

    return shell_launch(args);
}

int shell_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else if (chdir(args[1]) != 0) {
        perror("lsh");
    }
    return 1;
}

int shell_launch(char **args)
{
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("lsh");
    } else {
        waitpid(pid, &status, 0);
    }
    return 1;
}