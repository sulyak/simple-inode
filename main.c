#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TARVB.h"
#include "TTAB.h"
#include "shell.h"

int main(int argc, char *argv[]) {
    TTAB tabela;
    TTAB_inicializa(&tabela);
    TARVB *arvore = TARVB_inicializa();
    int t = 2;
    for(int i = 1; i < argc; i++)
        arvore = TTAB_insere_arquivo(&tabela, arvore, argv[i], t);

    printf("bem vindo. a principio t=2, para mudar use \"sett num\", onde num eh o novo valor.\n");
    printf("digite \"help\" para ser ajudado.\n");
    sh_loop(&tabela, &arvore, &t);

    TARVB_libera(arvore);
    TTAB_libera(&tabela);
    return 0;
}
