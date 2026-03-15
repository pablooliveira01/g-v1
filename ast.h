#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAMA, NODE_BLOCO, NODE_DECL_VAR, NODE_TIPO,
    NODE_ATRIB, NODE_LEIA, NODE_ESCREVA, NODE_NOVALINHA,
    NODE_SE, NODE_ENQUANTO,
    NODE_OP_BINARIA, NODE_OP_UNARIA,
    NODE_ID, NODE_INT, NODE_CAR, NODE_STR
} TipoNo;

typedef struct NoAST {
    TipoNo tipo;
    int linha;
    char *lexema;          /* Armazena o identificador, número, string ou operador */
    struct NoAST *esq;     /* Filho esquerdo (ex: lado esquerdo de uma soma) */
    struct NoAST *dir;     /* Filho direito (ex: lado direito de uma soma) */
    struct NoAST *condicao;/* Condição para if/while */
    struct NoAST *senao;   /* Bloco do else (se houver) */
    struct NoAST *prox;    /* Ponteiro para o próximo comando/declaração na lista */
} NoAST;

/* Funções de construção */
NoAST* criaNoFolha(TipoNo tipo, char* lexema, int linha);
NoAST* criaNo1(TipoNo tipo, NoAST* esq, int linha);
NoAST* criaNo2(TipoNo tipo, char* lexema, NoAST* esq, NoAST* dir, int linha);
NoAST* criaNoSe(NoAST* condicao, NoAST* cmd_entao, NoAST* cmd_senao, int linha);
NoAST* criaNoEnquanto(NoAST* condicao, NoAST* comando, int linha);
NoAST* concatenaLista(NoAST* lista, NoAST* novo_no);

/* Função para validar visualmente a árvore */
void imprimeAST(NoAST* raiz, int nivel);

#endif