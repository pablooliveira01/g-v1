#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

NoAST* alocaNo(TipoNo tipo, int linha) {
    NoAST* no = (NoAST*) malloc(sizeof(NoAST));
    no->tipo = tipo;
    no->linha = linha;
    no->lexema = NULL;
    no->esq = no->dir = no->condicao = no->senao = no->prox = NULL;
    return no;
}

NoAST* criaNoFolha(TipoNo tipo, char* lexema, int linha) {
    NoAST* no = alocaNo(tipo, linha);
    if(lexema) no->lexema = strdup(lexema);
    return no;
}

NoAST* criaNo1(TipoNo tipo, NoAST* esq, int linha) {
    NoAST* no = alocaNo(tipo, linha);
    no->esq = esq;
    return no;
}

NoAST* criaNo2(TipoNo tipo, char* lexema, NoAST* esq, NoAST* dir, int linha) {
    NoAST* no = alocaNo(tipo, linha);
    if(lexema) no->lexema = strdup(lexema);
    no->esq = esq;
    no->dir = dir;
    return no;
}

NoAST* criaNoSe(NoAST* condicao, NoAST* cmd_entao, NoAST* cmd_senao, int linha) {
    NoAST* no = alocaNo(NODE_SE, linha);
    no->condicao = condicao;
    no->esq = cmd_entao;
    no->senao = cmd_senao;
    return no;
}

NoAST* criaNoEnquanto(NoAST* condicao, NoAST* comando, int linha) {
    NoAST* no = alocaNo(NODE_ENQUANTO, linha);
    no->condicao = condicao;
    no->esq = comando;
    return no;
}

NoAST* concatenaLista(NoAST* lista, NoAST* novo_no) {
    if (!lista) return novo_no;
    NoAST* atual = lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = novo_no;
    return lista;
}

void imprimeAST(NoAST* raiz, int nivel) {
    if (!raiz) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    
    switch(raiz->tipo) {
        case NODE_PROGRAMA: printf("[Programa]\n"); break;
        case NODE_BLOCO: printf("[Bloco]\n"); break;
        case NODE_DECL_VAR: printf("[DeclVar: %s]\n", raiz->lexema); break;
        case NODE_ATRIB: printf("[Atribuicao: =]\n"); break;
        case NODE_LEIA: printf("[Leia: %s]\n", raiz->lexema); break;
        case NODE_ESCREVA: printf("[Escreva]\n"); break;
        case NODE_NOVALINHA: printf("[NovaLinha]\n"); break;
        case NODE_SE: printf("[Se]\n"); break;
        case NODE_ENQUANTO: printf("[Enquanto]\n"); break;
        case NODE_OP_BINARIA: printf("[OpBinaria: %s]\n", raiz->lexema); break;
        case NODE_OP_UNARIA: printf("[OpUnaria: %s]\n", raiz->lexema); break;
        case NODE_ID: printf("[ID: %s]\n", raiz->lexema); break;
        case NODE_INT: printf("[INT: %s]\n", raiz->lexema); break;
        case NODE_CAR: printf("[CAR: %s]\n", raiz->lexema); break;
        case NODE_STR: printf("[STR: %s]\n", raiz->lexema); break;
        case NODE_TIPO: printf("[Tipo: %s]\n", raiz->lexema); break;
    }

    imprimeAST(raiz->condicao, nivel + 1);
    imprimeAST(raiz->esq, nivel + 1);
    imprimeAST(raiz->dir, nivel + 1);
    if (raiz->senao) {
        for (int i = 0; i < nivel; i++) printf("  ");
        printf("[Senao]\n");
        imprimeAST(raiz->senao, nivel + 1);
    }
    imprimeAST(raiz->prox, nivel); /* Imprime o próximo no mesmo nível */
}