#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "TARVB.h"
#include "TTAB.h"
#include "shell.h"
#include "commands.h"

void sh_loop(TTAB *tab, TARVB **a, int *t) {
    char *line;
    char **args;
    int status;

    do {
        line = readline("> ");
        args = sh_parse_line(line);
        status = sh_execute(args, tab, a, t);

        free(line);
        free(args);
    }while(status);
}

char *readline(char *prompt) {
    printf("%s", prompt);
    int buff_size = 1024, pos = 0;
    char *buffer = (char *) malloc(buff_size * sizeof(char));

    if(!buffer) {
        fprintf(stderr, "allocation error\n");
        exit(1);
    }

    while(1) {
        int c = getchar();
        if(c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        }

        buffer[pos++] = c;

        // 2.0
        // erro no scope da realloc
        if(pos >= buff_size) {
            buff_size *= 2;
            buffer = (char *) realloc(buffer, buff_size);
            if(!buffer) {
                fprintf(stderr, "allocation error\n");
                exit(1);
            }
        }
    }
}

char **sh_parse_line(char *line) {
    int buff_size = 64, pos = 0;
    char **tokens = (char **) malloc(buff_size * sizeof(char *));
    char *token;

    if(!tokens) {
        fprintf(stderr, "allocation error\n");
        exit(1);
    }

    token = strtok(line, " \t\r\n\a");
    while(token) {
        tokens[pos++] = token;

        if(pos >= buff_size) {
            buff_size += 64;
            tokens = (char **) realloc(tokens, buff_size * sizeof(char *));
            if(!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(1);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[pos] = NULL;
    return tokens;
}

int sh_execute(char **args, TTAB *tab, TARVB **a, int *t) {
    while(!args[0])
        return 1;

    for(int i = 0; i < sh_num_func(); i++)
        if(!strcmp(args[0], func_str[i]))
            return (*func[i])(args, tab, a, t);


    printf("Comando nao definido: \"%s\". Tente \"help\".\n", args[0]);
    return 1;
}
