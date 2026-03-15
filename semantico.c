#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantico.h"
#include "tabela_simbolos.h"

/* Função recursiva que retorna o tipo de uma expressão ("int" ou "car") ou NULL se for comando */
char* percorrer_e_checar(NoAST *no) {
    if (!no) return NULL;

    char *tipo_esq = NULL;
    char *tipo_dir = NULL;
    Simbolo *simb = NULL;

    switch (no->tipo) {
        case NODE_PROGRAMA:
            percorrer_e_checar(no->esq);
            break;

        case NODE_BLOCO:
            empilhar_escopo(); /* Novo escopo ao entrar no bloco */
            percorrer_e_checar(no->esq); /* VarSection */
            percorrer_e_checar(no->dir); /* ListaComando */
            remover_escopo();  /* Remove escopo ao sair */
            break;

        case NODE_DECL_VAR:
            /* Tenta inserir. Se retornar NULL, a variável já existe neste escopo */
            tipo_dir = percorrer_e_checar(no->esq); /* Pega o tipo (ex: "int") */
            if (inserir_simbolo(no->lexema, tipo_dir, no->linha) == NULL) {
                printf("ERRO SEMANTICO: Variavel '%s' ja declarada neste escopo - linha: %d\n", no->lexema, no->linha);
                exit(1);
            }
            break;

        case NODE_TIPO:
            return no->lexema; /* Retorna "int" ou "car" */

        case NODE_ID:
            /* Verifica se a variável foi declarada antes do uso */
            simb = pesquisar_simbolo(no->lexema);
            if (!simb) {
                printf("ERRO SEMANTICO: Variavel '%s' nao declarada - linha: %d\n", no->lexema, no->linha);
                exit(1);
            }
            return simb->tipo; /* Retorna o tipo da variável salva na tabela */

        case NODE_INT:
            return "int";

        case NODE_CAR:
            return "car";

        case NODE_ATRIB:
            tipo_esq = percorrer_e_checar(no->esq); /* Tipo da variável destino */
            tipo_dir = percorrer_e_checar(no->dir); /* Tipo da expressão calculada */
            if (strcmp(tipo_esq, tipo_dir) != 0) {
                printf("ERRO SEMANTICO: Atribuicao de tipos incompativeis ('%s' = '%s') - linha: %d\n", tipo_esq, tipo_dir, no->linha);
                exit(1);
            }
            return tipo_esq;

        case NODE_OP_BINARIA:
            tipo_esq = percorrer_e_checar(no->esq);
            tipo_dir = percorrer_e_checar(no->dir);
            
            /* Operações matemáticas e relacionais exigem tipos iguais */
            if (strcmp(tipo_esq, tipo_dir) != 0) {
                printf("ERRO SEMANTICO: Operacao entre tipos incompativeis ('%s' e '%s') - linha: %d\n", tipo_esq, tipo_dir, no->linha);
                exit(1);
            }

            /* Operadores aritméticos devem ser aplicados apenas em int */
            if (strchr("+-*/", no->lexema[0]) && strcmp(tipo_esq, "int") != 0) {
                printf("ERRO SEMANTICO: Operador aritmetico aplicado a tipo nao inteiro - linha: %d\n", no->linha);
                exit(1);
            }
            
            /* Toda expressão relacional ou lógica resulta em um tipo int (verdadeiro/falso) na linguagem G-V1 */
            return "int";

        case NODE_LEIA:
        case NODE_ESCREVA:
        case NODE_OP_UNARIA:
            percorrer_e_checar(no->esq);
            break;

        case NODE_SE:
        case NODE_ENQUANTO:
            tipo_esq = percorrer_e_checar(no->condicao);
            if (strcmp(tipo_esq, "int") != 0) {
                printf("ERRO SEMANTICO: Condicao de laco/if deve ser uma expressao inteira ou relacional - linha: %d\n", no->linha);
                exit(1);
            }
            percorrer_e_checar(no->esq); /* Bloco interno do if/while */
            if (no->senao) percorrer_e_checar(no->senao);
            break;
            
        default:
            break;
    }

    /* Continua o percurso horizontal se for uma lista de comandos ou declarações */
    if (no->prox) {
        percorrer_e_checar(no->prox);
    }
    return NULL;
}

void analisar_semantica(NoAST *raiz) {
    iniciar_pilha();
    percorrer_e_checar(raiz);
    printf(">> Analise Semantica concluida com sucesso! Nenhum erro encontrado.\n");
}