%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantico.h"
#include "gerador.h"

extern int yylineno;
extern char* yytext;
extern int yylex();
extern FILE* yyin;

void yyerror(char const *s);

NoAST* raiz_ast = NULL; /* Raiz global da nossa árvore */
%}

%code requires {
    #include "ast.h"
}

/* O %union permite que tokens e regras retornem strings ou nós da árvore */
%union {
    char* str;
    NoAST* no;
}

%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA SE ENTAO SENAO FIMSE ENQUANTO
%token <str> IDENTIFICADOR CADEIACARACTERES INTCONST CARCONST
%token <str> OU E IGUAL DIFERENTE MAIORIGUAL MENORIGUAL

/* Tipamos as regras não-terminais para retornarem ponteiros de NoAST */
%type <no> Programa DeclPrograma Bloco VarSection ListaDeclVar DeclVar DeclVar_E Tipo
%type <no> ListaComando Comando Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr

%expect 1 
%glr-parser

%%

Programa: DeclPrograma { raiz_ast = $1; }
        ;

DeclPrograma: PRINCIPAL Bloco { $$ = criaNo1(NODE_PROGRAMA, $2, yylineno); }
            ;

Bloco: '{' VarSection ListaComando '}' { 
         NoAST* b = criaNoFolha(NODE_BLOCO, NULL, yylineno);
         b->esq = $2; 
         b->dir = $3; 
         $$ = b;
       }
     | '{' ListaComando '}' {
         NoAST* b = criaNoFolha(NODE_BLOCO, NULL, yylineno);
         b->dir = $2; 
         $$ = b;
       }
     ;

VarSection: ListaDeclVar { $$ = $1; }
          ;

ListaDeclVar: DeclVar ListaDeclVar { $$ = concatenaLista($1, $2); }
            | DeclVar { $$ = $1; }
            ;

DeclVar: IDENTIFICADOR DeclVar_E ':' Tipo ';' {
           NoAST* decl = criaNoFolha(NODE_DECL_VAR, $1, yylineno);
           decl->esq = $4;
           $$ = concatenaLista(decl, $2);
       }
       ;

DeclVar_E: ',' IDENTIFICADOR DeclVar_E {
             NoAST* decl = criaNoFolha(NODE_DECL_VAR, $2, yylineno);
             $$ = concatenaLista(decl, $3);
         }
         | /* Vazio */ { $$ = NULL; }
         ;

Tipo: INT { $$ = criaNoFolha(NODE_TIPO, "int", yylineno); }
    | CAR { $$ = criaNoFolha(NODE_TIPO, "car", yylineno); }
    ;

ListaComando: Comando ListaComando { $$ = concatenaLista($1, $2); }
            | Comando { $$ = $1; }
            ;

Comando: Expr ';' { $$ = $1; }
       | LEIA IDENTIFICADOR ';' { $$ = criaNoFolha(NODE_LEIA, $2, yylineno); }
       | ESCREVA Expr ';' { $$ = criaNo1(NODE_ESCREVA, $2, yylineno); }
       | ESCREVA CADEIACARACTERES ';' { 
           NoAST* str_no = criaNoFolha(NODE_STR, $2, yylineno);
           $$ = criaNo1(NODE_ESCREVA, str_no, yylineno); 
         }
       | NOVALINHA ';' { $$ = criaNoFolha(NODE_NOVALINHA, NULL, yylineno); }
       | SE '(' Expr ')' ENTAO Comando FIMSE { $$ = criaNoSe($3, $6, NULL, yylineno); }
       | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE { $$ = criaNoSe($3, $6, $8, yylineno); }
       | ENQUANTO '(' Expr ')' Comando { $$ = criaNoEnquanto($3, $5, yylineno); }
       | Bloco { $$ = $1; }
       ;

Expr: OrExpr { $$ = $1; }
    | IDENTIFICADOR '=' Expr { 
        NoAST* id = criaNoFolha(NODE_ID, $1, yylineno);
        $$ = criaNo2(NODE_ATRIB, "=", id, $3, yylineno); 
      }
    ;

OrExpr: OrExpr OU AndExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
      | AndExpr { $$ = $1; }
      ;

AndExpr: AndExpr E EqExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
       | EqExpr { $$ = $1; }
       ;

EqExpr: EqExpr IGUAL DesigExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
      | EqExpr DIFERENTE DesigExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
      | DesigExpr { $$ = $1; }
      ;

DesigExpr: DesigExpr '<' AddExpr { $$ = criaNo2(NODE_OP_BINARIA, "<", $1, $3, yylineno); }
         | DesigExpr '>' AddExpr { $$ = criaNo2(NODE_OP_BINARIA, ">", $1, $3, yylineno); }
         | DesigExpr MAIORIGUAL AddExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
         | DesigExpr MENORIGUAL AddExpr { $$ = criaNo2(NODE_OP_BINARIA, $2, $1, $3, yylineno); }
         | AddExpr { $$ = $1; }
         ;

AddExpr: AddExpr '+' MulExpr { $$ = criaNo2(NODE_OP_BINARIA, "+", $1, $3, yylineno); }
       | AddExpr '-' MulExpr { $$ = criaNo2(NODE_OP_BINARIA, "-", $1, $3, yylineno); }
       | MulExpr { $$ = $1; }
       ;

MulExpr: MulExpr '*' UnExpr { $$ = criaNo2(NODE_OP_BINARIA, "*", $1, $3, yylineno); }
       | MulExpr '/' UnExpr { $$ = criaNo2(NODE_OP_BINARIA, "/", $1, $3, yylineno); }
       | UnExpr { $$ = $1; }
       ;

UnExpr: '-' PrimExpr { $$ = criaNo1(NODE_OP_UNARIA, $2, yylineno); $$->lexema = strdup("-"); }
      | '!' PrimExpr { $$ = criaNo1(NODE_OP_UNARIA, $2, yylineno); $$->lexema = strdup("!"); }
      | PrimExpr { $$ = $1; }
      ;

PrimExpr: IDENTIFICADOR { $$ = criaNoFolha(NODE_ID, $1, yylineno); }
        | CARCONST { $$ = criaNoFolha(NODE_CAR, $1, yylineno); }
        | INTCONST { $$ = criaNoFolha(NODE_INT, $1, yylineno); }
        | '(' Expr ')' { $$ = $2; }
        ;

%%

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Uso correto: ./g-v1 nome_arq_entrada\n");
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Arquivo não pode ser aberto.\n");
        return 1;
    }
    yyparse();
    fclose(yyin);
    
    printf("\n--- ARVORE SINTATICA ABSTRATA (AST) ---\n");
    imprimeAST(raiz_ast, 0);
    printf("---------------------------------------\n");

    analisar_semantica(raiz_ast);

    gerar_codigo(raiz_ast);

    return 0;
}

void yyerror(char const *s) {
    printf("ERRO: Erro sintatico proximo a %s - linha: %d\n", yytext, yylineno);
}