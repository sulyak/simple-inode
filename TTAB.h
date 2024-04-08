#pragma once

#include "TIN.h"
#include "TARVB.h"

typedef struct tabela {
    TIN *info;
    int tam;
    int tam_max;
} TTAB;

void TTAB_inicializa(TTAB *tab);
int TTAB_prox_id(TTAB *tab);
void TTAB_insere(TTAB *tab, char *nome_arq, int id);
void TTAB_libera(TTAB *tab);
TARVB *TTAB_insere_arquivo(TTAB *tab, TARVB *a, char *nome_arq, int t);
TARVB *TTAB_append(TTAB *tab, TARVB *a, char *nome_arq, char *texto, int t);
int TTAB_busca_arquivo(TTAB *tab, char *nome_arq);
void TTAB_retira(TTAB *tab, char *nome_arq);
