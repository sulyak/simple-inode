#pragma once

#include "TARVB.h"
#include "TTAB.h"

void sh_loop(TTAB *tab, TARVB **a, int *t);
char *readline(char *prompt);
char **sh_parse_line(char *line);
int sh_execute(char **args, TTAB *tab, TARVB **a, int *t);
