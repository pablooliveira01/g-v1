#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#define TAM_HASH 211 /* Um número primo para espalhar bem as variáveis na tabela hash */

/* Representa uma variável armazenada na tabela */
typedef struct Simbolo {
    char *lexema;         /* O nome da variável (ex: "x", "letra") */
    char *tipo;           /* O tipo ("int" ou "car") */
    int linha_decl;       /* A linha onde foi declarada */
    int offset;           /* Serve para guardar a posição de memória*/
    struct Simbolo *prox; /* Ponteiro para tratar colisões na tabela hash */
} Simbolo;

/* Representa um escopo (um bloco do programa) */
typedef struct Escopo {
    Simbolo *tabela_hash[TAM_HASH]; /* A tabela de variáveis deste escopo específico */
    struct Escopo *abaixo;          /* Ponteiro para o escopo anterior (abaixo na pilha) */
} Escopo;

/* As 4 operações obrigatórias exigidas na especificação */
void iniciar_pilha();                                   /* a) Iniciar a pilha vazia */
void empilhar_escopo();                                 /* b) Criar nova tabela e empilhar */
Simbolo* pesquisar_simbolo(char *lexema);               /* c) Procurar do topo para a base */
void remover_escopo();                                  /* d) Eliminar a tabela do topo */

/* Função extra essencial para adicionar a variável na tabela do topo */
Simbolo* inserir_simbolo(char *lexema, char *tipo, int linha);

#endif