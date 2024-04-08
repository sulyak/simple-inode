#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "TARVB.h"
#include "TIN.h"
#include "TTAB.h"

void TTAB_inicializa(TTAB *t) {
    t->info = (TIN *) malloc(sizeof(TIN) * 16);
    t->tam_max = 16;
    t->tam = 0;
}

int TTAB_prox_id(TTAB *t) {
    static int id = 1;
    return id++;
}

void TTAB_insere(TTAB *t, char *nome_arq, int id) {
    // se a tablea tiver cheia, realloca com o dobro de tamanho
    if(t->tam == t->tam_max) {
        t->tam_max *= 2;
        t->tam_max++;
        t->info = (TIN *) realloc(t->info, t->tam_max);
    }
    strcpy(t->info[t->tam].nome_arq, nome_arq);
    t->info[t->tam].id = id;
    t->tam++;
}

void TTAB_libera(TTAB *tab) {
    free(tab->info);
    tab->info = NULL;
    tab->tam_max = 0;
    tab->tam = 0;
}

static int _fnget(FILE *input, char *buffer, int buff_size) {
    int i = 0;
    char c;
    while(i < buff_size - 1 && fscanf(input, "%c", &c) == 1)
        buffer[i++] = c;

    buffer[i] = '\0';
    return i;
}

TARVB* TTAB_insere_arquivo(TTAB *tab, TARVB *a, char *nome_arq, int t){
    int novo_id = TTAB_prox_id(tab);
    TTAB_insere(tab, nome_arq, novo_id);

    FILE *f = fopen(nome_arq, "r");
    if(!f) {
        fprintf(stderr, "erro abrindo arquivo %s.\n", nome_arq);
        exit(1);
    }

    char buffer[MAXTEXTO];
    TARQ *ant = NULL;

    int count = _fnget(f, buffer, MAXTEXTO);
    while(count) {
        // cria o novo bloco de arquivo
        TARQ *novo = (TARQ *) malloc(sizeof(TARQ));
        strcpy(novo->texto, buffer);
        novo->id = novo_id;
        novo->pai = ant ? ant->id : -1;
        novo->prox_id = -1;

        // insiro meu novo bloco na arvore
        a = TARVB_insere(a, novo, t);

        // conserta anterior->prox_id
        if(ant)
            ant->prox_id = novo_id;
        ant = novo;


        // pega o proximo id disponivel
        novo_id = TTAB_prox_id(tab);

        // le do arquivo novamente
        count = _fnget(f, buffer, MAXTEXTO);
    }

    fclose(f);
    return a;
}

TARVB *TTAB_append(TTAB *tab, TARVB *a, char *nome_arq, char *texto, int t) {
    int id = TTAB_busca_arquivo(tab, nome_arq);
    if(!id)
        return a;

    TARQ *result = TARVB_busca(a, id);
    while(result && result->prox_id != -1)
        result = TARVB_busca(a, result->prox_id);
    int prox_id = TTAB_prox_id(tab);

    result->prox_id = prox_id;

    TARQ *novo = (TARQ *) malloc(sizeof(TARQ));
    strcpy(novo->texto, texto);
    novo->id = prox_id;
    novo->pai = result->id;
    novo->prox_id = -1;

    a = TARVB_insere(a, novo, t);
    return a;
}

int TTAB_busca_arquivo(TTAB *tab, char *nome_arq) {
    for(int i = 0; i < tab->tam; i++)
        if(!strcmp(nome_arq, tab->info[i].nome_arq))
            return tab->info[i].id;
    return 0;
}

void TTAB_retira(TTAB *tab, char *nome_arq) {
    int indice = -1;
    for(int i = 0; i < tab->tam; i++)
        if(!strcmp(tab->info[i].nome_arq, nome_arq))
            indice = i;
    if(indice == -1)
        return;
    for(int i = indice + 1; i < tab->tam; i++)
        tab->info[i - 1] = tab->info[i];
    tab->tam--;
}
