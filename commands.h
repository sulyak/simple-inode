#pragma once

#include "TTAB.h"
#include "TARVB.h"


extern char *func_str[];
extern int (*func[]) (char **, TTAB *, TARVB **, int *t);

int sh_num_func(void);
int help(char **args, TTAB *tab, TARVB **a, int *t);
int quit(char **args, TTAB *tab, TARVB **a, int *t);
int print(char **args, TTAB *tab, TARVB **a, int *t);
int file(char **args, TTAB *tab, TARVB **a, int *t);
int list(char **args, TTAB *tab, TARVB **a, int *t);
int del(char **args, TTAB *tab, TARVB **a, int *t);
int append(char **args, TTAB *tab, TARVB **a, int *t);
int search(char **args, TTAB *tab, TARVB **a, int *t);
int add(char **args, TTAB *tab, TARVB **a, int *t);
int cut(char **args, TTAB *tab, TARVB **a, int *t);
int fic(char **args, TTAB *tab, TARVB **a, int *t);
int sett(char **args, TTAB *tab, TARVB **a, int *t);

// add a partir de um bloco?
// TODO?
