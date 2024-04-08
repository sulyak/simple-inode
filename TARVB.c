#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "TARVB.h"

TARVB *TARVB_cria(int t) {
    TARVB *novo = (TARVB *)malloc(sizeof(TARVB));
    novo->nchaves = 0;
    novo->folha = 1;

    novo->filho = (TARVB **) malloc(sizeof(TARVB *) * 2*t);
    for(int i = 0; i < 2*t; i++)
        novo->filho[i] = NULL;

    novo->chave = (TARQ **) malloc(sizeof(TARQ *) * (2*t - 1));
    for(int i = 0; i < 2*t - 1; i++)
        novo->chave[i] = NULL;
    return novo;
}

TARVB *TARVB_libera(TARVB *a) {
    if(!a)
        return a;
    if(!a->folha)
        for(int i = 0; i <= a->nchaves; i++)
            TARVB_libera(a->filho[i]);
    for(int i = 0; i < a->nchaves; i++) {
        free(a->chave[i]);
        a->chave[i] = NULL;
    }
    free(a->chave);
    free(a->filho);
    free(a);
    return NULL;
}

TARQ *TARVB_busca(TARVB *a, int id) {
    if(!a)
        return NULL;
    int i = 0;
    while(i < a->nchaves && id > a->chave[i]->id)
        i++;
    if(i < a->nchaves && id == a->chave[i]->id)
        return a->chave[i];
    return TARVB_busca(a->filho[i], id);
}

TARVB *TARVB_inicializa() {
    return NULL;
}

TARVB *divisao(TARVB *x, int i, TARVB *y, int t) {
    TARVB *z = TARVB_cria(t);
    z->nchaves = t - 1;
    z->folha = y->folha;

    for(int j = 0; j < t - 1; j++) {
        z->chave[j] = y->chave[j + t];
        y->chave[j + t] = NULL;
    }
    if(!y->folha)
        for(int j = 0; j < t; j++) {
            z->filho[j] = y->filho[j + t];
            y->filho[j + t] = NULL;
        }

    y->nchaves = t - 1;

    for(int j = x->nchaves; j >= i; j--)
        x->filho[j + 1] = x->filho[j];
    x->filho[i] = z;

    for(int j = x->nchaves; j >= i; j--) {
        x->chave[j] = x->chave[j - 1];
        x->chave[j - 1] = NULL;
    }
    x->chave[i - 1] = y->chave[t - 1];
    y->chave[t - 1] = NULL;

    x->nchaves++;
    return x;
}


TARVB *insere_nao_completo(TARVB *x, TARQ *k, int t) {
    int i = x->nchaves - 1;
    if(x->folha) {
        while(i >= 0 && k->id < x->chave[i]->id) {
            x->chave[i + 1] = x->chave[i];
            x->chave[i] = NULL;
            i--;
        }
        x->chave[i + 1] = k;
        x->nchaves++;
        return x;
    }
    while(i >= 0 && k->id < x->chave[i]->id)
        i--;
    i++;
    if(x->filho[i]->nchaves == 2*t - 1) {
        x = divisao(x, i + 1, x->filho[i], t);
        if(k->id > x->chave[i]->id)
            i++;
    }
    x->filho[i] = insere_nao_completo(x->filho[i], k, t);
    return x;
}

TARVB *TARVB_insere(TARVB *a, TARQ *k, int t) {
    if(TARVB_busca(a, k->id)) return a;

    if(!a) {
        a = TARVB_cria(t);
        a->chave[0] = k;
        a->nchaves = 1;
        return a;
    }

    if(a->nchaves == 2*t - 1) {
        TARVB *b = TARVB_cria(t);
        b->nchaves = 0;
        b->folha = 0;
        b->filho[0] = a;
        b = divisao(b, 1, a, t);
        b = insere_nao_completo(b, k, t);
        return b;
    }

    a = insere_nao_completo(a, k, t);
    return a;
}

