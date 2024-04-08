#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "TTAB.h"
#include "TARVB.h"
#include "commands.h"
#include "shell.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

char *func_str[] = {
    "help", "h",
    "quit", "q", "exit",
    "print", "p", "inspect", "show",
    "file", "f",
    "list", "ls",
    "del", "d", "delete", "remove", "rmv", "rm",
    "append",
    "search", "s",
    "add",
    "cut", "c",
    "fic",
    "sett"
};

int (*func[]) (char **, TTAB *, TARVB **, int *) = {
    help, help,
    quit, quit, quit,
    print, print, print, print,
    file, file,
    list, list,
    del, del, del, del, del, del,
    append,
    search, search,
    add,
    cut, cut,
    fic,
    sett
};

char *help_str[] = {
    "help, h\n",
    "\tTe ajuda.\n",
    "\tuso: help\n\n"

    "file, f\n",
    "\tCarrega um arquivo.\n",
    "\tuso: file nome_arquivo\n\n",

    "quit, exit, q\n",
    "\tSai do programa\n",
    "\tuso: quit\n\n",

    "print, show, inspect, p\n",
    "\tMostra um arquivo, a arvore ou um bloco de arquivo.\n",
    "\tuso: print [id | nome_arquivo]\n\n",

    "list, ls\n",
    "\tMostra a tabela de arquivos carregados.\n",
    "\tuso: list\n\n",

    "delete, del, remove, rmv, rm, d\n",
    "\tDeleta arquivos.\n",
    "\tuso: delete nome_arquivo1 [nome_arquivo2 ...]\n\n",

    "search, s\n",
    "\tProcura por uma string nos arquivos carregados.\n",
    "\tuso: search texto\n\n"

    "add\n",
    "\tAdiciona e cria um novo arquivo.\n",
    "\tuso: add nome_arquivo\n\n",

    "append\n",
    "\tAdiciona informacoes no final de um arquivo.\n",
    "\tuso: append nome_arquivo texto\n\n",

    "cut, c\n",
    "\tCorta uma parte de um arquivo.\n",
    "\tuso: cut nome_arquivo texto\n\n",

    "fic\n",
    "\t\"find in chain\": procura uma string dentro de uma cadeia de blocos de arquivo\n",
    "\tuso: fic id texto\n\n",

    "sett\n",
    "\tMuda o t da arvore.\n",
    "\tuso: sett novo_t\n\n",
    NULL
};

// ---------------------------

int sh_num_func(void) {
    return sizeof(func_str) / sizeof(char *);
}

int help(char **args, TTAB *tab, TARVB **a, int *t) {
    for(int i = 0; help_str[i]; i++)
        printf("%s", help_str[i]);
    return 1;
}

int quit(char **args, TTAB *tab, TARVB **a, int *t) {
    return 0;
}

int print(char **args, TTAB *tab, TARVB **a, int *t) {
    // sem argumentos -> imprime a arvore
    if(!args[1]) {
        TARVB_imprime(*a);
        return 1;
    }

    // para todo os argumentos
    for(int i = 1; args[i]; i++) {

        // pesquisa por nome de arquivo na tabela
        if(!atoi(args[i])) {
            int id = TTAB_busca_arquivo(tab, args[i]);
            if(!id)
                printf("arquivo \"%s\" nao encontado na tabela.\n", args[i]);
            else {
                char *result = TARVB_to_cstr(*a, id);
                printf("%s\n", result);
                free(result);
            }
        }

        // pesquisa por id na arvore
        else {
            TARQ *result = TARVB_busca(*a, atoi(args[i]));
            if(!result)
                printf("no %s nao encontrado.\n", args[i]);
            else
                TARQ_imprime(result);
        }
    }
    return 1;
}

int file(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1]) {
        printf("uso: %s arquivo1 [arquivo2 ...]\n", args[0]);
        return 1;
    }

    for(int i = 1; args[i]; i++) {
        if(access(args[i], F_OK))
            printf("arquivo %s nao existe. \n", args[i]);
        else if(TTAB_busca_arquivo(tab, args[i]))
            printf("arquivo %s ja carregado. pulando...\n", args[i]);
        else {
            *a = TTAB_insere_arquivo(tab, *a, args[i], *t);
            printf("arquivo %s carregado.\n", args[i]);
        }
    }

    return 1;
}

