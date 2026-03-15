#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela_simbolos.h"

/* Variável global que aponta para o escopo mais interno atual */
Escopo *topo = NULL; 

/* Função hash simples para calcular a posição no array */
unsigned int calcular_hash(char *lexema) {
    unsigned int hash = 0;
    while (*lexema) {
        hash = (hash << 5) + *lexema++;
    }
    return hash % TAM_HASH;
}

/* a) iniciar a pilha de tabela de símbolos [cite: 143] */
void iniciar_pilha() {
    topo = NULL;
}

/* b) criar uma nova tabela de símbolos (novo escopo) e empilhá-la [cite: 144] */
void empilhar_escopo() {
    Escopo *novo = (Escopo*) malloc(sizeof(Escopo));
    for (int i = 0; i < TAM_HASH; i++) {
        novo->tabela_hash[i] = NULL;
    }
    novo->abaixo = topo;
    topo = novo;
}

/* c) pesquisar por um nome na pilha de tabelas de símbolos [cite: 145] */
Simbolo* pesquisar_simbolo(char *lexema) {
    Escopo *atual = topo;
    unsigned int indice = calcular_hash(lexema);

    /* Pesquisa inicia no topo e desce em direção à base  */
    while (atual != NULL) {
        Simbolo *simb = atual->tabela_hash[indice];
        while (simb != NULL) {
            if (strcmp(simb->lexema, lexema) == 0) {
                return simb; /* Retorna o ponteiro onde foi encontrado [cite: 147] */
            }
            simb = simb->prox;
        }
        atual = atual->abaixo; /* Desce um nível na pilha */
    }
    return NULL; /* Retorna ponteiro vazio se não encontrou [cite: 147] */
}

/* d) remover escopo atual [cite: 148] */
void remover_escopo() {
    if (topo == NULL) return;

    Escopo *removido = topo;
    topo = topo->abaixo;

    /* Elimina a tabela de símbolos que está no topo[cite: 148], liberando a memória */
    for (int i = 0; i < TAM_HASH; i++) {
        Simbolo *simb = removido->tabela_hash[i];
        while (simb != NULL) {
            Simbolo *temp = simb;
            simb = simb->prox;
            free(temp->lexema);
            free(temp->tipo);
            free(temp);
        }
    }
    free(removido);
}

/* Função para inserir uma nova variável apenas no escopo do topo */
Simbolo* inserir_simbolo(char *lexema, char *tipo, int linha) {
    if (topo == NULL) empilhar_escopo(); /* Segurança */

    unsigned int indice = calcular_hash(lexema);

    /* Verifica se a variável JÁ EXISTE no mesmo escopo (Erro semântico) */
    Simbolo *simb = topo->tabela_hash[indice];
    while (simb != NULL) {
        if (strcmp(simb->lexema, lexema) == 0) {
            return NULL; /* Variável já declarada neste bloco! */
        }
        simb = simb->prox;
    }

    /* Aloca e insere o novo símbolo na tabela hash */
    Simbolo *novo = (Simbolo*) malloc(sizeof(Simbolo));
    novo->lexema = strdup(lexema);
    novo->tipo = strdup(tipo);
    novo->linha_decl = linha;
    novo->prox = topo->tabela_hash[indice];
    topo->tabela_hash[indice] = novo;

    return novo;
}