TARVB* remover(TARVB* arv, int ch, int t, int libera){
    if(!arv)
        return arv;
    int i;

    for(i = 0; i < arv->nchaves && arv->chave[i]->id < ch; i++);

    // CASOS 1, 2A, 2B e 2C
    if(i < arv->nchaves && ch == arv->chave[i]->id){

        // CASO 1
        if(arv->folha){
            int j;

            TARQ * tmp = arv->chave[i];
            for(j = i; j < arv->nchaves-1; j++)
                arv->chave[j] = arv->chave[j+1];
            arv->chave[j] = NULL;

            if(libera)
                free(tmp);

            arv->nchaves--;
            if(!arv->nchaves){ //ultima revisao: 04/2020
                TARVB_libera(arv);
                arv = NULL;
            }
            return arv;
        }

        // CASO 2A
        if(!arv->folha && arv->filho[i]->nchaves >= t){
            TARVB *y = arv->filho[i];  //Encontrar o predecessor k' de k na árvore com raiz em y while(!y->folha) y = y->filho[y->nchaves];
            TARQ *temp = y->chave[y->nchaves-1];
            arv->filho[i] = remover(arv->filho[i], temp->id, t, libera);
            //Eliminar recursivamente K e substitua K por K' em x
            arv->chave[i] = temp;
            return arv;
        }

        // CASO 2B
        if(!arv->folha && arv->filho[i+1]->nchaves >= t){
            TARVB *y = arv->filho[i+1];  //Encontrar o sucessor k' de k na árvore com raiz em y
            while(!y->folha)
                y = y->filho[0];
            TARQ *temp = y->chave[0];
            y = remover(arv->filho[i+1], temp->id, t, 0); //Eliminar recursivamente K e substitua K por K' em x
            arv->chave[i] = temp;
            return arv;
        }

        // CASO 2c
        if(!arv->folha && arv->filho[i+1]->nchaves == t-1 && \
                arv->filho[i]->nchaves == t-1){
            TARVB *y = arv->filho[i];
            TARVB *z = arv->filho[i+1];

            y->chave[y->nchaves] = arv->chave[i];          //colocar ch ao final de filho[i]
            int j;
            for(j = 0; j < t - 1; j++) { //juntar chave[i+1] com chave[i]
                y->chave[t+j] = z->chave[j];
                z->chave[j] = NULL;
            }
            for(j = 0; j <= t; j++){                 //juntar filho[i+1] com filho[i]
                y->filho[t+j] = z->filho[j];
                z->filho[j] = NULL; //ultima revisao: 04/2020
            }
            y->nchaves = 2*t-1;

            for(j=i; j < arv->nchaves-1; j++)   //remover ch de arv
                arv->chave[j] = arv->chave[j+1];
            arv->chave[j] = NULL;

            for(j=i+1; j < arv->nchaves; j++)
                arv->filho[j] = arv->filho[j+1]; //remover ponteiro para filho[i+1]
            arv->filho[j] = NULL;

            arv->nchaves--;
            if(!arv->nchaves){ //ultima revisao: 04/2020
                TARVB *temp = arv;
                arv = arv->filho[0];
                temp->filho[0] = NULL;
                TARVB_libera(temp);
                arv = remover(arv, ch, t, libera);
            }
            else {
                for(int x = 0; x < 2*t-1; x++) {
                    printf("%d ", arv->filho[i]->chave[x]->id);
                }
                arv->filho[i] = remover(arv->filho[i], ch, t, libera);
            }

            return arv;
        }
    }

    TARVB *y = arv->filho[i], *z = NULL;
    if(y->nchaves == t-1){ //CASOS 3A e 3B
        if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){ //CASO 3A
            z = arv->filho[i+1];
            y->chave[t-1] = arv->chave[i];   //dar a y a chave i da arv
            y->nchaves++;
            arv->chave[i] = z->chave[0];     //dar a arv uma chave de z
            int j;
            for(j=0; j < z->nchaves-1; j++)  //ajustar chaves de z
                z->chave[j] = z->chave[j+1];
            z->chave[j] = NULL;
            y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
            for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
                z->filho[j] = z->filho[j+1];
            z->nchaves--;
            arv->filho[i] = remover(arv->filho[i], ch, t, libera);
            return arv;
        }

        if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >=t)){ //CASO 3A
            z = arv->filho[i-1];
            int j;
            for(j = y->nchaves; j>0; j--)               //encaixar lugar da nova chave
                y->chave[j] = y->chave[j-1];
            y->chave[y->nchaves] = NULL;
            for(j = y->nchaves+1; j>0; j--)             //encaixar lugar dos filhos da nova chave
                y->filho[j] = y->filho[j-1];
            y->chave[0] = arv->chave[i-1];              //dar a y a chave i da arv
            arv->chave[i - 1] = NULL;
            y->nchaves++;
            arv->chave[i-1] = z->chave[z->nchaves-1];   //dar a arv uma chave de z
            z->chave[z->nchaves - 1] = NULL;
            y->filho[0] = z->filho[z->nchaves];         //enviar ponteiro de z para o novo elemento em y
            z->nchaves--;
            arv->filho[i] = remover(y, ch, t, libera);
            return arv;
        }

        //CASO 3B
        if(!z){
            if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){
                z = arv->filho[i+1];
                y->chave[t-1] = arv->chave[i];     //pegar chave [i] e coloca ao final de filho[i]
                arv->chave[i] = NULL;
                y->nchaves++;
                int j;
                for(j=0; j < t-1; j++){
                    y->chave[t+j] = z->chave[j];     //passar filho[i+1] para filho[i]
                    z->chave[j] = NULL;
                    y->nchaves++;
                }
                if(!y->folha){
                    for(j=0; j<t; j++){
                        y->filho[t+j] = z->filho[j];
                        z->filho[j] = NULL; //ultima revisao: 04/2020
                    }
                    TARVB_libera(z);
                }
            for(j=i; j < arv->nchaves-1; j++){ //limpar referências de i
                arv->chave[j] = arv->chave[j+1];
                arv->filho[j+1] = arv->filho[j+2];
            }
            arv->chave[j] = NULL;
            arv->filho[arv->nchaves] = NULL;
            arv->nchaves--;
            if(!arv->nchaves){ //ultima revisao: 04/2020
                TARVB *temp = arv;
                arv = arv->filho[0];
                temp->filho[0] = NULL;
                TARVB_libera(temp);
            }
            arv = remover(arv, ch, t, libera);
            return arv;
        }

            if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){
                z = arv->filho[i-1];
                if(i == arv->nchaves) {
                    z->chave[t-1] = arv->chave[i-1]; //pegar chave[i] e poe ao final de filho[i-1]
                    arv->chave[i - 1] = NULL;
                }
                else {
                    z->chave[t-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
                    arv->chave[i] = NULL;
                }
                z->nchaves++;
                int j;
                for(j=0; j < t-1; j++){
                    z->chave[t+j] = y->chave[j];     //passar filho[i+1] para filho[i]
                    y->chave[j] = NULL;
                    z->nchaves++;
                }
                if(!z->folha){
                    for(j=0; j<t; j++){
                    z->filho[t+j] = y->filho[j];
                    y->filho[j] = NULL; //ultima revisao: 04/2020
                }
                TARVB_libera(y);
                }
                arv->filho[arv->nchaves] = NULL;
                arv->nchaves--;
                if(!arv->nchaves){ //ultima revisao: 04/2020
                    TARVB *temp = arv;
                    arv = arv->filho[0];
                    temp->filho[0] = NULL;
                    TARVB_libera(temp);
                }
                else
                    arv->filho[i-1] = z;
                arv = remover(arv, ch, t, libera);
                return arv;
            }
        }
    }
    arv->filho[i] = remover(arv->filho[i], ch, t, libera);
    return arv;
}