int list(char **args, TTAB *tab, TARVB **a, int *t) {
    if(tab->tam)
        printf("id\tnome\n\n");
    else
        printf("empty\n");
    for(int i = 0; i < tab->tam; i++)
        printf("%3d %s\n", tab->info[i].id, tab->info[i].nome_arq);
    printf("\n");
    return 1;
}

int del(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1]) {
        printf("uso: %s arquivo1 [arquivo2 ...]\n", args[0]);
        return 1;
    }

    // passa por todos os argumentos
    for(int i = 1; args[i]; i++) {
        int id = TTAB_busca_arquivo(tab, args[i]);
        if(!id)
            printf("arquivo %s nao existe, tente \"list\".\n", args[i]);
        else {
            printf("deletando arquivo %s...\n", args[i]);

            // retira da arvore todos os blocos
            TARQ *result = TARVB_busca(*a, id);
            while(result) {
                int prox_id = result->prox_id;
                *a = TARVB_retira(*a, result->id, *t);
                result = TARVB_busca(*a, prox_id);
            }

            // retira da tabela
            TTAB_retira(tab, args[1]);

            // remove o arquivo fisico
            remove(args[1]);
        }
    }
    return 1;
}

int append(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1] || !args[2]) {
        printf("uso: %s nome_arquivo texto\n", args[0]);
        return 1;
    }

    char *nome_arq = args[1];

    // calculando o tamanho da string dos argumentos passados
    int size = 0;
    for(int i = 2; args[i]; i++) {
        if(i > 2)
            size++;
        size += strlen(args[i]);
    }

    char *s = (char *) malloc(sizeof(char) * (size + 1));
    s[0] = '\0';

    // concatenando as strings de argumentos em uma so
    strcat(s, args[2]);
    for(int i = 3; args[i]; i++) {
        if(i > 2)
            strcat(s, " ");
        strcat(s, args[i]);
    }

    // caminhando ate o ultimo no

    int id;
    TARQ *result;

    id = TTAB_busca_arquivo(tab, nome_arq);
    if(!id) {
        printf("arquivo %s nao encontrado na tebela.\n", args[1]);
        free(s);
        return 1;
    }

    // caminha ate o final do arquivo
    result = TARVB_busca(*a, id);
    while(result && result->prox_id != -1)
        result = TARVB_busca(*a, result->prox_id);

    // calcula quanto sobrou no ultimo no
    int sobra = (MAXTEXTO - 1) - strlen(result->texto);

    // colocar sobra caracteres dentro da bunda da chain
    strncat(result->texto, s, sobra);

    // copia varios chunks de tamanho=MAXTEXTO-1 para a string texto, dps poe texto na arvore
    char texto[MAXTEXTO];
    int len = strlen(s);
    for(int start = sobra; start < len; start += (MAXTEXTO - 1)) {
        int len_read = MIN(MAXTEXTO - 1, strlen(s + start));
        strncpy(texto, s + start, MAXTEXTO - 1);
        texto[len_read] = '\0';
        *a = TTAB_append(tab, *a, nome_arq, texto, *t);
    }

    FILE *f = fopen(nome_arq, "w");
    char *txt = TARVB_to_cstr(*a, id);
    fprintf(f, "%s", txt);

    free(txt);
    free(s);

    // 2.0
    // faltou fechar o arquivo
    fclose(f);
    return 1;
}

int search(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1]) {
        printf("uso: %s texto\n", args[0]);
        return 1;
    }

    char *target = args[1];

    // passar por todos os arquivos na tabela procurando pelo target
    for(int i = 0; i < tab->tam; i++) {
        int id = tab->info[i].id;

        int num_blocks;
        TARQ *prim = TARVB_find_in_chain(*a, id, target, &num_blocks, NULL);
        while(prim) {;
            printf("in file %s:\n", tab->info[i].nome_arq);
            for(int i = 0; i < num_blocks; i++) {
                TARQ_imprime(prim);
                prim = TARVB_busca(*a, prim->prox_id);
            }
            if(prim)
                prim = TARVB_find_in_chain(*a, prim->id, target, &num_blocks, NULL);
        }
    }

    return 1;
}

