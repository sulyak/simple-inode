#pragma once
#define MAXTEXTO 11

typedef struct arquivo {
    char texto[MAXTEXTO];
    int id;
    int pai;
    int prox_id;
} TARQ;

typedef struct arvb {
    int folha;
    struct arvb **filho;

    int nchaves;
    TARQ **chave;
} TARVB;

TARVB *TARVB_inicializa();
TARVB *TARVB_cria(int t);
TARVB *TARVB_libera(TARVB *a);
TARQ *TARVB_busca(TARVB* x, int ch);
TARQ *TARVB_find_in_chain(TARVB *a, int prim_id, char *target, int *num_blocks, int *pos);
TARVB *TARVB_insere(TARVB *T, TARQ *k, int t);
TARVB *TARVB_retira(TARVB* arv, int k, int t);
TARVB *TARVB_reconstroi(TARVB *a, int novo_t);
void TARVB_imprime_por_id(TARVB *a, int id);
void TARVB_imprime(TARVB *a);
char *TARVB_to_cstr(TARVB *a, int id);
void TARQ_imprime(TARQ *arq);