TARVB *TARVB_retira(TARVB *a, int k, int t) {
    if(!a || !TARVB_busca(a, k))
        return a;
    TARVB *result = remover(a, k, t, 1);
    return result;
}

void TARVB_imprime_por_id(TARVB *a, int id) {
    TARQ *result = TARVB_busca(a, id);

    while(result) {
        printf("%s", result->texto);
        result = TARVB_busca(a, result->prox_id);
    }
}

void imp_rec(TARVB *a, int andar){
    if(!a) return;
    int i;
    for(i = 0; i < a->nchaves; i++){
        imp_rec(a->filho[i], andar+1);
        for(int j = 0; j <= andar; j++)
            printf("\t");
        printf("%d\n", a->chave[i]->id);
    }
    imp_rec(a->filho[i], andar+1);
}

void TARVB_imprime(TARVB *a){
    imp_rec(a, 0);
}

char *TARVB_to_cstr(TARVB *a, int id) {
    char *c_str = NULL;
    int size = 0;

    TARQ *result = TARVB_busca(a, id);
    while(result) {
        size += strlen(result->texto);
        result = TARVB_busca(a, result->prox_id);
    }

    c_str = (char *) malloc(sizeof(char) * (size + 1));
    c_str[0] = '\0';

    result = TARVB_busca(a, id);
    while(result) {
        strncat(c_str, result->texto, strlen(result->texto));
        result = TARVB_busca(a, result->prox_id);
    }

    c_str[size] = '\0';
    return c_str;
}