int add(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1]) {
        printf("uso: %s nome_arquivo\n", args[0]);
        return 1;
    }

    char *nome_arq = args[1];
    if(!access(nome_arq, F_OK)) {
        printf("arquivo %s ja existe, overwrite? (sim/nao)\n", nome_arq);
        char *resp = readline("");
        if(strcmp(resp, "sim") && strcmp(resp, "s")) {
            free(resp);
            return 1;
        }
        free(resp);

        // se o arquivo existe, esta carregado na arvore, deleta
        int id = TTAB_busca_arquivo(tab, nome_arq);
        if(id) {
            char *new_args[] = {"", nome_arq, NULL};
            del(new_args, tab, a, t);
        }
    }

    FILE *f = fopen(nome_arq, "w");
    if(!f) {
        printf("add: erro abrindo arquivo %s\n", nome_arq);
        return 1;
    }

    char *buffer = readline("conteudo do arquivo:\n");
    fprintf(f, "%s", buffer);
    free(buffer);
    fclose(f);

    *a = TTAB_insere_arquivo(tab, *a, nome_arq, *t);
    return 1;
}

int cut(char **args, TTAB *tab, TARVB **a, int *t) {
   if(!args[1] || !args[2]) {
        printf("uso: %s nome_arquivo string_a_ser_deletada\n", args[0]);
        return 1;
    }

    char *nome_arquivo = args[1];
    int id = TTAB_busca_arquivo(tab, nome_arquivo);
    if(!id) {
        printf("arquivo \"%s\" nao encontrado.\n", nome_arquivo);
        return 1;
    }
    char *rmv = args[2];

    int len = strlen(rmv);
    int num_blocks;
    int pos;
    TARQ *ini = TARVB_find_in_chain(*a, id, rmv, &num_blocks, &pos);
    if(!ini) {
        printf("string \"%s\" nao encontrada no arquivo \"%s\".\n", rmv, nome_arquivo);
        return 1;
    }

    // shift <<
    int i = pos;
    int j = pos + len;

    TARQ *fim = ini;
    while(j >= MAXTEXTO - 1) {
        fim = TARVB_busca(*a, fim->prox_id);
        j -= (MAXTEXTO - 1);
    }

    while(fim) {
        ini->texto[i] = fim->texto[j];
        i++; j++;

        if(i >= MAXTEXTO - 1) {
            i = 0;
            ini = TARVB_busca(*a, ini->prox_id);
        }

        if(j >= strlen(fim->texto)) {
            j = 0;
            fim = TARVB_busca(*a, fim->prox_id);
        }
    }
    int prox_id = ini->prox_id;
    ini->texto[i] = '\0';
    ini->prox_id = -1;

    // remove os nos contendo conteudo que n esta mais presente dps do corte
    ini = TARVB_busca(*a, prox_id);
    while(ini) {
        prox_id = ini->prox_id;
        *a = TARVB_retira(*a, ini->id, *t);
        ini = TARVB_busca(*a, prox_id);
    }

    // 2.0
    // faltou atualizar o conteudo no arquivo fisico
    FILE *f = fopen(nome_arquivo, "w");
    char *txt = TARVB_to_cstr(*a, id);
    fprintf(f, "%s", txt);
    free(txt);
    fclose(f);

    return 1;
}

int fic(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1] || !args[2]) {
        printf("uso: %s id target.\n", args[0]);
        return 1;
    }
    int id = atoi(args[1]);
    char *target = args[2];

    int num_blocks;
    int pos;
    TARQ *arq = TARVB_find_in_chain(*a, id, target, &num_blocks, &pos);

    if(arq)
        printf("id:%d nb:%d pos:%d\n", arq->id, num_blocks, pos);
    else
        printf("n achei\n");
    return 1;
}

int sett(char **args, TTAB *tab, TARVB **a, int *t) {
    if(!args[1]) {
        printf("uso: %s num.\n", args[0]);
        return 1;
    }

    int novo_t = atoi(args[1]);

    if(!novo_t || novo_t < 2) {
        printf("novo t invalido. tente novamente (t >= 2).\n");
        return 1;
    }

    printf("novo t: %d\n", novo_t);
    if(*t != novo_t) {
        *a = TARVB_reconstroi(*a, novo_t);
        *t = novo_t;
    }
    return 1;
}