TARQ *TARVB_find_in_chain(TARVB *a, int prim_id, char *target, int *num_blocks, int *pos) {
    int id = prim_id;
    int count = 0;
    int j = 0;
    int len = strlen(target);

    *num_blocks = 0;
    int prim_pos = 0;

    // passando pelo arquivo inteiro
    TARQ *result = TARVB_busca(a, id);
    TARQ *pai = NULL;
    while(result) {
        // comparando cada letra do target com a letra atual
        if(target[count] == result->texto[j]) {

            // no primeiro match, salvo as coordenadas da letra
            if(!count) {
                *num_blocks = 1;
                pai = result;
                prim_pos = j;
            }
            count++;
        }
        else {
            if(count)  {
                result = pai;
                j = prim_pos;
            }
            count = 0;
        }

        // se achei todas as letras retorna o bloco onde a string comeca
        if(count == len) {
            if(pos)
                *pos = prim_pos;
            return pai;
        }

        // acabou o index precisa pular pro prox bloco
        j++;
        if(j >= strlen(result->texto)) {
            j = 0;
            result = TARVB_busca(a, result->prox_id);
            (*num_blocks)++;
        }
    }

    *num_blocks = 0;
    return NULL;
}

void TARQ_imprime(TARQ *arq) {
    if(!arq)
        return;
    printf("texto:   \"%s\"\n", arq->texto);
    printf("id:      %d\n", arq->id);
    printf("pai:     %d\n", arq->pai);
    printf("prox_id: %d\n", arq->prox_id);
    printf("\n");
}

static int nn(TARVB *a) {
    if(!a)
        return 0;
    int count = a->nchaves;
    for(int i = 0; i <= a->nchaves; i++)
        count += nn(a->filho[i]);
    return count;
}

static void tovet(TARVB *a, TARQ **v, int *index) {
    if(!a)
        return;
    int i;
    for(i = 0; i < a->nchaves; i++) {
        tovet(a->filho[i], v, index);
        v[*index] = a->chave[i];
        (*index)++;
    }
    tovet(a->filho[i], v, index);
}

static void nullfy(TARVB *a) {
    if(!a)
        return;
    int i;
    for(i = 0; i < a->nchaves; i++) {
        a->chave[i] = NULL;
        nullfy(a->filho[i]);
    }
    nullfy(a->filho[i]);
}

TARVB *TARVB_reconstroi(TARVB *a, int novo_t) {
    int size = nn(a);
    TARQ **v = (TARQ **) malloc(sizeof(TARQ *) * size);
    int i = 0;
    tovet(a, v, &i);
    TARVB *novo = TARVB_inicializa();
    for(int j = 0; j < size; j++)
        novo = TARVB_insere(novo, v[j], novo_t);
    nullfy(a);
    TARVB_libera(a);
    free(v);
    return novo;
}
